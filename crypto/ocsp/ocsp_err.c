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
#include <openssl/ocsperr.h>

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA OCSP_str_functs[] = {
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_D2I_OCSP_NONCE, 0),
     "d2i_ocsp_nonce"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_BASIC_ADD1_STATUS, 0),
     "OCSP_basic_add1_status"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_BASIC_SIGN, 0),
     "OCSP_basic_sign"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_BASIC_VERIFY, 0),
     "OCSP_basic_verify"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_CERT_ID_NEW, 0),
     "OCSP_cert_id_new"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_CHECK_DELEGATED, 0),
     "ocsp_check_delegated"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_CHECK_IDS, 0),
     "ocsp_check_ids"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_CHECK_ISSUER, 0),
     "ocsp_check_issuer"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_CHECK_VALIDITY, 0),
     "OCSP_check_validity"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_MATCH_ISSUERID, 0),
     "ocsp_match_issuerid"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_PARSE_URL, 0),
     "OCSP_parse_url"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_REQUEST_SIGN, 0),
     "OCSP_request_sign"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_REQUEST_VERIFY, 0),
     "OCSP_request_verify"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_OCSP_RESPONSE_GET1_BASIC, 0),
     "OCSP_response_get1_basic"},
    {ERR_PACK(ERR_LIB_OCSP, OCSP_F_PARSE_HTTP_LINE1, 0),
     "parse_http_line1"},
    {0, NULL}
};

static const ERR_STRING_DATA OCSP_str_reasons[] = {
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_CERTIFICATE_VERIFY_ERROR),
    "certificate verify error"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_DIGEST_ERR),
    "digest err"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_ERROR_IN_NEXTUPDATE_FIELD),
    "error in nextupdate field"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_ERROR_IN_THISUPDATE_FIELD),
    "error in thisupdate field"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_ERROR_PARSING_URL),
    "error parsing url"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_MISSING_OCSPSIGNING_USAGE),
    "missing ocspsigning usage"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_NEXTUPDATE_BEFORE_THISUPDATE),
    "nextupdate before thisupdate"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_NOT_BASIC_RESPONSE),
    "not basic response"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_NO_CERTIFICATES_IN_CHAIN),
    "no certificates in chain"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_NO_RESPONSE_DATA),
    "no response data"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_NO_REVOKED_TIME),
    "no revoked time"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_NO_SIGNER_KEY),
    "no signer key"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_PRIVATE_KEY_DOES_NOT_MATCH_CERTIFICATE),
    "private key does not match certificate"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_REQUEST_NOT_SIGNED),
    "request not signed"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_RESPONSE_CONTAINS_NO_REVOCATION_DATA),
    "response contains no revocation data"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_ROOT_CA_NOT_TRUSTED),
    "root ca not trusted"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_SERVER_RESPONSE_ERROR),
    "server response error"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_SERVER_RESPONSE_PARSE_ERROR),
    "server response parse error"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_SIGNATURE_FAILURE),
    "signature failure"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_SIGNER_CERTIFICATE_NOT_FOUND),
    "signer certificate not found"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_STATUS_EXPIRED),
    "status expired"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_STATUS_NOT_YET_VALID),
    "status not yet valid"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_STATUS_TOO_OLD),
    "status too old"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_UNKNOWN_MESSAGE_DIGEST),
    "unknown message digest"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_UNKNOWN_NID),
    "unknown nid"},
    {ERR_PACK(ERR_LIB_OCSP, 0, OCSP_R_UNSUPPORTED_REQUESTORNAME_TYPE),
    "unsupported requestorname type"},
    {0, NULL}
};

#endif

int ERR_load_OCSP_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_func_error_string(OCSP_str_functs[0].error) == NULL) {
        ERR_load_strings_const(OCSP_str_functs);
        ERR_load_strings_const(OCSP_str_reasons);
    }
#endif
    return 1;
}
