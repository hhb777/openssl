/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OPENSSL_UIERR_H
# define OPENSSL_UIERR_H

# ifndef OPENSSL_SYMHACKS_H
#  include <openssl/symhacks.h>
# endif

# ifdef  __cplusplus
extern "C"
# endif
int ERR_load_UI_strings(void);

/*
 * UI function codes.
 */
# define UI_F_CLOSE_CONSOLE                               115
# define UI_F_ECHO_CONSOLE                                116
# define UI_F_GENERAL_ALLOCATE_BOOLEAN                    108
# define UI_F_GENERAL_ALLOCATE_PROMPT                     109
# define UI_F_NOECHO_CONSOLE                              117
# define UI_F_OPEN_CONSOLE                                114
# define UI_F_UI_CONSTRUCT_PROMPT                         121
# define UI_F_UI_CREATE_METHOD                            112
# define UI_F_UI_CTRL                                     111
# define UI_F_UI_DUP_ERROR_STRING                         101
# define UI_F_UI_DUP_INFO_STRING                          102
# define UI_F_UI_DUP_INPUT_BOOLEAN                        110
# define UI_F_UI_DUP_INPUT_STRING                         103
# define UI_F_UI_DUP_USER_DATA                            118
# define UI_F_UI_DUP_VERIFY_STRING                        106
# define UI_F_UI_GET0_RESULT                              107
# define UI_F_UI_GET_RESULT_LENGTH                        119
# define UI_F_UI_NEW_METHOD                               104
# define UI_F_UI_PROCESS                                  113
# define UI_F_UI_SET_RESULT                               105
# define UI_F_UI_SET_RESULT_EX                            120

/*
 * UI reason codes.
 */
# define UI_R_COMMON_OK_AND_CANCEL_CHARACTERS             104
# define UI_R_INDEX_TOO_LARGE                             102
# define UI_R_INDEX_TOO_SMALL                             103
# define UI_R_NO_RESULT_BUFFER                            105
# define UI_R_PROCESSING_ERROR                            107
# define UI_R_RESULT_TOO_LARGE                            100
# define UI_R_RESULT_TOO_SMALL                            101
# define UI_R_SYSASSIGN_ERROR                             109
# define UI_R_SYSDASSGN_ERROR                             110
# define UI_R_SYSQIOW_ERROR                               111
# define UI_R_UNKNOWN_CONTROL_COMMAND                     106
# define UI_R_UNKNOWN_TTYGET_ERRNO_VALUE                  108
# define UI_R_USER_DATA_DUPLICATION_UNSUPPORTED           112

#endif
