/*
 * Copyright 2022 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <errno.h>
#include "bio_local.h"
#include "internal/cryptlib.h"

/* ===========================================================================
 * Byte-wise ring buffer which supports pushing and popping blocks of multiple
 * bytes at a time.
 */
struct ring_buf {
    void *start; /* start of buffer, never changes */
    size_t len; /* size of buffer allocation in bytes */
    size_t count; /* number of bytes currently pushed */
    /*
     * These index into start. Where idx[0] == idx[1], the buffer is full
     * (if count is nonzero) and empty otherwise.
     */
    size_t idx[2]; /* 0: head, 1: tail */
};

static int ring_buf_init(struct ring_buf *r, size_t nbytes)
{
    r->start = OPENSSL_malloc(nbytes);
    if (r->start == NULL)
        return 0;

    r->len = nbytes;
    r->idx[0] = r->idx[1] = r->count = 0;
    return 1;
}

static void ring_buf_destroy(struct ring_buf *r)
{
    OPENSSL_free(r->start);
    r->start    = NULL;
    r->len      = 0;
    r->count    = 0;
}

/*
 * Get a pointer to the next place to write data to be pushed to the ring buffer
 * (idx=0), or the next data to be popped from the ring buffer (idx=1). The
 * pointer is written to *buf and the maximum number of bytes which can be
 * read/written are written to *len. After writing data to the buffer, call
 * ring_buf_push/pop() with the number of bytes actually read/written, which
 * must not exceed the returned length.
 */
static void ring_buf_head_tail(struct ring_buf *r, int idx, uint8_t **buf, size_t *len)
{
    size_t max_len = r->len - r->idx[idx];
    if (!idx && max_len > r->len - r->count)
        max_len = r->len - r->count;
    if (idx && max_len > r->count)
        max_len = r->count;

    *buf = (uint8_t *)r->start + r->idx[idx];
    *len = max_len;
}

#define ring_buf_head(r, buf, len) ring_buf_head_tail((r), 0, (buf), (len))
#define ring_buf_tail(r, buf, len) ring_buf_head_tail((r), 1, (buf), (len))

/*
 * Commit bytes to the ring buffer previously filled after a call to
 * ring_buf_head().
 */
static void ring_buf_push_pop(struct ring_buf *r, int idx, size_t num_bytes)
{
    size_t new_idx;

    /* A single push/pop op cannot wrap around, though it can reach the end.
     * If the caller adheres to the convention of using the length returned
     * by ring_buf_head/tail(), this cannot happen.
     */
    if (!ossl_assert(num_bytes <= r->len - r->idx[idx]))
        return;

    /*
     * Must not overfill the buffer, or pop more than is in the buffer either.
     */
    if (!ossl_assert(idx ? num_bytes <= r->count
                         : num_bytes + r->count <= r->len))
        return;

    /* Update the index. */
    new_idx = r->idx[idx] + num_bytes;
    if (new_idx == r->len)
        new_idx = 0;

    r->idx[idx] = new_idx;
    if (idx)
        r->count -= num_bytes;
    else
        r->count += num_bytes;
}

#define ring_buf_push(r, num_bytes) ring_buf_push_pop((r), 0, (num_bytes))
#define ring_buf_pop(r, num_bytes) ring_buf_push_pop((r), 1, (num_bytes))

/* Returns number of bytes currently queued in the buffer. */
static ossl_unused ossl_inline size_t ring_buf_count(const struct ring_buf *r)
{
    return r->count;
}

static void ring_buf_clear(struct ring_buf *r)
{
    r->idx[0] = r->idx[1] = r->count = 0;
}

/* ===========================================================================
 * BIO_s_dgram_pair is documented in BIO_s_dgram_pair(3).
 *
 * INTERNAL DATA STRUCTURE
 *
 * This is managed internally by using a bytewise ring buffer which supports
 * pushing and popping spans of multiple bytes at once. The ring buffer stores
 * internal packets which look like this:
 *
 *   struct dgram_hdr hdr;
 *   uint8_t data[];
 *
 * The header contains the length of the data and metadata such as
 * source/destination addresses.
 *
 * The datagram pair BIO is designed to support both traditional
 * BIO_read/BIO_write (likely to be used by applications) as well as
 * BIO_recvmmsg/BIO_sendmmsg.
 */
struct bio_dgram_pair_st;
static int dgram_pair_write(BIO *bio, const char *buf, int sz_);
static int dgram_pair_read(BIO *bio, char *buf, int sz_);
static long dgram_pair_ctrl(BIO *bio, int cmd, long num, void *ptr);
static int dgram_pair_init(BIO *bio);
static int dgram_pair_free(BIO *bio);
static ossl_ssize_t dgram_pair_sendmmsg(BIO *b, BIO_MSG *msg, size_t stride, size_t num_msg, uint64_t flags);
static ossl_ssize_t dgram_pair_recvmmsg(BIO *b, BIO_MSG *msg, size_t stride, size_t num_msg, uint64_t flags);

static int dgram_pair_ctrl_destroy_bio_pair(BIO *bio1);
static size_t dgram_pair_read_inner(struct bio_dgram_pair_st *b, uint8_t *buf, size_t sz);

#define BIO_MSG_N(array, n) (*(BIO_MSG *)((char *)(array) + (n)*stride))

static const BIO_METHOD dgram_pair_method = {
    BIO_TYPE_DGRAM_PAIR,
    "BIO dgram pair",
    bwrite_conv,
    dgram_pair_write,
    bread_conv,
    dgram_pair_read,
    NULL, /* dgram_pair_puts */
    NULL, /* dgram_pair_gets */
    dgram_pair_ctrl,
    dgram_pair_init,
    dgram_pair_free,
    NULL, /* dgram_pair_callback_ctrl */
    dgram_pair_sendmmsg,
    dgram_pair_recvmmsg,
};

const BIO_METHOD *BIO_s_dgram_pair(void)
{
    return &dgram_pair_method;
}

struct dgram_hdr {
    size_t len; /* payload length in bytes, not including this struct */
    BIO_ADDR src_addr, dst_addr; /* family == 0: not present */
};

struct bio_dgram_pair_st {
    /* The other half of the BIO pair. */
    BIO *peer;
    /* Writes are directed to our own ringbuf and reads to our peer. */
    struct ring_buf rbuf;
    /* Requested size of rbuf buffer in bytes once we initialize. */
    size_t req_buf_len;
    /* Largest possible datagram size */
    size_t mtu;
    /* Capability flags. */
    uint32_t cap;
    /*
     * This lock protects updates to our rbuf. Since writes are directed to our
     * own rbuf, this means we use this lock for writes and our peer's lock for
     * reads.
     */
    CRYPTO_RWLOCK *lock;
    unsigned char no_trunc          : 1; /* Reads fail if they would truncate */
    unsigned char local_addr_enable : 1; /* Can use BIO_MSG->local? */
};

#define MIN_BUF_LEN (1024)

static int dgram_pair_init(BIO *bio)
{
    struct bio_dgram_pair_st *b = OPENSSL_zalloc(sizeof(*b));

    if (b == NULL)
        return 0;

    b->req_buf_len = 17*1024; /* default buffer size */
    b->mtu         = 1472;    /* conservative default MTU */

    b->lock = CRYPTO_THREAD_lock_new();
    if (b->lock == NULL) {
        OPENSSL_free(b);
        return 0;
    }

    bio->ptr = b;
    return 1;
}

static int dgram_pair_free(BIO *bio)
{
    struct bio_dgram_pair_st *b;

    if (bio == NULL)
        return 0;

    b = bio->ptr;
    if (!ossl_assert(b != NULL))
        return 0;

    /* We are being freed. Disconnect any peer and destroy buffers. */
    dgram_pair_ctrl_destroy_bio_pair(bio);

    CRYPTO_THREAD_lock_free(b->lock);
    OPENSSL_free(b);
    return 1;
}

/* BIO_make_bio_pair (BIO_C_MAKE_BIO_PAIR) */
static int dgram_pair_ctrl_make_bio_pair(BIO *bio1, BIO *bio2)
{
    struct bio_dgram_pair_st *b1, *b2;

    /* peer must be non-NULL. */
    if (bio1 == NULL || bio2 == NULL)
        return 0;

    /* Ensure the BIO we have been passed is actually a dgram pair BIO. */
    if (bio1->method != &dgram_pair_method || bio2->method != &dgram_pair_method)
        return 0;

    b1 = bio1->ptr;
    b2 = bio2->ptr;

    if (!ossl_assert(b1 != NULL && b2 != NULL))
        return 0;

    /*
     * This ctrl cannot be used to associate a BIO pair half which is already
     * associated.
     */
    if (b1->peer != NULL || b2->peer != NULL)
        return 0;

    if (!ossl_assert(b1->req_buf_len >= MIN_BUF_LEN
                        && b2->req_buf_len >= MIN_BUF_LEN))
        return 0;

    if (b1->rbuf.len != b1->req_buf_len)
        if (ring_buf_init(&b1->rbuf, b1->req_buf_len) == 0) {
            ERR_raise(ERR_LIB_BIO, ERR_R_MALLOC_FAILURE);
            return 0;
        }

    if (b2->rbuf.len != b2->req_buf_len)
        if (ring_buf_init(&b2->rbuf, b2->req_buf_len) == 0) {
            ERR_raise(ERR_LIB_BIO, ERR_R_MALLOC_FAILURE);
            ring_buf_destroy(&b1->rbuf);
            return 0;
        }

    b1->peer    = bio2;
    b2->peer    = bio1;
    bio1->init  = 1;
    bio2->init  = 1;
    return 1;
}

/* BIO_destroy_bio_pair (BIO_C_DESTROY_BIO_PAIR) */
static int dgram_pair_ctrl_destroy_bio_pair(BIO *bio1)
{
    BIO *bio2;
    struct bio_dgram_pair_st *b1 = bio1->ptr, *b2;

    /* If we already don't have a peer, treat this as a no-op. */
    if (b1->peer == NULL)
        return 1;

    bio2 = b1->peer;
    b2 = bio2->ptr;

    /* Invariants. */
    if (!ossl_assert(b1->peer == bio2 && b2->peer == bio1))
        return 0;

    /* Free buffers. */
    ring_buf_destroy(&b1->rbuf);
    ring_buf_destroy(&b2->rbuf);

    bio1->init = 0;
    bio2->init = 0;
    b1->peer = NULL;
    b2->peer = NULL;
    return 1;
}

/* BIO_eof (BIO_CTRL_EOF) */
static int dgram_pair_ctrl_eof(BIO *bio)
{
    struct bio_dgram_pair_st *b = bio->ptr, *peerb;

    if (!ossl_assert(b != NULL))
        return -1;

    /* If we have no peer, we can never read anything */
    if (b->peer == NULL)
        return 1;

    peerb = b->peer->ptr;
    if (!ossl_assert(peerb != NULL))
        return -1;

    /* 
     * Since we are emulating datagram semantics, never indicate EOF so long as
     * we have a peer.
     */
    return 0;
}

/* BIO_set_write_buf_size (BIO_C_SET_WRITE_BUF_SIZE) */
static int dgram_pair_ctrl_set_write_buf_size(BIO *bio, size_t len)
{
    struct bio_dgram_pair_st *b = bio->ptr;

    /* Changing buffer sizes is not permitted while a peer is connected. */
    if (b->peer != NULL) {
        ERR_raise(ERR_LIB_BIO, BIO_R_IN_USE);
        return 0;
    }

    /* Enforce minimum size. */
    if (len < MIN_BUF_LEN)
        len = MIN_BUF_LEN;

    /*
     * We have no peer yet, therefore the ring buffer should not have been
     * allocated yet.
     */
    if (!ossl_assert(b->rbuf.start == NULL))
        return 0;

    b->req_buf_len = len;
    return 1;
}

/* BIO_reset (BIO_CTRL_RESET) */
static int dgram_pair_ctrl_reset(BIO *bio)
{
    struct bio_dgram_pair_st *b = bio->ptr;

    ring_buf_clear(&b->rbuf);
    return 1;
}

/* BIO_pending (BIO_CTRL_PENDING) (Threadsafe) */
static size_t dgram_pair_ctrl_pending(BIO *bio)
{
    size_t saved_idx, saved_count;
    struct bio_dgram_pair_st *b = bio->ptr, *peerb;
    struct dgram_hdr hdr;
    size_t l;

    /* Safe to check; peer may not change during this call */
    if (b->peer == NULL)
        return 0;

    peerb = b->peer->ptr;

    if (CRYPTO_THREAD_read_lock(peerb->lock) == 0)
        return 0;

    saved_idx   = peerb->rbuf.idx[1];
    saved_count = peerb->rbuf.count;

    l = dgram_pair_read_inner(peerb, (uint8_t *)&hdr, sizeof(hdr));

    peerb->rbuf.idx[1] = saved_idx;
    peerb->rbuf.count  = saved_count;

    CRYPTO_THREAD_unlock(peerb->lock);

    if (!ossl_assert(l == 0 || l == sizeof(hdr)))
        /* Should not be possible */
        return 0;

    return l > 0 ? hdr.len : 0;
}

/* BIO_get_write_guarantee (BIO_C_GET_WRITE_GUARANTEE) (Threadsafe) */
static size_t dgram_pair_ctrl_get_write_guarantee(BIO *bio)
{
    size_t l;
    struct bio_dgram_pair_st *b = bio->ptr;

    if (CRYPTO_THREAD_read_lock(b->lock) == 0)
        return 0;

    l = b->rbuf.len - b->rbuf.count;
    if (l >= sizeof(struct dgram_hdr))
        l -= sizeof(struct dgram_hdr);

    /*
     * If the amount of buffer space would not be enough to accommodate the
     * worst-case size of a datagram, report no space available.
     */
    if (l < b->mtu)
        l = 0;

    CRYPTO_THREAD_unlock(b->lock);
    return l;
}

/* BIO_dgram_get_local_addr_cap (BIO_CTRL_DGRAM_GET_LOCAL_ADDR_CAP) */
static int dgram_pair_ctrl_get_local_addr_cap(BIO *bio)
{
    struct bio_dgram_pair_st *b = bio->ptr, *peerb;

    if (b->peer == NULL)
        return 0;

    peerb = b->peer->ptr;

    return (~peerb->cap & (BIO_DGRAM_CAP_HANDLES_SRC_ADDR
                          |BIO_DGRAM_CAP_PROVIDES_DST_ADDR)) == 0;
}

/* BIO_dgram_get_effective_caps (BIO_CTRL_DGRAM_GET_EFFECTIVE_CAPS) */
static int dgram_pair_ctrl_get_effective_caps(BIO *bio)
{
    struct bio_dgram_pair_st *b = bio->ptr, *peerb;

    if (b->peer == NULL)
        return 0;

    peerb = b->peer->ptr;

    return peerb->cap;
}

/* BIO_dgram_get_caps (BIO_CTRL_DGRAM_GET_CAPS) */
static uint32_t dgram_pair_ctrl_get_caps(BIO *bio)
{
    struct bio_dgram_pair_st *b = bio->ptr;

    return b->cap;
}

/* BIO_dgram_set_caps (BIO_CTRL_DGRAM_SET_CAPS) */
static int dgram_pair_ctrl_set_caps(BIO *bio, uint32_t caps)
{
    struct bio_dgram_pair_st *b = bio->ptr;

    b->cap = caps;
    return 1;
}

/* BIO_dgram_get_local_addr_enable (BIO_CTRL_DGRAM_GET_LOCAL_ADDR_ENABLE) */
static int dgram_pair_ctrl_get_local_addr_enable(BIO *bio)
{
    struct bio_dgram_pair_st *b = bio->ptr;
    return b->local_addr_enable;
}

/* BIO_dgram_set_local_addr_enable (BIO_CTRL_DGRAM_SET_LOCAL_ADDR_ENABLE) */
static int dgram_pair_ctrl_set_local_addr_enable(BIO *bio, int enable)
{
    struct bio_dgram_pair_st *b = bio->ptr;

    if (dgram_pair_ctrl_get_local_addr_cap(bio) == 0)
        return 0;

    b->local_addr_enable = (enable != 0 ? 1 : 0);
    return 1;
}

/* BIO_dgram_get_mtu (BIO_CTRL_DGRAM_GET_MTU) */
static int dgram_pair_ctrl_get_mtu(BIO *bio)
{
    struct bio_dgram_pair_st *b = bio->ptr;

    return b->mtu;
}

/* BIO_dgram_set_mtu (BIO_CTRL_DGRAM_SET_MTU) */
static int dgram_pair_ctrl_set_mtu(BIO *bio, size_t mtu)
{
    struct bio_dgram_pair_st *b = bio->ptr, *peerb;

    b->mtu = mtu;

    if (b->peer != NULL) {
        peerb = b->peer->ptr;
        peerb->mtu = mtu;
    }

    return 1;
}

/* Partially threadsafe (some commands) */
static long dgram_pair_ctrl(BIO *bio, int cmd, long num, void *ptr)
{
    long ret = 1;
    struct bio_dgram_pair_st *b = bio->ptr;

    if (!ossl_assert(b != NULL))
        return 0;

    switch (cmd) {

    /*
     * BIO_set_write_buf_size: Set the size of the ring buffer used for storing
     * datagrams. No more writes can be performed once the buffer is filled up,
     * until reads are performed. This cannot be used after a peer is connected.
     */
    case BIO_C_SET_WRITE_BUF_SIZE: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_set_write_buf_size(bio, (size_t)num);
        break;

    /*
     * BIO_get_write_buf_size: Get ring buffer size.
     */
    case BIO_C_GET_WRITE_BUF_SIZE: /* Non-threadsafe */
        ret = (long)b->req_buf_len;
        break;

    /*
     * BIO_make_bio_pair: this is usually used by BIO_new_dgram_pair, though it
     * may be used manually after manually creating each half of a BIO pair
     * using BIO_new. This only needs to be called on one of the BIOs.
     */
    case BIO_C_MAKE_BIO_PAIR: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_make_bio_pair(bio, (BIO *)ptr);
        break;

    /*
     * BIO_destroy_bio_pair: Manually disconnect two halves of a BIO pair so
     * that they are no longer peers.
     */
    case BIO_C_DESTROY_BIO_PAIR: /* Non-threadsafe */
        dgram_pair_ctrl_destroy_bio_pair(bio);
        break;

    /*
     * BIO_reset: Clear all data which was written to this side of the pair.
     */
    case BIO_CTRL_RESET: /* Non-threadsafe */
        dgram_pair_ctrl_reset(bio);
        break;

    /*
     * BIO_get_write_guarantee: Any BIO_write providing a buffer less than or
     * equal to this value is guaranteed to succeed.
     */
    case BIO_C_GET_WRITE_GUARANTEE: /* Threadsafe */
        ret = (long)dgram_pair_ctrl_get_write_guarantee(bio);
        break;

    /* BIO_pending: Bytes available to read. */
    case BIO_CTRL_PENDING: /* Threadsafe */
        ret = (long)dgram_pair_ctrl_pending(bio);
        break;

    /* BIO_flush: No-op. */
    case BIO_CTRL_FLUSH: /* Threadsafe */
        break;

    /* BIO_dgram_get_no_trunc */
    case BIO_CTRL_DGRAM_GET_NO_TRUNC: /* Non-threadsafe */
        ret = (long)b->no_trunc;
        break;

    /* BIO_dgram_set_no_trunc */
    case BIO_CTRL_DGRAM_SET_NO_TRUNC: /* Non-threadsafe */
        b->no_trunc = (num > 0);
        break;

    /* BIO_dgram_get_local_addr_enable */
    case BIO_CTRL_DGRAM_GET_LOCAL_ADDR_ENABLE: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_get_local_addr_enable(bio);
        break;

    /* BIO_dgram_set_local_addr_enable */
    case BIO_CTRL_DGRAM_SET_LOCAL_ADDR_ENABLE: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_set_local_addr_enable(bio, num);
        break;

    /* BIO_dgram_get_local_addr_cap: Can local addresses be supported? */
    case BIO_CTRL_DGRAM_GET_LOCAL_ADDR_CAP: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_get_local_addr_cap(bio);
        break;

    /* BIO_dgram_get_effective_caps */
    case BIO_CTRL_DGRAM_GET_EFFECTIVE_CAPS: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_get_effective_caps(bio);
        break;

    /* BIO_dgram_get_caps */
    case BIO_CTRL_DGRAM_GET_CAPS: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_get_caps(bio);
        break;

    /* BIO_dgram_set_caps */
    case BIO_CTRL_DGRAM_SET_CAPS: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_set_caps(bio, (uint32_t)num);
        break;

    /* BIO_dgram_get_mtu */
    case BIO_CTRL_DGRAM_GET_MTU: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_get_mtu(bio);
        break;

    /* BIO_dgram_set_mtu */
    case BIO_CTRL_DGRAM_SET_MTU: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_set_mtu(bio, (uint32_t)num);
        break;

    /* 
     * BIO_eof: Returns whether this half of the BIO pair is empty of data to
     * read.
     */
    case BIO_CTRL_EOF: /* Non-threadsafe */
        ret = (long)dgram_pair_ctrl_eof(bio);
        break;

    default:
        ret = 0;
        break;
    }

    return ret;
}

int BIO_new_bio_dgram_pair(BIO **pbio1, size_t writebuf1,
                           BIO **pbio2, size_t writebuf2)
{
    int ret = 0;
    long r;
    BIO *bio1 = NULL, *bio2 = NULL;

    bio1 = BIO_new(BIO_s_dgram_pair());
    if (bio1 == NULL)
        goto err;

    bio2 = BIO_new(BIO_s_dgram_pair());
    if (bio2 == NULL)
        goto err;

    if (writebuf1) {
        r = BIO_set_write_buf_size(bio1, writebuf1);
        if (r == 0)
            goto err;
    }

    if (writebuf2) {
        r = BIO_set_write_buf_size(bio2, writebuf2);
        if (r == 0)
            goto err;
    }

    r = BIO_make_bio_pair(bio1, bio2);
    if (r == 0)
        goto err;

    ret = 1;
err:
    if (ret == 0) {
        BIO_free(bio1);
        bio1 = NULL;
        BIO_free(bio2);
        bio2 = NULL;
    }

    *pbio1 = bio1;
    *pbio2 = bio2;
    return ret;
}

/* Must hold peer write lock */
static size_t dgram_pair_read_inner(struct bio_dgram_pair_st *b, uint8_t *buf, size_t sz)
{
    size_t total_read = 0;

    /*
     * We repeat pops from the ring buffer for as long as we have more
     * application *buffer to fill until we fail. We may not be able to pop
     * enough data to fill the buffer in one operation if the ring buffer wraps
     * around, but there may still be more data available.
     */
    while (sz > 0) {
        uint8_t *src_buf = NULL;
        size_t src_len = 0;

        /*
         * There are two BIO instances, each with a ringbuf. We read from the
         * peer ringbuf and write to our own ringbuf.
         */
        ring_buf_tail(&b->rbuf, &src_buf, &src_len);
        if (src_len == 0)
            break;

        if (src_len > sz)
            src_len = sz;

        if (buf != NULL)
            memcpy(buf, src_buf, src_len);

        ring_buf_pop(&b->rbuf, src_len);

        buf         += src_len;
        total_read  += src_len;
        sz          -= src_len;
    }

    return total_read;
}

/* Must hold peer write lock */
static ossl_ssize_t dgram_pair_read_actual(BIO *bio, char *buf, size_t sz,
                                           BIO_ADDR *local, BIO_ADDR *peer,
                                           int is_multi)
{
    size_t l, trunc = 0, saved_idx, saved_count;
    struct bio_dgram_pair_st *b = bio->ptr, *peerb;
    struct dgram_hdr hdr;

    if (!is_multi)
        BIO_clear_retry_flags(bio);

    if (!bio->init)
        return -1;

    if (!ossl_assert(b != NULL && b->peer != NULL))
        return -1;

    peerb = b->peer->ptr;
    if (!ossl_assert(peerb != NULL && peerb->rbuf.start != NULL))
        return -1;

    if (sz > 0 && buf == NULL)
        return -1;

    /* If the caller wants to know the local address, it must be enabled */
    if (local != NULL && b->local_addr_enable == 0)
        return -1;

    /* Read the header. */
    saved_idx   = peerb->rbuf.idx[1];
    saved_count = peerb->rbuf.count;
    l = dgram_pair_read_inner(peerb, (uint8_t *)&hdr, sizeof(hdr));
    if (l == 0) {
        /* Buffer was empty. */
        if (!is_multi)
            BIO_set_retry_read(bio);
        return -1;
    }

    if (!ossl_assert(l == sizeof(hdr)))
        /*
         * This should not be possible as headers (and their following payloads)
         * should always be written atomically.
         */
        return -1;

    if (sz > hdr.len)
        sz = hdr.len;
    else if (sz < hdr.len) {
        /* Truncation is occurring. */
        trunc = hdr.len - sz;
        if (b->no_trunc) {
            /* Restore original state. */
            peerb->rbuf.idx[1] = saved_idx;
            peerb->rbuf.count  = saved_count;
            return -1;
        }
    }

    l = dgram_pair_read_inner(peerb, (uint8_t *)buf, sz);
    if (!ossl_assert(l == sz))
        /* We were somehow not able to read the entire datagram. */
        return -1;

    /*
     * If the datagram was truncated due to an inadequate buffer, discard the
     * remainder.
     */
    if (trunc > 0)
        if (!ossl_assert(dgram_pair_read_inner(peerb, NULL, trunc) == trunc))
            /* We were somehow not able to read/skip the entire datagram. */
            return -1;

    if (local != NULL)
        *local = hdr.dst_addr;
    if (peer != NULL)
        *peer  = hdr.src_addr;

    return (ossl_ssize_t)l;
}

/* Threadsafe */
static int dgram_pair_read(BIO *bio, char *buf, int sz_)
{
    int ret;
    struct bio_dgram_pair_st *b = bio->ptr, *peerb;

    if (sz_ < 0)
        return -1;

    if (b->peer == NULL)
        return -1;

    peerb = b->peer->ptr;

    /*
     * For BIO_read we have to acquire both locks because we touch the retry
     * flags on the local bio. (This is avoided in the recvmmsg case as it does
     * not touch the retry flags.)
     */
    if (CRYPTO_THREAD_lock_dual(peerb->lock, 1, b->lock, 1) == 0)
        return -1;

    ret = dgram_pair_read_actual(bio, buf, (size_t)sz_, NULL, NULL, 0);
    CRYPTO_THREAD_unlock(b->lock);
    CRYPTO_THREAD_unlock(peerb->lock);
    return ret;
}

/* Threadsafe */
static ossl_ssize_t dgram_pair_recvmmsg(BIO *bio, BIO_MSG *msg,
                                        size_t stride, size_t num_msg,
                                        uint64_t flags)
{
    ossl_ssize_t ret, l;
    BIO_MSG *m;
    size_t i;
    struct bio_dgram_pair_st *b = bio->ptr, *peerb;

    if (num_msg == 0)
        return 0;

    if (b->peer == NULL)
        return -1;

    peerb = b->peer->ptr;

    if (CRYPTO_THREAD_write_lock(peerb->lock) == 0)
        return -1;

    for (i=0; i<num_msg; ++i) {
        m = &BIO_MSG_N(msg, i);
        l = dgram_pair_read_actual(bio, m->data, m->data_len,
                                   m->local, m->peer, 1);
        if (l < 0) {
            ret = i > 0 ? (ossl_ssize_t)i : -1;
            goto out;
        }

        m->data_len = l;
        m->flags    = 0;
    }

    ret = i;
out:
    CRYPTO_THREAD_unlock(peerb->lock);
    return ret;
}

/* Must hold local write lock */
static size_t dgram_pair_write_inner(struct bio_dgram_pair_st *b, const uint8_t *buf, size_t sz)
{
    size_t total_written = 0;

    /*
     * We repeat pushes to the ring buffer for as long as we have data until we
     * fail. We may not be able to push in one operation if the ring buffer
     * wraps around, but there may still be more room for data.
     */
    while (sz > 0) {
        size_t dst_len;
        uint8_t *dst_buf;

        /* 
         * There are two BIO instances, each with a ringbuf. We write to our own
         * ringbuf and read from the peer ringbuf.
         */
        ring_buf_head(&b->rbuf, &dst_buf, &dst_len);
        if (dst_len == 0)
            break;

        if (dst_len > sz)
            dst_len = sz;

        memcpy(dst_buf, buf, dst_len);
        ring_buf_push(&b->rbuf, dst_len);

        buf             += dst_len;
        sz              -= dst_len;
        total_written   += dst_len;
    }

    return total_written;
}

/* Must hold local write lock */
static ossl_ssize_t dgram_pair_write_actual(BIO *bio, const char *buf, size_t sz,
                                            const BIO_ADDR *local, const BIO_ADDR *peer,
                                            int is_multi)
{
    static const BIO_ADDR zero_addr = {0};
    size_t saved_idx, saved_count;
    struct bio_dgram_pair_st *b = bio->ptr, *peerb;
    struct dgram_hdr hdr = {0};

    if (!is_multi)
        BIO_clear_retry_flags(bio);

    if (!bio->init)
        return -1;

    if (!ossl_assert(b != NULL && b->peer != NULL && b->rbuf.start != NULL))
        return -1;

    if (sz > 0 && buf == NULL)
        return -1;

    if (local != NULL && b->local_addr_enable == 0)
        return -1;

    peerb = b->peer->ptr;
    if (peer != NULL && (peerb->cap & BIO_DGRAM_CAP_HANDLES_DST_ADDR) == 0)
        return -1;

    hdr.len = sz;
    hdr.dst_addr = (peer != NULL ? *peer : zero_addr);
    hdr.src_addr = (local != NULL ? *local : zero_addr);

    saved_idx   = b->rbuf.idx[0];
    saved_count = b->rbuf.count;
    if (dgram_pair_write_inner(b, (const uint8_t *)&hdr, sizeof(hdr)) != sizeof(hdr)
            || dgram_pair_write_inner(b, (const uint8_t *)buf, sz) != sz) {
        /*
         * We were not able to push the header and the entirety of the payload
         * onto the ring buffer, so abort and roll back the ring buffer state.
         */
        b->rbuf.idx[0] = saved_idx;
        b->rbuf.count  = saved_count;
        if (!is_multi)
            BIO_set_retry_write(bio);
        return -1;
    }

    return sz;
}

/* Threadsafe */
static int dgram_pair_write(BIO *bio, const char *buf, int sz_)
{
    int ret;
    struct bio_dgram_pair_st *b = bio->ptr;

    if (sz_ < 0)
        return -1;

    if (CRYPTO_THREAD_write_lock(b->lock) == 0)
        return -1;

    ret = (int)dgram_pair_write_actual(bio, buf, (size_t)sz_, NULL, NULL, 0);
    CRYPTO_THREAD_unlock(b->lock);
    return ret;
}

/* Threadsafe */
static ossl_ssize_t dgram_pair_sendmmsg(BIO *bio, BIO_MSG *msg,
                                        size_t stride, size_t num_msg,
                                        uint64_t flags)
{
    ossl_ssize_t ret, l;
    BIO_MSG *m;
    size_t i;
    struct bio_dgram_pair_st *b = bio->ptr;

    if (num_msg == 0)
        return 0;

    if (CRYPTO_THREAD_write_lock(b->lock) == 0)
        return -1;

    for (i=0; i<num_msg; ++i) {
        m = &BIO_MSG_N(msg, i);
        l = dgram_pair_write_actual(bio, m->data, m->data_len, 
                                    m->local, m->peer, 1);
        if (l < 0) {
            ret = i ? (ossl_ssize_t)i : -1;
            goto out;
        }

        m->flags = 0;
    }

    ret = i;
out:
    CRYPTO_THREAD_unlock(b->lock);
    return ret;
}
