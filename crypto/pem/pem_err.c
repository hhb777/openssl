/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2017 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <openssl/err.h>
#include <openssl/pemerr.h>

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA PEM_str_functs[] = {
    {ERR_PACK(ERR_LIB_PEM, PEM_F_B2I_DSS, 0), "b2i_dss"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_B2I_PVK_BIO, 0), "b2i_PVK_bio"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_B2I_RSA, 0), "b2i_rsa"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_CHECK_BITLEN_DSA, 0), "check_bitlen_dsa"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_CHECK_BITLEN_RSA, 0), "check_bitlen_rsa"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_D2I_PKCS8PRIVATEKEY_BIO, 0),
     "d2i_PKCS8PrivateKey_bio"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_D2I_PKCS8PRIVATEKEY_FP, 0),
     "d2i_PKCS8PrivateKey_fp"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_DO_B2I, 0), "do_b2i"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_DO_B2I_BIO, 0), "do_b2i_bio"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_DO_BLOB_HEADER, 0), "do_blob_header"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_DO_PK8PKEY, 0), "do_pk8pkey"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_DO_PK8PKEY_FP, 0), "do_pk8pkey_fp"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_DO_PVK_BODY, 0), "do_PVK_body"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_DO_PVK_HEADER, 0), "do_PVK_header"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_GET_HEADER_AND_DATA, 0),
     "get_header_and_data"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_GET_NAME, 0), "get_name"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_I2B_PVK, 0), "i2b_PVK"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_I2B_PVK_BIO, 0), "i2b_PVK_bio"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_LOAD_IV, 0), "load_iv"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_ASN1_READ, 0), "PEM_ASN1_read"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_ASN1_READ_BIO, 0),
     "PEM_ASN1_read_bio"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_ASN1_WRITE, 0), "PEM_ASN1_write"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_ASN1_WRITE_BIO, 0),
     "PEM_ASN1_write_bio"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_DEF_CALLBACK, 0), "PEM_def_callback"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_DO_HEADER, 0), "PEM_do_header"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_GET_EVP_CIPHER_INFO, 0),
     "PEM_get_EVP_CIPHER_INFO"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_READ, 0), "PEM_read"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_READ_BIO, 0), "PEM_read_bio"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_READ_BIO_DHPARAMS, 0),
     "PEM_read_bio_DHparams"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_READ_BIO_EX, 0), "PEM_read_bio_ex"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_READ_BIO_PARAMETERS, 0),
     "PEM_read_bio_Parameters"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_READ_BIO_PRIVATEKEY, 0),
     "PEM_read_bio_PrivateKey"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_READ_DHPARAMS, 0),
     "PEM_read_DHparams"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_READ_PRIVATEKEY, 0),
     "PEM_read_PrivateKey"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_SIGNFINAL, 0), "PEM_SignFinal"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_WRITE, 0), "PEM_write"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_WRITE_BIO, 0), "PEM_write_bio"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_WRITE_PRIVATEKEY, 0),
     "PEM_write_PrivateKey"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_X509_INFO_READ, 0),
     "PEM_X509_INFO_read"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_X509_INFO_READ_BIO, 0),
     "PEM_X509_INFO_read_bio"},
    {ERR_PACK(ERR_LIB_PEM, PEM_F_PEM_X509_INFO_WRITE_BIO, 0),
     "PEM_X509_INFO_write_bio"},
    {0, NULL}
};

static const ERR_STRING_DATA PEM_str_reasons[] = {
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BAD_BASE64_DECODE),
    "bad base64 decode"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BAD_DECRYPT),
    "bad decrypt"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BAD_END_LINE),
    "bad end line"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BAD_IV_CHARS),
    "bad iv chars"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BAD_MAGIC_NUMBER),
    "bad magic number"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BAD_PASSWORD_READ),
    "bad password read"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BAD_VERSION_NUMBER),
    "bad version number"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_BIO_WRITE_FAILURE),
    "bio write failure"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_CIPHER_IS_NULL),
    "cipher is null"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_ERROR_CONVERTING_PRIVATE_KEY),
    "error converting private key"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_EXPECTING_PRIVATE_KEY_BLOB),
    "expecting private key blob"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_EXPECTING_PUBLIC_KEY_BLOB),
    "expecting public key blob"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_HEADER_TOO_LONG),
    "header too long"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_INCONSISTENT_HEADER),
    "inconsistent header"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_KEYBLOB_HEADER_PARSE_ERROR),
    "keyblob header parse error"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_KEYBLOB_TOO_SHORT),
    "keyblob too short"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_MISSING_DEK_IV),
    "missing dek iv"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_NOT_DEK_INFO),
    "not dek info"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_NOT_ENCRYPTED),
    "not encrypted"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_NOT_PROC_TYPE),
    "not proc type"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_NO_START_LINE),
    "no start line"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_PROBLEMS_GETTING_PASSWORD),
    "problems getting password"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_PVK_DATA_TOO_SHORT),
    "pvk data too short"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_PVK_TOO_SHORT),
    "pvk too short"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_READ_KEY),
    "read key"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_SHORT_HEADER),
    "short header"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_UNEXPECTED_DEK_IV),
    "unexpected dek iv"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_UNSUPPORTED_CIPHER),
    "unsupported cipher"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_UNSUPPORTED_ENCRYPTION),
    "unsupported encryption"},
    {ERR_PACK(ERR_LIB_PEM, 0, PEM_R_UNSUPPORTED_KEY_COMPONENTS),
    "unsupported key components"},
    {0, NULL}
};

#endif

int ERR_load_PEM_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_func_error_string(PEM_str_functs[0].error) == NULL) {
        ERR_load_strings_const(PEM_str_functs);
        ERR_load_strings_const(PEM_str_reasons);
    }
#endif
    return 1;
}
