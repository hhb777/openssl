/*
 * Copyright 2019 The OpenSSL Project Authors. All Rights Reserved.
 * Copyright 2019 Red Hat, Inc.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/*
 * This implements https://csrc.nist.gov/publications/detail/sp/800-108/final
 * section 5.1 ("counter mode") and section 5.2 ("feedback mode") in both HMAC
 * and CMAC.  That document does not name the KDFs it defines; the name is
 * derived from
 * https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/Key-Derivation
 *
 * Note that section 5.3 ("double-pipeline mode") is not implemented, though
 * it would be possible to do so in the future.
 *
 * These versions all assume the counter is used.  It would be relatively
 * straightforward to expose a configuration handle should the need arise.
 *
 * Variable names attempt to match those of SP800-108.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <openssl/params.h>

#include "internal/cryptlib.h"
#include "crypto/evp.h"
#include "internal/numbers.h"
#include "prov/implementations.h"
#include "prov/provider_ctx.h"
#include "prov/provider_util.h"
#include "prov/providercommonerr.h"

#include "e_os.h"

#define MIN(a, b) ((a) < (b)) ? (a) : (b)

typedef enum {
    COUNTER = 0,
    FEEDBACK
} kbkdf_mode;

/* Our context structure. */
typedef struct {
    void *provctx;
    kbkdf_mode mode;
    EVP_MAC_CTX *ctx_init;

    /* Names are lowercased versions of those found in SP800-108. */
    unsigned char *ki;
    size_t ki_len;
    unsigned char *label;
    size_t label_len;
    unsigned char *context;
    size_t context_len;
    unsigned char *iv;
    size_t iv_len;
} KBKDF;

/* Definitions needed for typechecking. */
static OSSL_OP_kdf_newctx_fn kbkdf_new;
static OSSL_OP_kdf_freectx_fn kbkdf_free;
static OSSL_OP_kdf_reset_fn kbkdf_reset;
static OSSL_OP_kdf_derive_fn kbkdf_derive;
static OSSL_OP_kdf_settable_ctx_params_fn kbkdf_settable_ctx_params;
static OSSL_OP_kdf_set_ctx_params_fn kbkdf_set_ctx_params;

/* Not all platforms have htobe32(). */
static uint32_t be32(uint32_t host)
{
    uint32_t big = 0;
    const union {
        long one;
        char little;
    } is_endian = { 1 };

    if (!is_endian.little)
        return host;

    big |= (host & 0xff000000) >> 24;
    big |= (host & 0x00ff0000) >> 8;
    big |= (host & 0x0000ff00) << 8;
    big |= (host & 0x000000ff) << 24;
    return big;
}

static void *kbkdf_new(void *provctx)
{
    KBKDF *ctx;

    ctx = OPENSSL_zalloc(sizeof(*ctx));
    if (ctx == NULL) {
        ERR_raise(ERR_LIB_PROV, ERR_R_MALLOC_FAILURE);
        return NULL;
    }

    ctx->provctx = provctx;
    return ctx;
}

static void kbkdf_free(void *vctx)
{
    KBKDF *ctx = (KBKDF *)vctx;

    if (ctx != NULL) {
        kbkdf_reset(ctx);
        OPENSSL_free(ctx);
    }
}

static void kbkdf_reset(void *vctx)
{
    KBKDF *ctx = (KBKDF *)vctx;

    EVP_MAC_CTX_free(ctx->ctx_init);
    OPENSSL_clear_free(ctx->context, ctx->context_len);
    OPENSSL_clear_free(ctx->label, ctx->label_len);
    OPENSSL_clear_free(ctx->ki, ctx->ki_len);
    OPENSSL_clear_free(ctx->iv, ctx->iv_len);
    memset(ctx, 0, sizeof(*ctx));
}

/* SP800-108 section 5.1 or section 5.2 depending on mode. */
static int derive(EVP_MAC_CTX *ctx_init, kbkdf_mode mode, unsigned char *iv,
                  size_t iv_len, unsigned char *label, size_t label_len,
                  unsigned char *context, size_t context_len,
                  unsigned char *k_i, size_t h, uint32_t l, unsigned char *ko,
                  size_t ko_len)
{
    int ret = 0;
    EVP_MAC_CTX *ctx = NULL;
    size_t written = 0, to_write, k_i_len = iv_len;
    const unsigned char zero = 0;
    uint32_t counter, i;

    /* Setup K(0) for feedback mode. */
    if (iv_len > 0)
        memcpy(k_i, iv, iv_len);

    for (counter = 1; written < ko_len; counter++) {
        i = be32(counter);

        ctx = EVP_MAC_CTX_dup(ctx_init);
        if (ctx == NULL)
            goto done;

        /* Perform feedback, if appropriate. */
        if (mode == FEEDBACK && !EVP_MAC_update(ctx, k_i, k_i_len))
            goto done;

        if (!EVP_MAC_update(ctx, (unsigned char *)&i, 4)
            || !EVP_MAC_update(ctx, label, label_len)
            || !EVP_MAC_update(ctx, &zero, 1)
            || !EVP_MAC_update(ctx, context, context_len)
            || !EVP_MAC_update(ctx, (unsigned char *)&l, 4)
            || !EVP_MAC_final(ctx, k_i, NULL, h))
            goto done;

        to_write = ko_len - written;
        memcpy(ko + written, k_i, MIN(to_write, h));
        written += h;

        k_i_len = h;
        EVP_MAC_CTX_free(ctx);
        ctx = NULL;
    }

    ret = 1;
done:
    EVP_MAC_CTX_free(ctx);
    return ret;
}

static int kbkdf_derive(void *vctx, unsigned char *key, size_t keylen)
{
    KBKDF *ctx = (KBKDF *)vctx;
    int ret = 0;
    unsigned char *k_i = NULL;
    uint32_t l = be32(keylen * 8);
    size_t h = 0;

    /* label, context, and iv are permitted to be empty.  Check everything
     * else. */
    if (ctx->ctx_init == NULL) {
        if (ctx->ki_len == 0 || ctx->ki == NULL) {
            ERR_raise(ERR_LIB_PROV, PROV_R_NO_KEY_SET);
            return 0;
        }
        /* Could either be missing MAC or missing message digest or missing
         * cipher - arbitrarily, I pick this one. */
        ERR_raise(ERR_LIB_PROV, PROV_R_MISSING_MAC);
        return 0;
    }

    h = EVP_MAC_size(ctx->ctx_init);
    if (h == 0)
        goto done;
    if (ctx->iv_len != 0 && ctx->iv_len != h) {
        ERR_raise(ERR_LIB_PROV, PROV_R_INVALID_SEED_LENGTH);
        goto done;
    }

    k_i = OPENSSL_zalloc(h);
    if (k_i == NULL)
        goto done;

    ret = derive(ctx->ctx_init, ctx->mode, ctx->iv, ctx->iv_len, ctx->label,
                 ctx->label_len, ctx->context, ctx->context_len, k_i, h, l,
                 key, keylen);
done:
    if (ret != 1)
        OPENSSL_cleanse(key, keylen);
    OPENSSL_clear_free(k_i, h);
    return ret;
}

static int kbkdf_set_buffer(unsigned char **out, size_t *out_len,
                            const OSSL_PARAM *p)
{
    if (p->data == NULL || p->data_size == 0)
        return 1;

    OPENSSL_clear_free(*out, *out_len);
    *out = NULL;
    return OSSL_PARAM_get_octet_string(p, (void **)out, 0, out_len);
}

static int kbkdf_set_ctx_params(void *vctx, const OSSL_PARAM params[])
{
    KBKDF *ctx = (KBKDF *)vctx;
    OPENSSL_CTX *libctx = PROV_LIBRARY_CONTEXT_OF(ctx->provctx);
    const OSSL_PARAM *p;
    OSSL_PARAM mparams[2];

    if (!ossl_prov_macctx_load_from_params(&ctx->ctx_init, params, NULL,
                                           NULL, NULL, libctx))
        return 0;
    else if (ctx->ctx_init != NULL
             && !EVP_MAC_is_a(EVP_MAC_CTX_mac(ctx->ctx_init),
                              OSSL_MAC_NAME_HMAC)
             && !EVP_MAC_is_a(EVP_MAC_CTX_mac(ctx->ctx_init),
                              OSSL_MAC_NAME_CMAC)) {
        ERR_raise(ERR_LIB_PROV, PROV_R_INVALID_MAC);
        return 0;
    }

    p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_MODE);
    if (p != NULL && strncasecmp("counter", p->data, p->data_size) == 0) {
        ctx->mode = COUNTER;
    } else if (p != NULL
               && strncasecmp("feedback", p->data, p->data_size) == 0) {
        ctx->mode = FEEDBACK;
    } else if (p != NULL) {
        ERR_raise(ERR_LIB_PROV, PROV_R_INVALID_MODE);
        return 0;
    }

    p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_KEY);
    if (p != NULL && !kbkdf_set_buffer(&ctx->ki, &ctx->ki_len, p))
        return 0;

    p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_SALT);
    if (p != NULL && !kbkdf_set_buffer(&ctx->label, &ctx->label_len, p))
        return 0;

    p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_INFO);
    if (p != NULL && !kbkdf_set_buffer(&ctx->context, &ctx->context_len, p))
        return 0;

    p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_SEED);
    if (p != NULL && !kbkdf_set_buffer(&ctx->iv, &ctx->iv_len, p))
        return 0;

    /* Set up digest context, if we can. */
    if (ctx->ctx_init != NULL && ctx->ki_len != 0) {
        mparams[0] = OSSL_PARAM_construct_octet_string(OSSL_MAC_PARAM_KEY,
                                                       ctx->ki, ctx->ki_len);
        mparams[1] = OSSL_PARAM_construct_end();

        if (!EVP_MAC_CTX_set_params(ctx->ctx_init, mparams)
            || !EVP_MAC_init(ctx->ctx_init))
            return 0;
    }

    return 1;
}

static const OSSL_PARAM *kbkdf_settable_ctx_params(void)
{
    static const OSSL_PARAM known_settable_ctx_params[] = {
        OSSL_PARAM_octet_string(OSSL_KDF_PARAM_INFO, NULL, 0),
        OSSL_PARAM_octet_string(OSSL_KDF_PARAM_SALT, NULL, 0),
        OSSL_PARAM_octet_string(OSSL_KDF_PARAM_KEY, NULL, 0),
        OSSL_PARAM_octet_string(OSSL_KDF_PARAM_SEED, NULL, 0),
        OSSL_PARAM_utf8_string(OSSL_KDF_PARAM_DIGEST, NULL, 0),
        OSSL_PARAM_utf8_string(OSSL_KDF_PARAM_CIPHER, NULL, 0),
        OSSL_PARAM_utf8_string(OSSL_KDF_PARAM_MAC, NULL, 0),
        OSSL_PARAM_utf8_string(OSSL_KDF_PARAM_MODE, NULL, 0),

        OSSL_PARAM_utf8_string(OSSL_KDF_PARAM_PROPERTIES, NULL, 0),
        OSSL_PARAM_END,
    };
    return known_settable_ctx_params;
}

static int kbkdf_get_ctx_params(void *vctx, OSSL_PARAM params[])
{
    OSSL_PARAM *p;

    p = OSSL_PARAM_locate(params, OSSL_KDF_PARAM_SIZE);
    if (p == NULL)
        return -2;

    /* KBKDF can produce results as large as you like. */
    return OSSL_PARAM_set_size_t(p, SIZE_MAX);
}

static const OSSL_PARAM *kbkdf_gettable_ctx_params(void)
{
    static const OSSL_PARAM known_gettable_ctx_params[] =
        { OSSL_PARAM_size_t(OSSL_KDF_PARAM_SIZE, NULL), OSSL_PARAM_END };
    return known_gettable_ctx_params;
}

const OSSL_DISPATCH kdf_kbkdf_functions[] = {
    { OSSL_FUNC_KDF_NEWCTX, (void(*)(void))kbkdf_new },
    { OSSL_FUNC_KDF_FREECTX, (void(*)(void))kbkdf_free },
    { OSSL_FUNC_KDF_RESET, (void(*)(void))kbkdf_reset },
    { OSSL_FUNC_KDF_DERIVE, (void(*)(void))kbkdf_derive },
    { OSSL_FUNC_KDF_SETTABLE_CTX_PARAMS,
      (void(*)(void))kbkdf_settable_ctx_params },
    { OSSL_FUNC_KDF_SET_CTX_PARAMS, (void(*)(void))kbkdf_set_ctx_params },
    { OSSL_FUNC_KDF_GETTABLE_CTX_PARAMS,
      (void(*)(void))kbkdf_gettable_ctx_params },
    { OSSL_FUNC_KDF_GET_CTX_PARAMS, (void(*)(void))kbkdf_get_ctx_params },
    { 0, NULL },
};
