/*
 * Copyright 2018-2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>
#include "internal/cryptlib.h"
#include "internal/numbers.h"
#include "internal/evp_int.h"
#include "internal/provider_ctx.h"
#include "internal/providercommonerr.h"
#include "internal/provider_algs.h"

/* See RFC 4253, Section 7.2 */
static OSSL_OP_kdf_newctx_fn kdf_sshkdf_new;
static OSSL_OP_kdf_freectx_fn kdf_sshkdf_free;
static OSSL_OP_kdf_reset_fn kdf_sshkdf_reset;
static OSSL_OP_kdf_derive_fn kdf_sshkdf_derive;
static OSSL_OP_kdf_settable_ctx_params_fn kdf_sshkdf_settable_ctx_params;
static OSSL_OP_kdf_set_ctx_params_fn kdf_sshkdf_set_ctx_params;
static OSSL_OP_kdf_gettable_ctx_params_fn kdf_sshkdf_gettable_ctx_params;
static OSSL_OP_kdf_get_ctx_params_fn kdf_sshkdf_get_ctx_params;

static int SSHKDF(const EVP_MD *evp_md,
                  const unsigned char *key, size_t key_len,
                  const unsigned char *xcghash, size_t xcghash_len,
                  const unsigned char *session_id, size_t session_id_len,
                  char type, unsigned char *okey, size_t okey_len);

typedef struct {
    void *provctx;
    EVP_MD *md;
    unsigned char *key; /* K */
    size_t key_len;
    unsigned char *xcghash; /* H */
    size_t xcghash_len;
    char type; /* X */
    unsigned char *session_id;
    size_t session_id_len;
} KDF_SSHKDF;

static void *kdf_sshkdf_new(void *provctx)
{
    KDF_SSHKDF *ctx;

    if ((ctx = OPENSSL_zalloc(sizeof(*ctx))) == NULL)
        ERR_raise(ERR_LIB_PROV, ERR_R_MALLOC_FAILURE);
    ctx->provctx = provctx;
    return ctx;
}

static void kdf_sshkdf_free(void *vctx)
{
    KDF_SSHKDF *ctx = (KDF_SSHKDF *)vctx;

    kdf_sshkdf_reset(ctx);
    EVP_MD_meth_free(ctx->md);
    OPENSSL_free(ctx);
}

static void kdf_sshkdf_reset(void *vctx)
{
    KDF_SSHKDF *ctx = (KDF_SSHKDF *)vctx;

    OPENSSL_clear_free(ctx->key, ctx->key_len);
    OPENSSL_clear_free(ctx->xcghash, ctx->xcghash_len);
    OPENSSL_clear_free(ctx->session_id, ctx->session_id_len);
    memset(ctx, 0, sizeof(*ctx));
}

static int sshkdf_set_membuf(unsigned char **dst, size_t *dst_len,
                             const OSSL_PARAM *p)
{
    OPENSSL_clear_free(*dst, *dst_len);
    *dst = NULL;
    return OSSL_PARAM_get_octet_string(p, (void **)dst, 0, dst_len);
}

static int kdf_sshkdf_derive(void *vctx, unsigned char *key,
                             size_t keylen)
{
    KDF_SSHKDF *ctx = (KDF_SSHKDF *)vctx;

    if (ctx->md == NULL) {
        ERR_raise(ERR_LIB_PROV, PROV_R_MISSING_MESSAGE_DIGEST);
        return 0;
    }
    if (ctx->key == NULL) {
        ERR_raise(ERR_LIB_PROV, PROV_R_MISSING_KEY);
        return 0;
    }
    if (ctx->xcghash == NULL) {
        ERR_raise(ERR_LIB_PROV, PROV_R_MISSING_XCGHASH);
        return 0;
    }
    if (ctx->session_id == NULL) {
        ERR_raise(ERR_LIB_PROV, PROV_R_MISSING_SESSION_ID);
        return 0;
    }
    if (ctx->type == 0) {
        ERR_raise(ERR_LIB_PROV, PROV_R_MISSING_TYPE);
        return 0;
    }
    return SSHKDF(ctx->md, ctx->key, ctx->key_len,
                  ctx->xcghash, ctx->xcghash_len,
                  ctx->session_id, ctx->session_id_len,
                  ctx->type, key, keylen);
}

static int kdf_sshkdf_set_ctx_params(void *vctx, const OSSL_PARAM params[])
{
    const OSSL_PARAM *p;
    KDF_SSHKDF *ctx = vctx;
    EVP_MD *md;
    int t;
    const char *properties = NULL;

    /* Grab search properties, this should be before the digest lookup */
    if ((p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_PROPERTIES))
        != NULL) {
        if (p->data_type != OSSL_PARAM_UTF8_STRING)
            return 0;
        properties = p->data;
    }
    /* Handle aliasing of digest parameter names */
    if ((p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_DIGEST)) != NULL) {
        if (p->data_type != OSSL_PARAM_UTF8_STRING)
            return 0;
        md = EVP_MD_fetch(PROV_LIBRARY_CONTEXT_OF(ctx->provctx), p->data,
                          properties);
        if (md == NULL) {
            ERR_raise(ERR_LIB_PROV, PROV_R_INVALID_DIGEST);
            return 0;
        }
        EVP_MD_meth_free(ctx->md);
        ctx->md = md;
    }

    if ((p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_KEY)) != NULL)
        if (!sshkdf_set_membuf(&ctx->key, &ctx->key_len, p))
            return 0;

    if ((p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_SSHKDF_XCGHASH))
        != NULL)
        if (!sshkdf_set_membuf(&ctx->xcghash, &ctx->xcghash_len, p))
            return 0;

    if ((p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_SSHKDF_SESSION_ID))
        != NULL)
        if (!sshkdf_set_membuf(&ctx->session_id, &ctx->session_id_len, p))
            return 0;

    if ((p = OSSL_PARAM_locate_const(params, OSSL_KDF_PARAM_SSHKDF_TYPE))
        != NULL) {
        if (p->data == NULL || p->data_size == 0)
            return 0;
        t = *(unsigned char *)p->data;
        if (t < 65 || t > 70) {
            ERR_raise(ERR_LIB_PROV, PROV_R_VALUE_ERROR);
            return 0;
        }
        ctx->type = (char)t;
    }
    return 1;
}

static const OSSL_PARAM *kdf_sshkdf_settable_ctx_params(void)
{
    static const OSSL_PARAM known_settable_ctx_params[] = {
        OSSL_PARAM_utf8_string(OSSL_KDF_PARAM_PROPERTIES, NULL, 0),
        OSSL_PARAM_utf8_string(OSSL_KDF_PARAM_DIGEST, NULL, 0),
        OSSL_PARAM_octet_string(OSSL_KDF_PARAM_KEY, NULL, 0),
        OSSL_PARAM_octet_string(OSSL_KDF_PARAM_SSHKDF_XCGHASH, NULL, 0),
        OSSL_PARAM_octet_string(OSSL_KDF_PARAM_SSHKDF_SESSION_ID, NULL, 0),
        OSSL_PARAM_utf8_string(OSSL_KDF_PARAM_SSHKDF_TYPE, NULL, 0),
        OSSL_PARAM_END
    };
    return known_settable_ctx_params;
}

static int kdf_sshkdf_get_ctx_params(void *vctx, OSSL_PARAM params[])
{
    OSSL_PARAM *p;

    if ((p = OSSL_PARAM_locate(params, OSSL_KDF_PARAM_SIZE)) != NULL)
        return OSSL_PARAM_set_size_t(p, SIZE_MAX);
    return -2;
}

static const OSSL_PARAM *kdf_sshkdf_gettable_ctx_params(void)
{
    static const OSSL_PARAM known_gettable_ctx_params[] = {
        OSSL_PARAM_size_t(OSSL_KDF_PARAM_SIZE, NULL),
        OSSL_PARAM_END
    };
    return known_gettable_ctx_params;
}

const OSSL_DISPATCH kdf_sshkdf_functions[] = {
    { OSSL_FUNC_KDF_NEWCTX, (void(*)(void))kdf_sshkdf_new },
    { OSSL_FUNC_KDF_FREECTX, (void(*)(void))kdf_sshkdf_free },
    { OSSL_FUNC_KDF_RESET, (void(*)(void))kdf_sshkdf_reset },
    { OSSL_FUNC_KDF_DERIVE, (void(*)(void))kdf_sshkdf_derive },
    { OSSL_FUNC_KDF_SETTABLE_CTX_PARAMS,
      (void(*)(void))kdf_sshkdf_settable_ctx_params },
    { OSSL_FUNC_KDF_SET_CTX_PARAMS, (void(*)(void))kdf_sshkdf_set_ctx_params },
    { OSSL_FUNC_KDF_GETTABLE_CTX_PARAMS,
      (void(*)(void))kdf_sshkdf_gettable_ctx_params },
    { OSSL_FUNC_KDF_GET_CTX_PARAMS, (void(*)(void))kdf_sshkdf_get_ctx_params },
    { 0, NULL }
};

static int SSHKDF(const EVP_MD *evp_md,
                  const unsigned char *key, size_t key_len,
                  const unsigned char *xcghash, size_t xcghash_len,
                  const unsigned char *session_id, size_t session_id_len,
                  char type, unsigned char *okey, size_t okey_len)
{
    EVP_MD_CTX *md = NULL;
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int dsize = 0;
    size_t cursize = 0;
    int ret = 0;

    md = EVP_MD_CTX_new();
    if (md == NULL)
        return 0;

    if (!EVP_DigestInit_ex(md, evp_md, NULL))
        goto out;

    if (!EVP_DigestUpdate(md, key, key_len))
        goto out;

    if (!EVP_DigestUpdate(md, xcghash, xcghash_len))
        goto out;

    if (!EVP_DigestUpdate(md, &type, 1))
        goto out;

    if (!EVP_DigestUpdate(md, session_id, session_id_len))
        goto out;

    if (!EVP_DigestFinal_ex(md, digest, &dsize))
        goto out;

    if (okey_len < dsize) {
        memcpy(okey, digest, okey_len);
        ret = 1;
        goto out;
    }

    memcpy(okey, digest, dsize);

    for (cursize = dsize; cursize < okey_len; cursize += dsize) {

        if (!EVP_DigestInit_ex(md, evp_md, NULL))
            goto out;

        if (!EVP_DigestUpdate(md, key, key_len))
            goto out;

        if (!EVP_DigestUpdate(md, xcghash, xcghash_len))
            goto out;

        if (!EVP_DigestUpdate(md, okey, cursize))
            goto out;

        if (!EVP_DigestFinal_ex(md, digest, &dsize))
            goto out;

        if (okey_len < cursize + dsize) {
            memcpy(okey + cursize, digest, okey_len - cursize);
            ret = 1;
            goto out;
        }

        memcpy(okey + cursize, digest, dsize);
    }

    ret = 1;

out:
    EVP_MD_CTX_free(md);
    OPENSSL_cleanse(digest, EVP_MAX_MD_SIZE);
    return ret;
}

