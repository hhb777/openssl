/*
 * Copyright 2015-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <string.h>

#include <openssl/evp.h>
#include "internal/evp_int.h"
#include "internal/provider.h"
#include "evp_locl.h"

EVP_CIPHER *EVP_CIPHER_meth_new(int cipher_type, int block_size, int key_len)
{
    EVP_CIPHER *cipher = OPENSSL_zalloc(sizeof(EVP_CIPHER));

    if (cipher != NULL) {
        cipher->nid = cipher_type;
        cipher->block_size = block_size;
        cipher->key_len = key_len;
        cipher->lock = CRYPTO_THREAD_lock_new();
        if (cipher->lock == NULL) {
            OPENSSL_free(cipher);
            return NULL;
        }
        cipher->refcnt = 1;
    }
    return cipher;
}

EVP_CIPHER *EVP_CIPHER_meth_dup(const EVP_CIPHER *cipher)
{
    EVP_CIPHER *to = EVP_CIPHER_meth_new(cipher->nid, cipher->block_size,
                                         cipher->key_len);

    if (to != NULL) {
        CRYPTO_RWLOCK *lock = to->lock;

        memcpy(to, cipher, sizeof(*to));
        to->lock = lock;
    }
    return to;
}

void EVP_CIPHER_meth_free(EVP_CIPHER *cipher)
{
    if (cipher != NULL) {
        int i;

        CRYPTO_DOWN_REF(&cipher->refcnt, &i, cipher->lock);
        if (i > 0)
            return;
        ossl_provider_free(cipher->prov);
        OPENSSL_free(cipher->name);
        CRYPTO_THREAD_lock_free(cipher->lock);
        OPENSSL_free(cipher);
    }
}

int EVP_CIPHER_up_ref(EVP_CIPHER *cipher)
{
    int ref = 0;

    CRYPTO_UP_REF(&cipher->refcnt, &ref, cipher->lock);
    return 1;
}

int EVP_CIPHER_meth_set_iv_length(EVP_CIPHER *cipher, int iv_len)
{
    cipher->iv_len = iv_len;
    return 1;
}

int EVP_CIPHER_meth_set_flags(EVP_CIPHER *cipher, unsigned long flags)
{
    cipher->flags = flags;
    return 1;
}

int EVP_CIPHER_meth_set_impl_ctx_size(EVP_CIPHER *cipher, int ctx_size)
{
    cipher->ctx_size = ctx_size;
    return 1;
}

int EVP_CIPHER_meth_set_init(EVP_CIPHER *cipher,
                             int (*init) (EVP_CIPHER_CTX *ctx,
                                          const unsigned char *key,
                                          const unsigned char *iv,
                                          int enc))
{
    cipher->init = init;
    return 1;
}

int EVP_CIPHER_meth_set_do_cipher(EVP_CIPHER *cipher,
                                  int (*do_cipher) (EVP_CIPHER_CTX *ctx,
                                                    unsigned char *out,
                                                    const unsigned char *in,
                                                    size_t inl))
{
    cipher->do_cipher = do_cipher;
    return 1;
}

int EVP_CIPHER_meth_set_cleanup(EVP_CIPHER *cipher,
                                int (*cleanup) (EVP_CIPHER_CTX *))
{
    cipher->cleanup = cleanup;
    return 1;
}

int EVP_CIPHER_meth_set_set_asn1_params(EVP_CIPHER *cipher,
                                        int (*set_asn1_parameters) (EVP_CIPHER_CTX *,
                                                                    ASN1_TYPE *))
{
    cipher->set_asn1_parameters = set_asn1_parameters;
    return 1;
}

int EVP_CIPHER_meth_set_get_asn1_params(EVP_CIPHER *cipher,
                                        int (*get_asn1_parameters) (EVP_CIPHER_CTX *,
                                                                    ASN1_TYPE *))
{
    cipher->get_asn1_parameters = get_asn1_parameters;
    return 1;
}

int EVP_CIPHER_meth_set_ctrl(EVP_CIPHER *cipher,
                             int (*ctrl) (EVP_CIPHER_CTX *, int type,
                                          int arg, void *ptr))
{
    cipher->ctrl = ctrl;
    return 1;
}


int (*EVP_CIPHER_meth_get_init(const EVP_CIPHER *cipher))(EVP_CIPHER_CTX *ctx,
                                                          const unsigned char *key,
                                                          const unsigned char *iv,
                                                          int enc)
{
    return cipher->init;
}
int (*EVP_CIPHER_meth_get_do_cipher(const EVP_CIPHER *cipher))(EVP_CIPHER_CTX *ctx,
                                                               unsigned char *out,
                                                               const unsigned char *in,
                                                               size_t inl)
{
    return cipher->do_cipher;
}

int (*EVP_CIPHER_meth_get_cleanup(const EVP_CIPHER *cipher))(EVP_CIPHER_CTX *)
{
    return cipher->cleanup;
}

int (*EVP_CIPHER_meth_get_set_asn1_params(const EVP_CIPHER *cipher))(EVP_CIPHER_CTX *,
                                                                     ASN1_TYPE *)
{
    return cipher->set_asn1_parameters;
}

int (*EVP_CIPHER_meth_get_get_asn1_params(const EVP_CIPHER *cipher))(EVP_CIPHER_CTX *,
                                                               ASN1_TYPE *)
{
    return cipher->get_asn1_parameters;
}

int (*EVP_CIPHER_meth_get_ctrl(const EVP_CIPHER *cipher))(EVP_CIPHER_CTX *,
                                                          int type, int arg,
                                                          void *ptr)
{
    return cipher->ctrl;
}

