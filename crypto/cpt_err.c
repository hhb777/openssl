/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <openssl/err.h>
#include <openssl/cryptoerr.h>

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA CRYPTO_str_functs[] = {
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_CRYPTO_DUP_EX_DATA, 0),
     "CRYPTO_dup_ex_data"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_CRYPTO_FREE_EX_DATA, 0),
     "CRYPTO_free_ex_data"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_CRYPTO_GET_EX_NEW_INDEX, 0),
     "CRYPTO_get_ex_new_index"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_CRYPTO_MEMDUP, 0), "CRYPTO_memdup"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_CRYPTO_NEW_EX_DATA, 0),
     "CRYPTO_new_ex_data"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_CRYPTO_SET_EX_DATA, 0),
     "CRYPTO_set_ex_data"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_FIPS_MODE_SET, 0), "FIPS_mode_set"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_GET_AND_LOCK, 0), "get_and_lock"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_OPENSSL_ATEXIT, 0), "OPENSSL_atexit"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_OPENSSL_BUF2HEXSTR, 0),
     "OPENSSL_buf2hexstr"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_OPENSSL_FOPEN, 0), "openssl_fopen"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_OPENSSL_HEXSTR2BUF, 0),
     "OPENSSL_hexstr2buf"},
    {ERR_PACK(ERR_LIB_CRYPTO, CRYPTO_F_OPENSSL_INIT_CRYPTO, 0),
     "OPENSSL_init_crypto"},
    {0, NULL}
};

static const ERR_STRING_DATA CRYPTO_str_reasons[] = {
    {ERR_PACK(ERR_LIB_CRYPTO, 0, CRYPTO_R_FIPS_MODE_NOT_SUPPORTED),
    "fips mode not supported"},
    {ERR_PACK(ERR_LIB_CRYPTO, 0, CRYPTO_R_ILLEGAL_HEX_DIGIT),
    "illegal hex digit"},
    {ERR_PACK(ERR_LIB_CRYPTO, 0, CRYPTO_R_ODD_NUMBER_OF_DIGITS),
    "odd number of digits"},
    {0, NULL}
};

#endif

int ERR_load_CRYPTO_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_func_error_string(CRYPTO_str_functs[0].error) == NULL) {
        ERR_load_strings_const(CRYPTO_str_functs);
        ERR_load_strings_const(CRYPTO_str_reasons);
    }
#endif
    return 1;
}
