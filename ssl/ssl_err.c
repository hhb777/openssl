/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <openssl/err.h>
#include <openssl/sslerr.h>
#include "sslerr.h"

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA SSL_str_reasons[] = {
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_APPLICATION_DATA_AFTER_CLOSE_NOTIFY),
    "application data after close notify"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_APP_DATA_IN_HANDSHAKE),
    "app data in handshake"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_ATTEMPT_TO_REUSE_SESSION_IN_DIFFERENT_CONTEXT),
    "attempt to reuse session in different context"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_AT_LEAST_TLS_1_0_NEEDED_IN_FIPS_MODE),
    "at least TLS 1.0 needed in FIPS mode"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_AT_LEAST_TLS_1_2_NEEDED_IN_SUITEB_MODE),
    "at least (D)TLS 1.2 needed in Suite B mode"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_CHANGE_CIPHER_SPEC),
    "bad change cipher spec"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_CIPHER), "bad cipher"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_DATA), "bad data"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_DATA_LENGTH), "bad data length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_DATA_RETURNED_BY_CALLBACK),
    "bad data returned by callback"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_DECOMPRESSION), "bad decompression"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_DH_VALUE), "bad dh value"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_DIGEST_LENGTH), "bad digest length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_EARLY_DATA), "bad early data"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_ECC_CERT), "bad ecc cert"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_ECPOINT), "bad ecpoint"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_EXTENSION), "bad extension"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_HANDSHAKE_LENGTH),
    "bad handshake length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_HANDSHAKE_STATE),
    "bad handshake state"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_HELLO_REQUEST), "bad hello request"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_HRR_VERSION), "bad hrr version"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_KEY_SHARE), "bad key share"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_KEY_UPDATE), "bad key update"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_LEGACY_VERSION), "bad legacy version"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_LENGTH), "bad length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_PACKET), "bad packet"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_PACKET_LENGTH), "bad packet length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_PROTOCOL_VERSION_NUMBER),
    "bad protocol version number"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_PSK), "bad psk"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_PSK_IDENTITY), "bad psk identity"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_RECORD_TYPE), "bad record type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_RSA_ENCRYPT), "bad rsa encrypt"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_SIGNATURE), "bad signature"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_SRP_A_LENGTH), "bad srp a length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_SRP_PARAMETERS), "bad srp parameters"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_SRTP_MKI_VALUE), "bad srtp mki value"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_SRTP_PROTECTION_PROFILE_LIST),
    "bad srtp protection profile list"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_SSL_FILETYPE), "bad ssl filetype"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_VALUE), "bad value"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BAD_WRITE_RETRY), "bad write retry"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BINDER_DOES_NOT_VERIFY),
    "binder does not verify"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BIO_NOT_SET), "bio not set"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BLOCK_CIPHER_PAD_IS_WRONG),
    "block cipher pad is wrong"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_BN_LIB), "bn lib"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CALLBACK_FAILED), "callback failed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CANNOT_CHANGE_CIPHER),
    "cannot change cipher"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CA_DN_LENGTH_MISMATCH),
    "ca dn length mismatch"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CA_KEY_TOO_SMALL), "ca key too small"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CA_MD_TOO_WEAK), "ca md too weak"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CCS_RECEIVED_EARLY), "ccs received early"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CERTIFICATE_VERIFY_FAILED),
    "certificate verify failed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CERT_CB_ERROR), "cert cb error"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CERT_LENGTH_MISMATCH),
    "cert length mismatch"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CIPHERSUITE_DIGEST_HAS_CHANGED),
    "ciphersuite digest has changed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CIPHER_CODE_WRONG_LENGTH),
    "cipher code wrong length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CLIENTHELLO_TLSEXT), "clienthello tlsext"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_COMPRESSED_LENGTH_TOO_LONG),
    "compressed length too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_COMPRESSION_DISABLED),
    "compression disabled"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_COMPRESSION_FAILURE),
    "compression failure"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_COMPRESSION_ID_NOT_WITHIN_PRIVATE_RANGE),
    "compression id not within private range"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_COMPRESSION_LIBRARY_ERROR),
    "compression library error"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CONNECTION_TYPE_NOT_SET),
    "connection type not set"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CONTEXT_NOT_DANE_ENABLED),
    "context not dane enabled"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_COOKIE_GEN_CALLBACK_FAILURE),
    "cookie gen callback failure"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_COOKIE_MISMATCH), "cookie mismatch"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_COPY_PARAMETERS_FAILED),
    "copy parameters failed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_CUSTOM_EXT_HANDLER_ALREADY_INSTALLED),
    "custom ext handler already installed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_ALREADY_ENABLED),
    "dane already enabled"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_CANNOT_OVERRIDE_MTYPE_FULL),
    "dane cannot override mtype full"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_NOT_ENABLED), "dane not enabled"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_TLSA_BAD_CERTIFICATE),
    "dane tlsa bad certificate"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_TLSA_BAD_CERTIFICATE_USAGE),
    "dane tlsa bad certificate usage"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_TLSA_BAD_DATA_LENGTH),
    "dane tlsa bad data length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_TLSA_BAD_DIGEST_LENGTH),
    "dane tlsa bad digest length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_TLSA_BAD_MATCHING_TYPE),
    "dane tlsa bad matching type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_TLSA_BAD_PUBLIC_KEY),
    "dane tlsa bad public key"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_TLSA_BAD_SELECTOR),
    "dane tlsa bad selector"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DANE_TLSA_NULL_DATA),
    "dane tlsa null data"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DATA_BETWEEN_CCS_AND_FINISHED),
    "data between ccs and finished"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DATA_LENGTH_TOO_LONG),
    "data length too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DECRYPTION_FAILED), "decryption failed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC),
    "decryption failed or bad record mac"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DH_KEY_TOO_SMALL), "dh key too small"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DH_PUBLIC_VALUE_LENGTH_IS_WRONG),
    "dh public value length is wrong"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DIGEST_CHECK_FAILED),
    "digest check failed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DTLS_MESSAGE_TOO_BIG),
    "dtls message too big"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_DUPLICATE_COMPRESSION_ID),
    "duplicate compression id"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_ECC_CERT_NOT_FOR_SIGNING),
    "ecc cert not for signing"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_ECDH_REQUIRED_FOR_SUITEB_MODE),
    "ecdh required for suiteb mode"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_EE_KEY_TOO_SMALL), "ee key too small"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_EMPTY_SRTP_PROTECTION_PROFILE_LIST),
    "empty srtp protection profile list"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_ENCRYPTED_LENGTH_TOO_LONG),
    "encrypted length too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_ERROR_IN_RECEIVED_CIPHER_LIST),
    "error in received cipher list"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_ERROR_SETTING_TLSA_BASE_DOMAIN),
    "error setting tlsa base domain"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_EXCEEDS_MAX_FRAGMENT_SIZE),
    "exceeds max fragment size"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_EXCESSIVE_MESSAGE_SIZE),
    "excessive message size"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_EXTENSION_NOT_RECEIVED),
    "extension not received"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_EXTRA_DATA_IN_MESSAGE),
    "extra data in message"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_EXT_LENGTH_MISMATCH),
    "ext length mismatch"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_FAILED_TO_INIT_ASYNC),
    "failed to init async"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_FRAGMENTED_CLIENT_HELLO),
    "fragmented client hello"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_GOT_A_FIN_BEFORE_A_CCS),
    "got a fin before a ccs"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_HTTPS_PROXY_REQUEST),
    "https proxy request"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_HTTP_REQUEST), "http request"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_ILLEGAL_POINT_COMPRESSION),
    "illegal point compression"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_ILLEGAL_SUITEB_DIGEST),
    "illegal Suite B digest"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INAPPROPRIATE_FALLBACK),
    "inappropriate fallback"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INCONSISTENT_COMPRESSION),
    "inconsistent compression"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INCONSISTENT_EARLY_DATA_ALPN),
    "inconsistent early data alpn"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INCONSISTENT_EARLY_DATA_SNI),
    "inconsistent early data sni"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INCONSISTENT_EXTMS), "inconsistent extms"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INSUFFICIENT_SECURITY),
    "insufficient security"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INTERNAL_ERROR), "internal error"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_ALERT), "invalid alert"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_CCS_MESSAGE),
    "invalid ccs message"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_CERTIFICATE_OR_ALG),
    "invalid certificate or alg"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_COMMAND), "invalid command"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_COMPRESSION_ALGORITHM),
    "invalid compression algorithm"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_CONFIG), "invalid config"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_CONFIGURATION_NAME),
    "invalid configuration name"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_CONTEXT), "invalid context"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_CT_VALIDATION_TYPE),
    "invalid ct validation type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_KEY_UPDATE_TYPE),
    "invalid key update type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_MAX_EARLY_DATA),
    "invalid max early data"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_NULL_CMD_NAME),
    "invalid null cmd name"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_SEQUENCE_NUMBER),
    "invalid sequence number"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_SERVERINFO_DATA),
    "invalid serverinfo data"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_SESSION_ID), "invalid session id"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_SRP_USERNAME),
    "invalid srp username"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_STATUS_RESPONSE),
    "invalid status response"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_INVALID_TICKET_KEYS_LENGTH),
    "invalid ticket keys length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_LENGTH_MISMATCH), "length mismatch"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_LENGTH_TOO_LONG), "length too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_LENGTH_TOO_SHORT), "length too short"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_LIBRARY_BUG), "library bug"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_LIBRARY_HAS_NO_CIPHERS),
    "library has no ciphers"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_DSA_SIGNING_CERT),
    "missing dsa signing cert"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_ECDSA_SIGNING_CERT),
    "missing ecdsa signing cert"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_FATAL), "missing fatal"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_PARAMETERS), "missing parameters"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_RSA_CERTIFICATE),
    "missing rsa certificate"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_RSA_ENCRYPTING_CERT),
    "missing rsa encrypting cert"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_RSA_SIGNING_CERT),
    "missing rsa signing cert"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_SIGALGS_EXTENSION),
    "missing sigalgs extension"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_SIGNING_CERT),
    "missing signing cert"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_SRP_PARAM),
    "can't find SRP server param"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_SUPPORTED_GROUPS_EXTENSION),
    "missing supported groups extension"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_TMP_DH_KEY), "missing tmp dh key"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MISSING_TMP_ECDH_KEY),
    "missing tmp ecdh key"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_MIXED_HANDSHAKE_AND_NON_HANDSHAKE_DATA),
    "mixed handshake and non handshake data"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NOT_ON_RECORD_BOUNDARY),
    "not on record boundary"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NOT_REPLACING_CERTIFICATE),
    "not replacing certificate"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NOT_SERVER), "not server"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_APPLICATION_PROTOCOL),
    "no application protocol"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_CERTIFICATES_RETURNED),
    "no certificates returned"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_CERTIFICATE_ASSIGNED),
    "no certificate assigned"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_CERTIFICATE_SET), "no certificate set"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_CHANGE_FOLLOWING_HRR),
    "no change following hrr"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_CIPHERS_AVAILABLE),
    "no ciphers available"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_CIPHERS_SPECIFIED),
    "no ciphers specified"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_CIPHER_MATCH), "no cipher match"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_CLIENT_CERT_METHOD),
    "no client cert method"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_COMPRESSION_SPECIFIED),
    "no compression specified"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_COOKIE_CALLBACK_SET),
    "no cookie callback set"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_GOST_CERTIFICATE_SENT_BY_PEER),
    "Peer haven't sent GOST certificate, required for selected ciphersuite"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_METHOD_SPECIFIED),
    "no method specified"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_PEM_EXTENSIONS), "no pem extensions"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_PRIVATE_KEY_ASSIGNED),
    "no private key assigned"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_PROTOCOLS_AVAILABLE),
    "no protocols available"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_RENEGOTIATION), "no renegotiation"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_REQUIRED_DIGEST), "no required digest"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_SHARED_CIPHER), "no shared cipher"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_SHARED_GROUPS), "no shared groups"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_SHARED_SIGNATURE_ALGORITHMS),
    "no shared signature algorithms"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_SRTP_PROFILES), "no srtp profiles"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_SUITABLE_DIGEST_ALGORITHM),
    "no suitable digest algorithm"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_SUITABLE_KEY_SHARE),
    "no suitable key share"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_SUITABLE_SIGNATURE_ALGORITHM),
    "no suitable signature algorithm"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_VALID_SCTS), "no valid scts"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NO_VERIFY_COOKIE_CALLBACK),
    "no verify cookie callback"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NULL_SSL_CTX), "null ssl ctx"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_NULL_SSL_METHOD_PASSED),
    "null ssl method passed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_OLD_SESSION_CIPHER_NOT_RETURNED),
    "old session cipher not returned"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_OLD_SESSION_COMPRESSION_ALGORITHM_NOT_RETURNED),
    "old session compression algorithm not returned"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_OVERFLOW_ERROR), "overflow error"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PACKET_LENGTH_TOO_LONG),
    "packet length too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PARSE_TLSEXT), "parse tlsext"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PATH_TOO_LONG), "path too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PEER_DID_NOT_RETURN_A_CERTIFICATE),
    "peer did not return a certificate"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PEM_NAME_BAD_PREFIX),
    "pem name bad prefix"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PEM_NAME_TOO_SHORT), "pem name too short"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PIPELINE_FAILURE), "pipeline failure"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_POST_HANDSHAKE_AUTH_ENCODING_ERR),
    "post handshake auth encoding err"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PRIVATE_KEY_MISMATCH),
    "private key mismatch"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PROTOCOL_IS_SHUTDOWN),
    "protocol is shutdown"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PSK_IDENTITY_NOT_FOUND),
    "psk identity not found"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PSK_NO_CLIENT_CB), "psk no client cb"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_PSK_NO_SERVER_CB), "psk no server cb"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_READ_BIO_NOT_SET), "read bio not set"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_READ_TIMEOUT_EXPIRED),
    "read timeout expired"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_RECORD_LENGTH_MISMATCH),
    "record length mismatch"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_RECORD_TOO_SMALL), "record too small"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_RENEGOTIATE_EXT_TOO_LONG),
    "renegotiate ext too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_RENEGOTIATION_ENCODING_ERR),
    "renegotiation encoding err"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_RENEGOTIATION_MISMATCH),
    "renegotiation mismatch"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_REQUEST_PENDING), "request pending"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_REQUEST_SENT), "request sent"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_REQUIRED_CIPHER_MISSING),
    "required cipher missing"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_REQUIRED_COMPRESSION_ALGORITHM_MISSING),
    "required compression algorithm missing"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SCSV_RECEIVED_WHEN_RENEGOTIATING),
    "scsv received when renegotiating"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SCT_VERIFICATION_FAILED),
    "sct verification failed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SERVERHELLO_TLSEXT), "serverhello tlsext"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SESSION_ID_CONTEXT_UNINITIALIZED),
    "session id context uninitialized"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SHUTDOWN_WHILE_IN_INIT),
    "shutdown while in init"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SIGNATURE_ALGORITHMS_ERROR),
    "signature algorithms error"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SIGNATURE_FOR_NON_SIGNING_CERTIFICATE),
    "signature for non signing certificate"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SRP_A_CALC), "error with the srp params"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SRTP_COULD_NOT_ALLOCATE_PROFILES),
    "srtp could not allocate profiles"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SRTP_PROTECTION_PROFILE_LIST_TOO_LONG),
    "srtp protection profile list too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SRTP_UNKNOWN_PROTECTION_PROFILE),
    "srtp unknown protection profile"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL3_EXT_INVALID_MAX_FRAGMENT_LENGTH),
    "ssl3 ext invalid max fragment length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL3_EXT_INVALID_SERVERNAME),
    "ssl3 ext invalid servername"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL3_EXT_INVALID_SERVERNAME_TYPE),
    "ssl3 ext invalid servername type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL3_SESSION_ID_TOO_LONG),
    "ssl3 session id too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_BAD_CERTIFICATE),
    "sslv3 alert bad certificate"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_BAD_RECORD_MAC),
    "sslv3 alert bad record mac"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED),
    "sslv3 alert certificate expired"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_CERTIFICATE_REVOKED),
    "sslv3 alert certificate revoked"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_CERTIFICATE_UNKNOWN),
    "sslv3 alert certificate unknown"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_DECOMPRESSION_FAILURE),
    "sslv3 alert decompression failure"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_HANDSHAKE_FAILURE),
    "sslv3 alert handshake failure"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_ILLEGAL_PARAMETER),
    "sslv3 alert illegal parameter"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_NO_CERTIFICATE),
    "sslv3 alert no certificate"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_UNEXPECTED_MESSAGE),
    "sslv3 alert unexpected message"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSLV3_ALERT_UNSUPPORTED_CERTIFICATE),
    "sslv3 alert unsupported certificate"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_COMMAND_SECTION_EMPTY),
    "ssl command section empty"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_COMMAND_SECTION_NOT_FOUND),
    "ssl command section not found"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_CTX_HAS_NO_DEFAULT_SSL_VERSION),
    "ssl ctx has no default ssl version"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_HANDSHAKE_FAILURE),
    "ssl handshake failure"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_LIBRARY_HAS_NO_CIPHERS),
    "ssl library has no ciphers"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_NEGATIVE_LENGTH),
    "ssl negative length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_SECTION_EMPTY), "ssl section empty"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_SECTION_NOT_FOUND),
    "ssl section not found"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_SESSION_ID_CALLBACK_FAILED),
    "ssl session id callback failed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_SESSION_ID_CONFLICT),
    "ssl session id conflict"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_SESSION_ID_CONTEXT_TOO_LONG),
    "ssl session id context too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_SESSION_ID_HAS_BAD_LENGTH),
    "ssl session id has bad length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_SESSION_ID_TOO_LONG),
    "ssl session id too long"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_SSL_SESSION_VERSION_MISMATCH),
    "ssl session version mismatch"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_STILL_IN_INIT), "still in init"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED),
    "tlsv13 alert certificate required"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV13_ALERT_MISSING_EXTENSION),
    "tlsv13 alert missing extension"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_ACCESS_DENIED),
    "tlsv1 alert access denied"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_DECODE_ERROR),
    "tlsv1 alert decode error"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_DECRYPTION_FAILED),
    "tlsv1 alert decryption failed"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_DECRYPT_ERROR),
    "tlsv1 alert decrypt error"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_EXPORT_RESTRICTION),
    "tlsv1 alert export restriction"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_INAPPROPRIATE_FALLBACK),
    "tlsv1 alert inappropriate fallback"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_INSUFFICIENT_SECURITY),
    "tlsv1 alert insufficient security"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_INTERNAL_ERROR),
    "tlsv1 alert internal error"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_NO_RENEGOTIATION),
    "tlsv1 alert no renegotiation"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_PROTOCOL_VERSION),
    "tlsv1 alert protocol version"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_RECORD_OVERFLOW),
    "tlsv1 alert record overflow"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_UNKNOWN_CA),
    "tlsv1 alert unknown ca"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_ALERT_USER_CANCELLED),
    "tlsv1 alert user cancelled"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_BAD_CERTIFICATE_HASH_VALUE),
    "tlsv1 bad certificate hash value"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_BAD_CERTIFICATE_STATUS_RESPONSE),
    "tlsv1 bad certificate status response"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_CERTIFICATE_UNOBTAINABLE),
    "tlsv1 certificate unobtainable"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_UNRECOGNIZED_NAME),
    "tlsv1 unrecognized name"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLSV1_UNSUPPORTED_EXTENSION),
    "tlsv1 unsupported extension"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLS_ILLEGAL_EXPORTER_LABEL),
    "tls illegal exporter label"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TLS_INVALID_ECPOINTFORMAT_LIST),
    "tls invalid ecpointformat list"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TOO_MANY_KEY_UPDATES),
    "too many key updates"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TOO_MANY_WARN_ALERTS),
    "too many warn alerts"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_TOO_MUCH_EARLY_DATA),
    "too much early data"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNABLE_TO_FIND_ECDH_PARAMETERS),
    "unable to find ecdh parameters"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNABLE_TO_FIND_PUBLIC_KEY_PARAMETERS),
    "unable to find public key parameters"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNABLE_TO_LOAD_SSL3_MD5_ROUTINES),
    "unable to load ssl3 md5 routines"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNABLE_TO_LOAD_SSL3_SHA1_ROUTINES),
    "unable to load ssl3 sha1 routines"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNEXPECTED_CCS_MESSAGE),
    "unexpected ccs message"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNEXPECTED_END_OF_EARLY_DATA),
    "unexpected end of early data"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNEXPECTED_EOF_WHILE_READING),
    "unexpected eof while reading"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNEXPECTED_MESSAGE), "unexpected message"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNEXPECTED_RECORD), "unexpected record"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNINITIALIZED), "uninitialized"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_ALERT_TYPE), "unknown alert type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_CERTIFICATE_TYPE),
    "unknown certificate type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_CIPHER_RETURNED),
    "unknown cipher returned"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_CIPHER_TYPE),
    "unknown cipher type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_CMD_NAME), "unknown cmd name"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_COMMAND), "unknown command"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_DIGEST), "unknown digest"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_KEY_EXCHANGE_TYPE),
    "unknown key exchange type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_PKEY_TYPE), "unknown pkey type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_PROTOCOL), "unknown protocol"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_SSL_VERSION),
    "unknown ssl version"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNKNOWN_STATE), "unknown state"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNSAFE_LEGACY_RENEGOTIATION_DISABLED),
    "unsafe legacy renegotiation disabled"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNSOLICITED_EXTENSION),
    "unsolicited extension"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNSUPPORTED_COMPRESSION_ALGORITHM),
    "unsupported compression algorithm"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNSUPPORTED_ELLIPTIC_CURVE),
    "unsupported elliptic curve"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNSUPPORTED_PROTOCOL),
    "unsupported protocol"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNSUPPORTED_SSL_VERSION),
    "unsupported ssl version"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_UNSUPPORTED_STATUS_TYPE),
    "unsupported status type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_USE_SRTP_NOT_NEGOTIATED),
    "use srtp not negotiated"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_VERSION_TOO_HIGH), "version too high"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_VERSION_TOO_LOW), "version too low"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_WRONG_CERTIFICATE_TYPE),
    "wrong certificate type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_WRONG_CIPHER_RETURNED),
    "wrong cipher returned"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_WRONG_CURVE), "wrong curve"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_WRONG_ENCRYPTION_LEVEL_RECEIVED),
    "wrong encryption level received"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_WRONG_SIGNATURE_LENGTH),
    "wrong signature length"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_WRONG_SIGNATURE_SIZE),
    "wrong signature size"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_WRONG_SIGNATURE_TYPE),
    "wrong signature type"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_WRONG_SSL_VERSION), "wrong ssl version"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_WRONG_VERSION_NUMBER),
    "wrong version number"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_X509_LIB), "x509 lib"},
    {ERR_PACK(ERR_LIB_SSL, 0, SSL_R_X509_VERIFICATION_SETUP_PROBLEMS),
    "x509 verification setup problems"},
    {0, NULL}
};

#endif

int err_load_SSL_strings_int(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_reason_error_string(SSL_str_reasons[0].error) == NULL)
        ERR_load_strings_const(SSL_str_reasons);
#endif
    return 1;
}
