/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */
/* ====================================================================
 * Copyright (c) 1998-2001 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

#if !defined(_POSIX_C_SOURCE) && defined(OPENSSL_SYS_VMS)
/*
 * On VMS, you need to define this to get the declaration of fileno().  The
 * value 2 is to make sure no function defined in POSIX-2 is left undefined.
 */
# define _POSIX_C_SOURCE 2
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(OPENSSL_SYS_WIN32) && !defined(OPENSSL_SYS_WINCE) && !defined(NETWARE_CLIB)
# include <strings.h>
#endif
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/ui.h>
#include <openssl/safestack.h>
#ifndef OPENSSL_NO_ENGINE
# include <openssl/engine.h>
#endif
#ifndef OPENSSL_NO_RSA
# include <openssl/rsa.h>
#endif
#include <openssl/bn.h>
#ifndef OPENSSL_NO_JPAKE
# include <openssl/jpake.h>
#endif
#include <openssl/ssl.h>

#include "apps.h"

#ifdef _WIN32
static int WIN32_rename(const char *from, const char *to);
# define rename(from,to) WIN32_rename((from),(to))
#endif

typedef struct {
    const char *name;
    unsigned long flag;
    unsigned long mask;
} NAME_EX_TBL;

static UI_METHOD *ui_method = NULL;

static int set_table_opts(unsigned long *flags, const char *arg,
                          const NAME_EX_TBL * in_tbl);
static int set_multi_opts(unsigned long *flags, const char *arg,
                          const NAME_EX_TBL * in_tbl);

#if !defined(OPENSSL_NO_RC4) && !defined(OPENSSL_NO_RSA)
/* Looks like this stuff is worth moving into separate function */
static EVP_PKEY *load_netscape_key(BIO *key, const char *file,
                                   const char *key_descrip, int format);
#endif

int app_init(long mesgwin);

int chopup_args(ARGS *arg, char *buf)
{
    int quoted;
    char c = '\0', *p = NULL;

    arg->argc = 0;
    if (arg->size == 0) {
        arg->size = 20;
        arg->argv = app_malloc(sizeof(*arg->argv) * arg->size, "argv space");
        if (arg->argv == NULL)
            return 0;
    }

    for (p = buf;;) {
        /* Skip whitespace. */
        while (*p && isspace(*p))
            p++;
        if (!*p)
            break;

        /* The start of something good :-) */
        if (arg->argc >= arg->size) {
            arg->size += 20;
            arg->argv = OPENSSL_realloc(arg->argv,
                                        sizeof(*arg->argv) * arg->size);
            if (arg->argv == NULL)
                return 0;
        }
        quoted = *p == '\'' || *p == '"';
        if (quoted)
            c = *p++;
        arg->argv[arg->argc++] = p;

        /* now look for the end of this */
        if (quoted) {
            while (*p && *p != c)
                p++;
            *p++ = '\0';
        } else {
            while (*p && !isspace(*p))
                p++;
            if (*p)
                *p++ = '\0';
        }
    }
    arg->argv[arg->argc] = NULL;
    return (1);
}

#ifndef APP_INIT
int app_init(long mesgwin)
{
    return (1);
}
#endif

int ctx_set_verify_locations(SSL_CTX *ctx,
                             const char *CAfile, const char *CApath)
{
    if (CAfile == NULL && CApath == NULL)
        return SSL_CTX_set_default_verify_paths(ctx);
    return SSL_CTX_load_verify_locations(ctx, CAfile, CApath);
}

int dump_cert_text(BIO *out, X509 *x)
{
    char *p;

    p = X509_NAME_oneline(X509_get_subject_name(x), NULL, 0);
    BIO_puts(out, "subject=");
    BIO_puts(out, p);
    OPENSSL_free(p);

    p = X509_NAME_oneline(X509_get_issuer_name(x), NULL, 0);
    BIO_puts(out, "\nissuer=");
    BIO_puts(out, p);
    BIO_puts(out, "\n");
    OPENSSL_free(p);

    return 0;
}

static int ui_open(UI *ui)
{
    return UI_method_get_opener(UI_OpenSSL())(ui);
}

static int ui_read(UI *ui, UI_STRING *uis)
{
    if (UI_get_input_flags(uis) & UI_INPUT_FLAG_DEFAULT_PWD
        && UI_get0_user_data(ui)) {
        switch (UI_get_string_type(uis)) {
        case UIT_PROMPT:
        case UIT_VERIFY:
            {
                const char *password =
                    ((PW_CB_DATA *)UI_get0_user_data(ui))->password;
                if (password && password[0] != '\0') {
                    UI_set_result(ui, uis, password);
                    return 1;
                }
            }
        default:
            break;
        }
    }
    return UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

static int ui_write(UI *ui, UI_STRING *uis)
{
    if (UI_get_input_flags(uis) & UI_INPUT_FLAG_DEFAULT_PWD
        && UI_get0_user_data(ui)) {
        switch (UI_get_string_type(uis)) {
        case UIT_PROMPT:
        case UIT_VERIFY:
            {
                const char *password =
                    ((PW_CB_DATA *)UI_get0_user_data(ui))->password;
                if (password && password[0] != '\0')
                    return 1;
            }
        default:
            break;
        }
    }
    return UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

static int ui_close(UI *ui)
{
    return UI_method_get_closer(UI_OpenSSL())(ui);
}

int setup_ui_method(void)
{
    ui_method = UI_create_method("OpenSSL application user interface");
    UI_method_set_opener(ui_method, ui_open);
    UI_method_set_reader(ui_method, ui_read);
    UI_method_set_writer(ui_method, ui_write);
    UI_method_set_closer(ui_method, ui_close);
    return 0;
}

void destroy_ui_method(void)
{
    if (ui_method) {
        UI_destroy_method(ui_method);
        ui_method = NULL;
    }
}

int password_callback(char *buf, int bufsiz, int verify, PW_CB_DATA *cb_tmp)
{
    UI *ui = NULL;
    int res = 0;
    const char *prompt_info = NULL;
    const char *password = NULL;
    PW_CB_DATA *cb_data = (PW_CB_DATA *)cb_tmp;

    if (cb_data) {
        if (cb_data->password)
            password = cb_data->password;
        if (cb_data->prompt_info)
            prompt_info = cb_data->prompt_info;
    }

    if (password) {
        res = strlen(password);
        if (res > bufsiz)
            res = bufsiz;
        memcpy(buf, password, res);
        return res;
    }

    ui = UI_new_method(ui_method);
    if (ui) {
        int ok = 0;
        char *buff = NULL;
        int ui_flags = 0;
        char *prompt;

        prompt = UI_construct_prompt(ui, "pass phrase", prompt_info);
        if (!prompt) {
            BIO_printf(bio_err, "Out of memory\n");
            UI_free(ui);
            return 0;
        }

        ui_flags |= UI_INPUT_FLAG_DEFAULT_PWD;
        UI_ctrl(ui, UI_CTRL_PRINT_ERRORS, 1, 0, 0);

        if (ok >= 0)
            ok = UI_add_input_string(ui, prompt, ui_flags, buf,
                                     PW_MIN_LENGTH, bufsiz - 1);
        if (ok >= 0 && verify) {
            buff = app_malloc(bufsiz, "password buffer");
            ok = UI_add_verify_string(ui, prompt, ui_flags, buff,
                                      PW_MIN_LENGTH, bufsiz - 1, buf);
        }
        if (ok >= 0)
            do {
                ok = UI_process(ui);
            }
            while (ok < 0 && UI_ctrl(ui, UI_CTRL_IS_REDOABLE, 0, 0, 0));

        OPENSSL_clear_free(buff, (unsigned int)bufsiz);

        if (ok >= 0)
            res = strlen(buf);
        if (ok == -1) {
            BIO_printf(bio_err, "User interface error\n");
            ERR_print_errors(bio_err);
            OPENSSL_cleanse(buf, (unsigned int)bufsiz);
            res = 0;
        }
        if (ok == -2) {
            BIO_printf(bio_err, "aborted!\n");
            OPENSSL_cleanse(buf, (unsigned int)bufsiz);
            res = 0;
        }
        UI_free(ui);
        OPENSSL_free(prompt);
    }
    return res;
}

static char *app_get_pass(char *arg, int keepbio);

int app_passwd(char *arg1, char *arg2, char **pass1, char **pass2)
{
    int same;
    if (!arg2 || !arg1 || strcmp(arg1, arg2))
        same = 0;
    else
        same = 1;
    if (arg1) {
        *pass1 = app_get_pass(arg1, same);
        if (!*pass1)
            return 0;
    } else if (pass1)
        *pass1 = NULL;
    if (arg2) {
        *pass2 = app_get_pass(arg2, same ? 2 : 0);
        if (!*pass2)
            return 0;
    } else if (pass2)
        *pass2 = NULL;
    return 1;
}

static char *app_get_pass(char *arg, int keepbio)
{
    char *tmp, tpass[APP_PASS_LEN];
    static BIO *pwdbio = NULL;
    int i;

    if (strncmp(arg, "pass:", 5) == 0)
        return BUF_strdup(arg + 5);
    if (strncmp(arg, "env:", 4) == 0) {
        tmp = getenv(arg + 4);
        if (!tmp) {
            BIO_printf(bio_err, "Can't read environment variable %s\n", arg + 4);
            return NULL;
        }
        return BUF_strdup(tmp);
    }
    if (!keepbio || !pwdbio) {
        if (strncmp(arg, "file:", 5) == 0) {
            pwdbio = BIO_new_file(arg + 5, "r");
            if (!pwdbio) {
                BIO_printf(bio_err, "Can't open file %s\n", arg + 5);
                return NULL;
            }
#if !defined(_WIN32)
            /*
             * Under _WIN32, which covers even Win64 and CE, file
             * descriptors referenced by BIO_s_fd are not inherited
             * by child process and therefore below is not an option.
             * It could have been an option if bss_fd.c was operating
             * on real Windows descriptors, such as those obtained
             * with CreateFile.
             */
        } else if (strncmp(arg, "fd:", 3) == 0) {
            BIO *btmp;
            i = atoi(arg + 3);
            if (i >= 0)
                pwdbio = BIO_new_fd(i, BIO_NOCLOSE);
            if ((i < 0) || !pwdbio) {
                BIO_printf(bio_err, "Can't access file descriptor %s\n", arg + 3);
                return NULL;
            }
            /*
             * Can't do BIO_gets on an fd BIO so add a buffering BIO
             */
            btmp = BIO_new(BIO_f_buffer());
            pwdbio = BIO_push(btmp, pwdbio);
#endif
        } else if (strcmp(arg, "stdin") == 0) {
            pwdbio = dup_bio_in();
            if (!pwdbio) {
                BIO_printf(bio_err, "Can't open BIO for stdin\n");
                return NULL;
            }
        } else {
            BIO_printf(bio_err, "Invalid password argument \"%s\"\n", arg);
            return NULL;
        }
    }
    i = BIO_gets(pwdbio, tpass, APP_PASS_LEN);
    if (keepbio != 1) {
        BIO_free_all(pwdbio);
        pwdbio = NULL;
    }
    if (i <= 0) {
        BIO_printf(bio_err, "Error reading password from BIO\n");
        return NULL;
    }
    tmp = strchr(tpass, '\n');
    if (tmp)
        *tmp = 0;
    return BUF_strdup(tpass);
}

CONF *app_load_config(const char *filename)
{
    long errorline = -1;
    CONF *conf;
    int i;
    BIO *in;

    in = bio_open_default(filename, "r");
    if (in == NULL)
        return NULL;

    conf = NCONF_new(NULL);
    i = NCONF_load_bio(conf, in, &errorline);
    BIO_free(in);
    if (i > 0)
        return conf;

    if (errorline <= 0)
        BIO_printf(bio_err, "%s: Can't load config file \"%s\"\n",
                   opt_getprog(), filename);
    else
        BIO_printf(bio_err, "%s: Error on line %ld of config file \"%s\"\n",
                   opt_getprog(), errorline, filename);
    NCONF_free(conf);
    return NULL;
}

int add_oid_section(CONF *conf)
{
    char *p;
    STACK_OF(CONF_VALUE) *sktmp;
    CONF_VALUE *cnf;
    int i;

    if ((p = NCONF_get_string(conf, NULL, "oid_section")) == NULL) {
        ERR_clear_error();
        return 1;
    }
    if ((sktmp = NCONF_get_section(conf, p)) == NULL) {
        BIO_printf(bio_err, "problem loading oid section %s\n", p);
        return 0;
    }
    for (i = 0; i < sk_CONF_VALUE_num(sktmp); i++) {
        cnf = sk_CONF_VALUE_value(sktmp, i);
        if (OBJ_create(cnf->value, cnf->name, cnf->name) == NID_undef) {
            BIO_printf(bio_err, "problem creating object %s=%s\n",
                       cnf->name, cnf->value);
            return 0;
        }
    }
    return 1;
}

static int load_pkcs12(BIO *in, const char *desc,
                       pem_password_cb *pem_cb, void *cb_data,
                       EVP_PKEY **pkey, X509 **cert, STACK_OF(X509) **ca)
{
    const char *pass;
    char tpass[PEM_BUFSIZE];
    int len, ret = 0;
    PKCS12 *p12;
    p12 = d2i_PKCS12_bio(in, NULL);
    if (p12 == NULL) {
        BIO_printf(bio_err, "Error loading PKCS12 file for %s\n", desc);
        goto die;
    }
    /* See if an empty password will do */
    if (PKCS12_verify_mac(p12, "", 0) || PKCS12_verify_mac(p12, NULL, 0))
        pass = "";
    else {
        if (!pem_cb)
            pem_cb = (pem_password_cb *)password_callback;
        len = pem_cb(tpass, PEM_BUFSIZE, 0, cb_data);
        if (len < 0) {
            BIO_printf(bio_err, "Passphrase callback error for %s\n", desc);
            goto die;
        }
        if (len < PEM_BUFSIZE)
            tpass[len] = 0;
        if (!PKCS12_verify_mac(p12, tpass, len)) {
            BIO_printf(bio_err,
                       "Mac verify error (wrong password?) in PKCS12 file for %s\n",
                       desc);
            goto die;
        }
        pass = tpass;
    }
    ret = PKCS12_parse(p12, pass, pkey, cert, ca);
 die:
    PKCS12_free(p12);
    return ret;
}

int load_cert_crl_http(const char *url, X509 **pcert, X509_CRL **pcrl)
{
    char *host = NULL, *port = NULL, *path = NULL;
    BIO *bio = NULL;
    OCSP_REQ_CTX *rctx = NULL;
    int use_ssl, rv = 0;
    if (!OCSP_parse_url(url, &host, &port, &path, &use_ssl))
        goto err;
    if (use_ssl) {
        BIO_puts(bio_err, "https not supported\n");
        goto err;
    }
    bio = BIO_new_connect(host);
    if (!bio || !BIO_set_conn_port(bio, port))
        goto err;
    rctx = OCSP_REQ_CTX_new(bio, 1024);
    if (!rctx)
        goto err;
    if (!OCSP_REQ_CTX_http(rctx, "GET", path))
        goto err;
    if (!OCSP_REQ_CTX_add1_header(rctx, "Host", host))
        goto err;
    if (pcert) {
        do {
            rv = X509_http_nbio(rctx, pcert);
        } while (rv == -1);
    } else {
        do {
            rv = X509_CRL_http_nbio(rctx, pcrl);
        } while (rv == -1);
    }

 err:
    OPENSSL_free(host);
    OPENSSL_free(path);
    OPENSSL_free(port);
    if (bio)
        BIO_free_all(bio);
    OCSP_REQ_CTX_free(rctx);
    if (rv != 1) {
        BIO_printf(bio_err, "Error loading %s from %s\n",
                   pcert ? "certificate" : "CRL", url);
        ERR_print_errors(bio_err);
    }
    return rv;
}

X509 *load_cert(const char *file, int format,
                const char *pass, ENGINE *e, const char *cert_descrip)
{
    X509 *x = NULL;
    BIO *cert;

    if (format == FORMAT_HTTP) {
        load_cert_crl_http(file, &x, NULL);
        return x;
    }

    if (file == NULL) {
        unbuffer(stdin);
        cert = dup_bio_in();
    } else
        cert = bio_open_default(file, RB(format));
    if (cert == NULL)
        goto end;

    if (format == FORMAT_ASN1)
        x = d2i_X509_bio(cert, NULL);
    else if (format == FORMAT_NETSCAPE) {
        NETSCAPE_X509 *nx;
        nx = ASN1_item_d2i_bio(ASN1_ITEM_rptr(NETSCAPE_X509), cert, NULL);
        if (nx == NULL)
            goto end;

        if ((strncmp(NETSCAPE_CERT_HDR, (char *)nx->header->data,
                     nx->header->length) != 0)) {
            NETSCAPE_X509_free(nx);
            BIO_printf(bio_err, "Error reading header on certificate\n");
            goto end;
        }
        x = nx->cert;
        nx->cert = NULL;
        NETSCAPE_X509_free(nx);
    } else if (format == FORMAT_PEM)
        x = PEM_read_bio_X509_AUX(cert, NULL,
                                  (pem_password_cb *)password_callback, NULL);
    else if (format == FORMAT_PKCS12) {
        if (!load_pkcs12(cert, cert_descrip, NULL, NULL, NULL, &x, NULL))
            goto end;
    } else {
        BIO_printf(bio_err, "bad input format specified for %s\n", cert_descrip);
        goto end;
    }
 end:
    if (x == NULL) {
        BIO_printf(bio_err, "unable to load certificate\n");
        ERR_print_errors(bio_err);
    }
    BIO_free(cert);
    return (x);
}

X509_CRL *load_crl(const char *infile, int format)
{
    X509_CRL *x = NULL;
    BIO *in = NULL;

    if (format == FORMAT_HTTP) {
        load_cert_crl_http(infile, NULL, &x);
        return x;
    }

    in = bio_open_default(infile, RB(format));
    if (in == NULL)
        goto end;
    if (format == FORMAT_ASN1)
        x = d2i_X509_CRL_bio(in, NULL);
    else if (format == FORMAT_PEM)
        x = PEM_read_bio_X509_CRL(in, NULL, NULL, NULL);
    else {
        BIO_printf(bio_err, "bad input format specified for input crl\n");
        goto end;
    }
    if (x == NULL) {
        BIO_printf(bio_err, "unable to load CRL\n");
        ERR_print_errors(bio_err);
        goto end;
    }

 end:
    BIO_free(in);
    return (x);
}

EVP_PKEY *load_key(const char *file, int format, int maybe_stdin,
                   const char *pass, ENGINE *e, const char *key_descrip)
{
    BIO *key = NULL;
    EVP_PKEY *pkey = NULL;
    PW_CB_DATA cb_data;

    cb_data.password = pass;
    cb_data.prompt_info = file;

    if (file == NULL && (!maybe_stdin || format == FORMAT_ENGINE)) {
        BIO_printf(bio_err, "no keyfile specified\n");
        goto end;
    }
#ifndef OPENSSL_NO_ENGINE
    if (format == FORMAT_ENGINE) {
        if (!e)
            BIO_printf(bio_err, "no engine specified\n");
        else {
            pkey = ENGINE_load_private_key(e, file, ui_method, &cb_data);
            if (!pkey) {
                BIO_printf(bio_err, "cannot load %s from engine\n", key_descrip);
                ERR_print_errors(bio_err);
            }
        }
        goto end;
    }
#endif
    if (file == NULL && maybe_stdin) {
        unbuffer(stdin);
        key = dup_bio_in();
    } else
        key = bio_open_default(file, RB(format));
    if (key == NULL)
        goto end;
    if (format == FORMAT_ASN1) {
        pkey = d2i_PrivateKey_bio(key, NULL);
    } else if (format == FORMAT_PEM) {
        pkey = PEM_read_bio_PrivateKey(key, NULL,
                                       (pem_password_cb *)password_callback,
                                       &cb_data);
    }
#if !defined(OPENSSL_NO_RC4) && !defined(OPENSSL_NO_RSA)
    else if (format == FORMAT_NETSCAPE)
        pkey = load_netscape_key(key, file, key_descrip, format);
#endif
    else if (format == FORMAT_PKCS12) {
        if (!load_pkcs12(key, key_descrip,
                         (pem_password_cb *)password_callback, &cb_data,
                         &pkey, NULL, NULL))
            goto end;
    }
#if !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DSA) && !defined (OPENSSL_NO_RC4)
    else if (format == FORMAT_MSBLOB)
        pkey = b2i_PrivateKey_bio(key);
    else if (format == FORMAT_PVK)
        pkey = b2i_PVK_bio(key, (pem_password_cb *)password_callback,
                           &cb_data);
#endif
    else {
        BIO_printf(bio_err, "bad input format specified for key file\n");
        goto end;
    }
 end:
    BIO_free(key);
    if (pkey == NULL) {
        BIO_printf(bio_err, "unable to load %s\n", key_descrip);
        ERR_print_errors(bio_err);
    }
    return (pkey);
}

static const char *key_file_format(int format)
{
    if (format == FORMAT_PEM || format == FORMAT_PEMRSA)
        return "r";
    return "rb";
}

EVP_PKEY *load_pubkey(const char *file, int format, int maybe_stdin,
                      const char *pass, ENGINE *e, const char *key_descrip)
{
    BIO *key = NULL;
    EVP_PKEY *pkey = NULL;
    PW_CB_DATA cb_data;

    cb_data.password = pass;
    cb_data.prompt_info = file;

    if (file == NULL && (!maybe_stdin || format == FORMAT_ENGINE)) {
        BIO_printf(bio_err, "no keyfile specified\n");
        goto end;
    }
#ifndef OPENSSL_NO_ENGINE
    if (format == FORMAT_ENGINE) {
        if (!e)
            BIO_printf(bio_err, "no engine specified\n");
        else
            pkey = ENGINE_load_public_key(e, file, ui_method, &cb_data);
        goto end;
    }
#endif
    if (file == NULL && maybe_stdin) {
        unbuffer(stdin);
        key = dup_bio_in();
    } else
        key = bio_open_default(file, key_file_format(format));
    if (key == NULL)
        goto end;
    if (format == FORMAT_ASN1) {
        pkey = d2i_PUBKEY_bio(key, NULL);
    }
#ifndef OPENSSL_NO_RSA
    else if (format == FORMAT_ASN1RSA) {
        RSA *rsa;
        rsa = d2i_RSAPublicKey_bio(key, NULL);
        if (rsa) {
            pkey = EVP_PKEY_new();
            if (pkey)
                EVP_PKEY_set1_RSA(pkey, rsa);
            RSA_free(rsa);
        } else
            pkey = NULL;
    } else if (format == FORMAT_PEMRSA) {
        RSA *rsa;
        rsa = PEM_read_bio_RSAPublicKey(key, NULL,
                                        (pem_password_cb *)password_callback,
                                        &cb_data);
        if (rsa) {
            pkey = EVP_PKEY_new();
            if (pkey)
                EVP_PKEY_set1_RSA(pkey, rsa);
            RSA_free(rsa);
        } else
            pkey = NULL;
    }
#endif
    else if (format == FORMAT_PEM) {
        pkey = PEM_read_bio_PUBKEY(key, NULL,
                                   (pem_password_cb *)password_callback,
                                   &cb_data);
    }
#if !defined(OPENSSL_NO_RC4) && !defined(OPENSSL_NO_RSA)
    else if (format == FORMAT_NETSCAPE)
        pkey = load_netscape_key(key, file, key_descrip, format);
#endif
#if !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DSA)
    else if (format == FORMAT_MSBLOB)
        pkey = b2i_PublicKey_bio(key);
#endif
 end:
    BIO_free(key);
    if (pkey == NULL)
        BIO_printf(bio_err, "unable to load %s\n", key_descrip);
    return (pkey);
}

#if !defined(OPENSSL_NO_RC4) && !defined(OPENSSL_NO_RSA)
static EVP_PKEY *load_netscape_key(BIO *key, const char *file,
                                   const char *key_descrip, int format)
{
    EVP_PKEY *pkey;
    BUF_MEM *buf;
    RSA *rsa;
    const unsigned char *p;
    int size, i;

    buf = BUF_MEM_new();
    pkey = EVP_PKEY_new();
    size = 0;
    if (buf == NULL || pkey == NULL)
        goto error;
    for (;;) {
        if (!BUF_MEM_grow_clean(buf, size + 1024 * 10))
            goto error;
        i = BIO_read(key, &(buf->data[size]), 1024 * 10);
        size += i;
        if (i == 0)
            break;
        if (i < 0) {
            BIO_printf(bio_err, "Error reading %s %s", key_descrip, file);
            goto error;
        }
    }
    p = (unsigned char *)buf->data;
    rsa = d2i_RSA_NET(NULL, &p, (long)size, NULL, 0);
    if (rsa == NULL)
        goto error;
    BUF_MEM_free(buf);
    EVP_PKEY_set1_RSA(pkey, rsa);
    return pkey;
 error:
    BUF_MEM_free(buf);
    EVP_PKEY_free(pkey);
    return NULL;
}
#endif                          /* ndef OPENSSL_NO_RC4 */

static int load_certs_crls(const char *file, int format,
                           const char *pass, ENGINE *e, const char *desc,
                           STACK_OF(X509) **pcerts,
                           STACK_OF(X509_CRL) **pcrls)
{
    int i;
    BIO *bio;
    STACK_OF(X509_INFO) *xis = NULL;
    X509_INFO *xi;
    PW_CB_DATA cb_data;
    int rv = 0;

    cb_data.password = pass;
    cb_data.prompt_info = file;

    if (format != FORMAT_PEM) {
        BIO_printf(bio_err, "bad input format specified for %s\n", desc);
        return 0;
    }

    bio = bio_open_default(file, "r");
    if (bio == NULL)
        return 0;

    xis = PEM_X509_INFO_read_bio(bio, NULL,
                                 (pem_password_cb *)password_callback,
                                 &cb_data);

    BIO_free(bio);

    if (pcerts) {
        *pcerts = sk_X509_new_null();
        if (!*pcerts)
            goto end;
    }

    if (pcrls) {
        *pcrls = sk_X509_CRL_new_null();
        if (!*pcrls)
            goto end;
    }

    for (i = 0; i < sk_X509_INFO_num(xis); i++) {
        xi = sk_X509_INFO_value(xis, i);
        if (xi->x509 && pcerts) {
            if (!sk_X509_push(*pcerts, xi->x509))
                goto end;
            xi->x509 = NULL;
        }
        if (xi->crl && pcrls) {
            if (!sk_X509_CRL_push(*pcrls, xi->crl))
                goto end;
            xi->crl = NULL;
        }
    }

    if (pcerts && sk_X509_num(*pcerts) > 0)
        rv = 1;

    if (pcrls && sk_X509_CRL_num(*pcrls) > 0)
        rv = 1;

 end:

    sk_X509_INFO_pop_free(xis, X509_INFO_free);

    if (rv == 0) {
        if (pcerts) {
            sk_X509_pop_free(*pcerts, X509_free);
            *pcerts = NULL;
        }
        if (pcrls) {
            sk_X509_CRL_pop_free(*pcrls, X509_CRL_free);
            *pcrls = NULL;
        }
        BIO_printf(bio_err, "unable to load %s\n",
                   pcerts ? "certificates" : "CRLs");
        ERR_print_errors(bio_err);
    }
    return rv;
}

void* app_malloc(int sz, const char *what)
{
    void *vp = OPENSSL_malloc(sz);

    if (vp == NULL) {
        BIO_printf(bio_err, "%s: Could not allocate %d bytes for %s\n",
                opt_getprog(), sz, what);
        ERR_print_errors(bio_err);
        exit(1);
    }
    return vp;
}



STACK_OF(X509) *load_certs(const char *file, int format,
                           const char *pass, ENGINE *e, const char *desc)
{
    STACK_OF(X509) *certs;
    if (!load_certs_crls(file, format, pass, e, desc, &certs, NULL))
        return NULL;
    return certs;
}

STACK_OF(X509_CRL) *load_crls(const char *file, int format,
                              const char *pass, ENGINE *e, const char *desc)
{
    STACK_OF(X509_CRL) *crls;
    if (!load_certs_crls(file, format, pass, e, desc, NULL, &crls))
        return NULL;
    return crls;
}

#define X509V3_EXT_UNKNOWN_MASK         (0xfL << 16)
/* Return error for unknown extensions */
#define X509V3_EXT_DEFAULT              0
/* Print error for unknown extensions */
#define X509V3_EXT_ERROR_UNKNOWN        (1L << 16)
/* ASN1 parse unknown extensions */
#define X509V3_EXT_PARSE_UNKNOWN        (2L << 16)
/* BIO_dump unknown extensions */
#define X509V3_EXT_DUMP_UNKNOWN         (3L << 16)

#define X509_FLAG_CA (X509_FLAG_NO_ISSUER | X509_FLAG_NO_PUBKEY | \
                         X509_FLAG_NO_HEADER | X509_FLAG_NO_VERSION)

int set_cert_ex(unsigned long *flags, const char *arg)
{
    static const NAME_EX_TBL cert_tbl[] = {
        {"compatible", X509_FLAG_COMPAT, 0xffffffffl},
        {"ca_default", X509_FLAG_CA, 0xffffffffl},
        {"no_header", X509_FLAG_NO_HEADER, 0},
        {"no_version", X509_FLAG_NO_VERSION, 0},
        {"no_serial", X509_FLAG_NO_SERIAL, 0},
        {"no_signame", X509_FLAG_NO_SIGNAME, 0},
        {"no_validity", X509_FLAG_NO_VALIDITY, 0},
        {"no_subject", X509_FLAG_NO_SUBJECT, 0},
        {"no_issuer", X509_FLAG_NO_ISSUER, 0},
        {"no_pubkey", X509_FLAG_NO_PUBKEY, 0},
        {"no_extensions", X509_FLAG_NO_EXTENSIONS, 0},
        {"no_sigdump", X509_FLAG_NO_SIGDUMP, 0},
        {"no_aux", X509_FLAG_NO_AUX, 0},
        {"no_attributes", X509_FLAG_NO_ATTRIBUTES, 0},
        {"ext_default", X509V3_EXT_DEFAULT, X509V3_EXT_UNKNOWN_MASK},
        {"ext_error", X509V3_EXT_ERROR_UNKNOWN, X509V3_EXT_UNKNOWN_MASK},
        {"ext_parse", X509V3_EXT_PARSE_UNKNOWN, X509V3_EXT_UNKNOWN_MASK},
        {"ext_dump", X509V3_EXT_DUMP_UNKNOWN, X509V3_EXT_UNKNOWN_MASK},
        {NULL, 0, 0}
    };
    return set_multi_opts(flags, arg, cert_tbl);
}

int set_name_ex(unsigned long *flags, const char *arg)
{
    static const NAME_EX_TBL ex_tbl[] = {
        {"esc_2253", ASN1_STRFLGS_ESC_2253, 0},
        {"esc_ctrl", ASN1_STRFLGS_ESC_CTRL, 0},
        {"esc_msb", ASN1_STRFLGS_ESC_MSB, 0},
        {"use_quote", ASN1_STRFLGS_ESC_QUOTE, 0},
        {"utf8", ASN1_STRFLGS_UTF8_CONVERT, 0},
        {"ignore_type", ASN1_STRFLGS_IGNORE_TYPE, 0},
        {"show_type", ASN1_STRFLGS_SHOW_TYPE, 0},
        {"dump_all", ASN1_STRFLGS_DUMP_ALL, 0},
        {"dump_nostr", ASN1_STRFLGS_DUMP_UNKNOWN, 0},
        {"dump_der", ASN1_STRFLGS_DUMP_DER, 0},
        {"compat", XN_FLAG_COMPAT, 0xffffffffL},
        {"sep_comma_plus", XN_FLAG_SEP_COMMA_PLUS, XN_FLAG_SEP_MASK},
        {"sep_comma_plus_space", XN_FLAG_SEP_CPLUS_SPC, XN_FLAG_SEP_MASK},
        {"sep_semi_plus_space", XN_FLAG_SEP_SPLUS_SPC, XN_FLAG_SEP_MASK},
        {"sep_multiline", XN_FLAG_SEP_MULTILINE, XN_FLAG_SEP_MASK},
        {"dn_rev", XN_FLAG_DN_REV, 0},
        {"nofname", XN_FLAG_FN_NONE, XN_FLAG_FN_MASK},
        {"sname", XN_FLAG_FN_SN, XN_FLAG_FN_MASK},
        {"lname", XN_FLAG_FN_LN, XN_FLAG_FN_MASK},
        {"align", XN_FLAG_FN_ALIGN, 0},
        {"oid", XN_FLAG_FN_OID, XN_FLAG_FN_MASK},
        {"space_eq", XN_FLAG_SPC_EQ, 0},
        {"dump_unknown", XN_FLAG_DUMP_UNKNOWN_FIELDS, 0},
        {"RFC2253", XN_FLAG_RFC2253, 0xffffffffL},
        {"oneline", XN_FLAG_ONELINE, 0xffffffffL},
        {"multiline", XN_FLAG_MULTILINE, 0xffffffffL},
        {"ca_default", XN_FLAG_MULTILINE, 0xffffffffL},
        {NULL, 0, 0}
    };
    return set_multi_opts(flags, arg, ex_tbl);
}

int set_ext_copy(int *copy_type, const char *arg)
{
    if (strcasecmp(arg, "none") == 0)
        *copy_type = EXT_COPY_NONE;
    else if (strcasecmp(arg, "copy") == 0)
        *copy_type = EXT_COPY_ADD;
    else if (strcasecmp(arg, "copyall") == 0)
        *copy_type = EXT_COPY_ALL;
    else
        return 0;
    return 1;
}

int copy_extensions(X509 *x, X509_REQ *req, int copy_type)
{
    STACK_OF(X509_EXTENSION) *exts = NULL;
    X509_EXTENSION *ext, *tmpext;
    ASN1_OBJECT *obj;
    int i, idx, ret = 0;
    if (!x || !req || (copy_type == EXT_COPY_NONE))
        return 1;
    exts = X509_REQ_get_extensions(req);

    for (i = 0; i < sk_X509_EXTENSION_num(exts); i++) {
        ext = sk_X509_EXTENSION_value(exts, i);
        obj = X509_EXTENSION_get_object(ext);
        idx = X509_get_ext_by_OBJ(x, obj, -1);
        /* Does extension exist? */
        if (idx != -1) {
            /* If normal copy don't override existing extension */
            if (copy_type == EXT_COPY_ADD)
                continue;
            /* Delete all extensions of same type */
            do {
                tmpext = X509_get_ext(x, idx);
                X509_delete_ext(x, idx);
                X509_EXTENSION_free(tmpext);
                idx = X509_get_ext_by_OBJ(x, obj, -1);
            } while (idx != -1);
        }
        if (!X509_add_ext(x, ext, -1))
            goto end;
    }

    ret = 1;

 end:

    sk_X509_EXTENSION_pop_free(exts, X509_EXTENSION_free);

    return ret;
}

static int set_multi_opts(unsigned long *flags, const char *arg,
                          const NAME_EX_TBL * in_tbl)
{
    STACK_OF(CONF_VALUE) *vals;
    CONF_VALUE *val;
    int i, ret = 1;
    if (!arg)
        return 0;
    vals = X509V3_parse_list(arg);
    for (i = 0; i < sk_CONF_VALUE_num(vals); i++) {
        val = sk_CONF_VALUE_value(vals, i);
        if (!set_table_opts(flags, val->name, in_tbl))
            ret = 0;
    }
    sk_CONF_VALUE_pop_free(vals, X509V3_conf_free);
    return ret;
}

static int set_table_opts(unsigned long *flags, const char *arg,
                          const NAME_EX_TBL * in_tbl)
{
    char c;
    const NAME_EX_TBL *ptbl;
    c = arg[0];

    if (c == '-') {
        c = 0;
        arg++;
    } else if (c == '+') {
        c = 1;
        arg++;
    } else
        c = 1;

    for (ptbl = in_tbl; ptbl->name; ptbl++) {
        if (strcasecmp(arg, ptbl->name) == 0) {
            *flags &= ~ptbl->mask;
            if (c)
                *flags |= ptbl->flag;
            else
                *flags &= ~ptbl->flag;
            return 1;
        }
    }
    return 0;
}

void print_name(BIO *out, const char *title, X509_NAME *nm,
                unsigned long lflags)
{
    char *buf;
    char mline = 0;
    int indent = 0;

    if (title)
        BIO_puts(out, title);
    if ((lflags & XN_FLAG_SEP_MASK) == XN_FLAG_SEP_MULTILINE) {
        mline = 1;
        indent = 4;
    }
    if (lflags == XN_FLAG_COMPAT) {
        buf = X509_NAME_oneline(nm, 0, 0);
        BIO_puts(out, buf);
        BIO_puts(out, "\n");
        OPENSSL_free(buf);
    } else {
        if (mline)
            BIO_puts(out, "\n");
        X509_NAME_print_ex(out, nm, indent, lflags);
        BIO_puts(out, "\n");
    }
}

void print_bignum_var(BIO *out, BIGNUM *in, const char *var,
                      int len, unsigned char *buffer)
{
    BIO_printf(out, "    static unsigned char %s_%d[] = {", var, len);
    if (BN_is_zero(in))
        BIO_printf(out, "\n\t0x00");
    else {
        int i, l;

        l = BN_bn2bin(in, buffer);
        for (i = 0; i < l; i++) {
            if ((i % 10) == 0)
                BIO_printf(out, "\n\t");
            if (i < l - 1)
                BIO_printf(out, "0x%02X, ", buffer[i]);
            else
                BIO_printf(out, "0x%02X", buffer[i]);
        }
    }
    BIO_printf(out, "\n    };\n");
}
void print_array(BIO *out, const char* title, int len, const unsigned char* d)
{
    int i;

    BIO_printf(out, "unsigned char %s[%d] = {", title, len);
    for (i = 0; i < len; i++) {
        if ((i % 10) == 0)
            BIO_printf(out, "\n    ");
        if (i < len - 1)
            BIO_printf(out, "0x%02X, ", d[i]);
        else
            BIO_printf(out, "0x%02X", d[i]);
    }
    BIO_printf(out, "\n};\n");
}

X509_STORE *setup_verify(char *CAfile, char *CApath)
{
    X509_STORE *store = X509_STORE_new();
    X509_LOOKUP *lookup;

    if (!store)
        goto end;
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    if (lookup == NULL)
        goto end;
    if (CAfile) {
        if (!X509_LOOKUP_load_file(lookup, CAfile, X509_FILETYPE_PEM)) {
            BIO_printf(bio_err, "Error loading file %s\n", CAfile);
            goto end;
        }
    } else
        X509_LOOKUP_load_file(lookup, NULL, X509_FILETYPE_DEFAULT);

    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
    if (lookup == NULL)
        goto end;
    if (CApath) {
        if (!X509_LOOKUP_add_dir(lookup, CApath, X509_FILETYPE_PEM)) {
            BIO_printf(bio_err, "Error loading directory %s\n", CApath);
            goto end;
        }
    } else
        X509_LOOKUP_add_dir(lookup, NULL, X509_FILETYPE_DEFAULT);

    ERR_clear_error();
    return store;
 end:
    X509_STORE_free(store);
    return NULL;
}

#ifndef OPENSSL_NO_ENGINE
/* Try to load an engine in a shareable library */
static ENGINE *try_load_engine(const char *engine, int debug)
{
    ENGINE *e = ENGINE_by_id("dynamic");
    if (e) {
        if (!ENGINE_ctrl_cmd_string(e, "SO_PATH", engine, 0)
            || !ENGINE_ctrl_cmd_string(e, "LOAD", NULL, 0)) {
            ENGINE_free(e);
            e = NULL;
        }
    }
    return e;
}

ENGINE *setup_engine(const char *engine, int debug)
{
    ENGINE *e = NULL;

    if (engine) {
        if (strcmp(engine, "auto") == 0) {
            BIO_printf(bio_err, "enabling auto ENGINE support\n");
            ENGINE_register_all_complete();
            return NULL;
        }
        if ((e = ENGINE_by_id(engine)) == NULL
            && (e = try_load_engine(engine, debug)) == NULL) {
            BIO_printf(bio_err, "invalid engine \"%s\"\n", engine);
            ERR_print_errors(bio_err);
            return NULL;
        }
        if (debug) {
            ENGINE_ctrl(e, ENGINE_CTRL_SET_LOGSTREAM, 0, bio_err, 0);
        }
        ENGINE_ctrl_cmd(e, "SET_USER_INTERFACE", 0, ui_method, 0, 1);
        if (!ENGINE_set_default(e, ENGINE_METHOD_ALL)) {
            BIO_printf(bio_err, "can't use that engine\n");
            ERR_print_errors(bio_err);
            ENGINE_free(e);
            return NULL;
        }

        BIO_printf(bio_err, "engine \"%s\" set.\n", ENGINE_get_id(e));

        /* Free our "structural" reference. */
        ENGINE_free(e);
    }
    return e;
}
#endif

static unsigned long index_serial_hash(const OPENSSL_CSTRING *a)
{
    const char *n;

    n = a[DB_serial];
    while (*n == '0')
        n++;
    return (lh_strhash(n));
}

static int index_serial_cmp(const OPENSSL_CSTRING *a,
                            const OPENSSL_CSTRING *b)
{
    const char *aa, *bb;

    for (aa = a[DB_serial]; *aa == '0'; aa++) ;
    for (bb = b[DB_serial]; *bb == '0'; bb++) ;
    return (strcmp(aa, bb));
}

static int index_name_qual(char **a)
{
    return (a[0][0] == 'V');
}

static unsigned long index_name_hash(const OPENSSL_CSTRING *a)
{
    return (lh_strhash(a[DB_name]));
}

int index_name_cmp(const OPENSSL_CSTRING *a, const OPENSSL_CSTRING *b)
{
    return (strcmp(a[DB_name], b[DB_name]));
}

static IMPLEMENT_LHASH_HASH_FN(index_serial, OPENSSL_CSTRING)
static IMPLEMENT_LHASH_COMP_FN(index_serial, OPENSSL_CSTRING)
static IMPLEMENT_LHASH_HASH_FN(index_name, OPENSSL_CSTRING)
static IMPLEMENT_LHASH_COMP_FN(index_name, OPENSSL_CSTRING)
#undef BSIZE
#define BSIZE 256
BIGNUM *load_serial(char *serialfile, int create, ASN1_INTEGER **retai)
{
    BIO *in = NULL;
    BIGNUM *ret = NULL;
    char buf[1024];
    ASN1_INTEGER *ai = NULL;

    ai = ASN1_INTEGER_new();
    if (ai == NULL)
        goto err;

    in = BIO_new_file(serialfile, "r");
    if (in == NULL) {
        if (!create) {
            perror(serialfile);
            goto err;
        }
        ERR_clear_error();
        ret = BN_new();
        if (ret == NULL || !rand_serial(ret, ai))
            BIO_printf(bio_err, "Out of memory\n");
    } else {
        if (!a2i_ASN1_INTEGER(in, ai, buf, 1024)) {
            BIO_printf(bio_err, "unable to load number from %s\n",
                       serialfile);
            goto err;
        }
        ret = ASN1_INTEGER_to_BN(ai, NULL);
        if (ret == NULL) {
            BIO_printf(bio_err,
                       "error converting number from bin to BIGNUM\n");
            goto err;
        }
    }

    if (ret && retai) {
        *retai = ai;
        ai = NULL;
    }
 err:
    BIO_free(in);
    ASN1_INTEGER_free(ai);
    return (ret);
}

int save_serial(char *serialfile, char *suffix, BIGNUM *serial,
                ASN1_INTEGER **retai)
{
    char buf[1][BSIZE];
    BIO *out = NULL;
    int ret = 0;
    ASN1_INTEGER *ai = NULL;
    int j;

    if (suffix == NULL)
        j = strlen(serialfile);
    else
        j = strlen(serialfile) + strlen(suffix) + 1;
    if (j >= BSIZE) {
        BIO_printf(bio_err, "file name too long\n");
        goto err;
    }

    if (suffix == NULL)
        BUF_strlcpy(buf[0], serialfile, BSIZE);
    else {
#ifndef OPENSSL_SYS_VMS
        j = BIO_snprintf(buf[0], sizeof buf[0], "%s.%s", serialfile, suffix);
#else
        j = BIO_snprintf(buf[0], sizeof buf[0], "%s-%s", serialfile, suffix);
#endif
    }
#ifdef RL_DEBUG
    BIO_printf(bio_err, "DEBUG: writing \"%s\"\n", buf[0]);
#endif
    out = BIO_new_file(buf[0], "w");
    if (out == NULL) {
        ERR_print_errors(bio_err);
        goto err;
    }

    if ((ai = BN_to_ASN1_INTEGER(serial, NULL)) == NULL) {
        BIO_printf(bio_err, "error converting serial to ASN.1 format\n");
        goto err;
    }
    i2a_ASN1_INTEGER(out, ai);
    BIO_puts(out, "\n");
    ret = 1;
    if (retai) {
        *retai = ai;
        ai = NULL;
    }
 err:
    BIO_free_all(out);
    ASN1_INTEGER_free(ai);
    return (ret);
}

int rotate_serial(char *serialfile, char *new_suffix, char *old_suffix)
{
    char buf[5][BSIZE];
    int i, j;

    i = strlen(serialfile) + strlen(old_suffix);
    j = strlen(serialfile) + strlen(new_suffix);
    if (i > j)
        j = i;
    if (j + 1 >= BSIZE) {
        BIO_printf(bio_err, "file name too long\n");
        goto err;
    }
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[0], sizeof buf[0], "%s.%s", serialfile, new_suffix);
#else
    j = BIO_snprintf(buf[0], sizeof buf[0], "%s-%s", serialfile, new_suffix);
#endif
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[1], sizeof buf[1], "%s.%s", serialfile, old_suffix);
#else
    j = BIO_snprintf(buf[1], sizeof buf[1], "%s-%s", serialfile, old_suffix);
#endif
#ifdef RL_DEBUG
    BIO_printf(bio_err, "DEBUG: renaming \"%s\" to \"%s\"\n",
               serialfile, buf[1]);
#endif
    if (rename(serialfile, buf[1]) < 0 && errno != ENOENT
#ifdef ENOTDIR
        && errno != ENOTDIR
#endif
        ) {
        BIO_printf(bio_err,
                   "unable to rename %s to %s\n", serialfile, buf[1]);
        perror("reason");
        goto err;
    }
#ifdef RL_DEBUG
    BIO_printf(bio_err, "DEBUG: renaming \"%s\" to \"%s\"\n",
               buf[0], serialfile);
#endif
    if (rename(buf[0], serialfile) < 0) {
        BIO_printf(bio_err,
                   "unable to rename %s to %s\n", buf[0], serialfile);
        perror("reason");
        rename(buf[1], serialfile);
        goto err;
    }
    return 1;
 err:
    return 0;
}

int rand_serial(BIGNUM *b, ASN1_INTEGER *ai)
{
    BIGNUM *btmp;
    int ret = 0;

    if (b)
        btmp = b;
    else
        btmp = BN_new();

    if (!btmp)
        return 0;

    if (!BN_pseudo_rand(btmp, SERIAL_RAND_BITS, 0, 0))
        goto error;
    if (ai && !BN_to_ASN1_INTEGER(btmp, ai))
        goto error;

    ret = 1;

 error:

    if (btmp != b)
        BN_free(btmp);

    return ret;
}

CA_DB *load_index(char *dbfile, DB_ATTR *db_attr)
{
    CA_DB *retdb = NULL;
    TXT_DB *tmpdb = NULL;
    BIO *in;
    CONF *dbattr_conf = NULL;
    char buf[BSIZE];

    in = BIO_new_file(dbfile, "r");
    if (in == NULL) {
        ERR_print_errors(bio_err);
        goto err;
    }
    if ((tmpdb = TXT_DB_read(in, DB_NUMBER)) == NULL)
        goto err;

#ifndef OPENSSL_SYS_VMS
    BIO_snprintf(buf, sizeof buf, "%s.attr", dbfile);
#else
    BIO_snprintf(buf, sizeof buf, "%s-attr", dbfile);
#endif
    dbattr_conf = app_load_config(buf);

    retdb = app_malloc(sizeof(*retdb), "new DB");
    retdb->db = tmpdb;
    tmpdb = NULL;
    if (db_attr)
        retdb->attributes = *db_attr;
    else {
        retdb->attributes.unique_subject = 1;
    }

    if (dbattr_conf) {
        char *p = NCONF_get_string(dbattr_conf, NULL, "unique_subject");
        if (p) {
#ifdef RL_DEBUG
            BIO_printf(bio_err,
                       "DEBUG[load_index]: unique_subject = \"%s\"\n", p);
#endif
            retdb->attributes.unique_subject = parse_yesno(p, 1);
        }
    }

 err:
    NCONF_free(dbattr_conf);
    TXT_DB_free(tmpdb);
    BIO_free_all(in);
    return retdb;
}

int index_index(CA_DB *db)
{
    if (!TXT_DB_create_index(db->db, DB_serial, NULL,
                             LHASH_HASH_FN(index_serial),
                             LHASH_COMP_FN(index_serial))) {
        BIO_printf(bio_err,
                   "error creating serial number index:(%ld,%ld,%ld)\n",
                   db->db->error, db->db->arg1, db->db->arg2);
        return 0;
    }

    if (db->attributes.unique_subject
        && !TXT_DB_create_index(db->db, DB_name, index_name_qual,
                                LHASH_HASH_FN(index_name),
                                LHASH_COMP_FN(index_name))) {
        BIO_printf(bio_err, "error creating name index:(%ld,%ld,%ld)\n",
                   db->db->error, db->db->arg1, db->db->arg2);
        return 0;
    }
    return 1;
}

int save_index(const char *dbfile, const char *suffix, CA_DB *db)
{
    char buf[3][BSIZE];
    BIO *out;
    int j;

    j = strlen(dbfile) + strlen(suffix);
    if (j + 6 >= BSIZE) {
        BIO_printf(bio_err, "file name too long\n");
        goto err;
    }
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[2], sizeof buf[2], "%s.attr", dbfile);
#else
    j = BIO_snprintf(buf[2], sizeof buf[2], "%s-attr", dbfile);
#endif
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[1], sizeof buf[1], "%s.attr.%s", dbfile, suffix);
#else
    j = BIO_snprintf(buf[1], sizeof buf[1], "%s-attr-%s", dbfile, suffix);
#endif
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[0], sizeof buf[0], "%s.%s", dbfile, suffix);
#else
    j = BIO_snprintf(buf[0], sizeof buf[0], "%s-%s", dbfile, suffix);
#endif
#ifdef RL_DEBUG
    BIO_printf(bio_err, "DEBUG: writing \"%s\"\n", buf[0]);
#endif
    out = BIO_new_file(buf[0], "w");
    if (out == NULL) {
        perror(dbfile);
        BIO_printf(bio_err, "unable to open '%s'\n", dbfile);
        goto err;
    }
    j = TXT_DB_write(out, db->db);
    BIO_free(out);
    if (j <= 0)
        goto err;

    out = BIO_new_file(buf[1], "w");
#ifdef RL_DEBUG
    BIO_printf(bio_err, "DEBUG: writing \"%s\"\n", buf[1]);
#endif
    if (out == NULL) {
        perror(buf[2]);
        BIO_printf(bio_err, "unable to open '%s'\n", buf[2]);
        goto err;
    }
    BIO_printf(out, "unique_subject = %s\n",
               db->attributes.unique_subject ? "yes" : "no");
    BIO_free(out);

    return 1;
 err:
    return 0;
}

int rotate_index(const char *dbfile, const char *new_suffix,
                 const char *old_suffix)
{
    char buf[5][BSIZE];
    int i, j;

    i = strlen(dbfile) + strlen(old_suffix);
    j = strlen(dbfile) + strlen(new_suffix);
    if (i > j)
        j = i;
    if (j + 6 >= BSIZE) {
        BIO_printf(bio_err, "file name too long\n");
        goto err;
    }
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[4], sizeof buf[4], "%s.attr", dbfile);
#else
    j = BIO_snprintf(buf[4], sizeof buf[4], "%s-attr", dbfile);
#endif
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[2], sizeof buf[2], "%s.attr.%s", dbfile, new_suffix);
#else
    j = BIO_snprintf(buf[2], sizeof buf[2], "%s-attr-%s", dbfile, new_suffix);
#endif
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[0], sizeof buf[0], "%s.%s", dbfile, new_suffix);
#else
    j = BIO_snprintf(buf[0], sizeof buf[0], "%s-%s", dbfile, new_suffix);
#endif
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[1], sizeof buf[1], "%s.%s", dbfile, old_suffix);
#else
    j = BIO_snprintf(buf[1], sizeof buf[1], "%s-%s", dbfile, old_suffix);
#endif
#ifndef OPENSSL_SYS_VMS
    j = BIO_snprintf(buf[3], sizeof buf[3], "%s.attr.%s", dbfile, old_suffix);
#else
    j = BIO_snprintf(buf[3], sizeof buf[3], "%s-attr-%s", dbfile, old_suffix);
#endif
#ifdef RL_DEBUG
    BIO_printf(bio_err, "DEBUG: renaming \"%s\" to \"%s\"\n", dbfile, buf[1]);
#endif
    if (rename(dbfile, buf[1]) < 0 && errno != ENOENT
#ifdef ENOTDIR
        && errno != ENOTDIR
#endif
        ) {
        BIO_printf(bio_err, "unable to rename %s to %s\n", dbfile, buf[1]);
        perror("reason");
        goto err;
    }
#ifdef RL_DEBUG
    BIO_printf(bio_err, "DEBUG: renaming \"%s\" to \"%s\"\n", buf[0], dbfile);
#endif
    if (rename(buf[0], dbfile) < 0) {
        BIO_printf(bio_err, "unable to rename %s to %s\n", buf[0], dbfile);
        perror("reason");
        rename(buf[1], dbfile);
        goto err;
    }
#ifdef RL_DEBUG
    BIO_printf(bio_err, "DEBUG: renaming \"%s\" to \"%s\"\n", buf[4], buf[3]);
#endif
    if (rename(buf[4], buf[3]) < 0 && errno != ENOENT
#ifdef ENOTDIR
        && errno != ENOTDIR
#endif
        ) {
        BIO_printf(bio_err, "unable to rename %s to %s\n", buf[4], buf[3]);
        perror("reason");
        rename(dbfile, buf[0]);
        rename(buf[1], dbfile);
        goto err;
    }
#ifdef RL_DEBUG
    BIO_printf(bio_err, "DEBUG: renaming \"%s\" to \"%s\"\n", buf[2], buf[4]);
#endif
    if (rename(buf[2], buf[4]) < 0) {
        BIO_printf(bio_err, "unable to rename %s to %s\n", buf[2], buf[4]);
        perror("reason");
        rename(buf[3], buf[4]);
        rename(dbfile, buf[0]);
        rename(buf[1], dbfile);
        goto err;
    }
    return 1;
 err:
    return 0;
}

void free_index(CA_DB *db)
{
    if (db) {
        TXT_DB_free(db->db);
        OPENSSL_free(db);
    }
}

int parse_yesno(const char *str, int def)
{
    if (str) {
        switch (*str) {
        case 'f':              /* false */
        case 'F':              /* FALSE */
        case 'n':              /* no */
        case 'N':              /* NO */
        case '0':              /* 0 */
            return 0;
        case 't':              /* true */
        case 'T':              /* TRUE */
        case 'y':              /* yes */
        case 'Y':              /* YES */
        case '1':              /* 1 */
            return 1;
        }
    }
    return def;
}

/*
 * name is expected to be in the format /type0=value0/type1=value1/type2=...
 * where characters may be escaped by \
 */
X509_NAME *parse_name(const char *cp, long chtype, int canmulti)
{
    int nextismulti = 0;
    char *work;
    X509_NAME *n;

    if (*cp++ != '/')
        return NULL;

    n = X509_NAME_new();
    if (n == NULL)
        return NULL;
    work = OPENSSL_strdup(cp);
    if (work == NULL)
        goto err;

    while (*cp) {
        char *bp = work;
        char *typestr = bp;
        unsigned char *valstr;
        int nid;
        int ismulti = nextismulti;
        nextismulti = 0;

        /* Collect the type */
        while (*cp && *cp != '=')
            *bp++ = *cp++;
        if (*cp == '\0') {
            BIO_printf(bio_err,
                    "%s: Hit end of string before finding the equals.\n",
                    opt_getprog());
            goto err;
        }
        *bp++ = '\0';
        ++cp;

        /* Collect the value. */
        valstr = (unsigned char *)bp;
        for (; *cp && *cp != '/'; *bp++ = *cp++) {
            if (canmulti && *cp == '+') {
                nextismulti = 1;
                break;
            }
            if (*cp == '\\' && *++cp == '\0') {
                BIO_printf(bio_err,
                        "%s: escape character at end of string\n",
                        opt_getprog());
                goto err;
            }
        }
        *bp++ = '\0';

        /* If not at EOS (must be + or /), move forward. */
        if (*cp)
            ++cp;

        /* Parse */
        nid = OBJ_txt2nid(typestr);
        if (nid == NID_undef) {
            BIO_printf(bio_err, "%s: Skipping unknown attribute \"%s\"\n",
                      opt_getprog(), typestr);
            continue;
        }
        if (!X509_NAME_add_entry_by_NID(n, nid, chtype,
                                        valstr, strlen((char *)valstr),
                                        -1, ismulti ? -1 : 0))
            goto err;
    }

    OPENSSL_free(work);
    return n;

 err:
    X509_NAME_free(n);
    OPENSSL_free(work);
    return NULL;
}

/*
 * Read whole contents of a BIO into an allocated memory buffer and return
 * it.
 */

int bio_to_mem(unsigned char **out, int maxlen, BIO *in)
{
    BIO *mem;
    int len, ret;
    unsigned char tbuf[1024];
    mem = BIO_new(BIO_s_mem());
    if (!mem)
        return -1;
    for (;;) {
        if ((maxlen != -1) && maxlen < 1024)
            len = maxlen;
        else
            len = 1024;
        len = BIO_read(in, tbuf, len);
        if (len <= 0)
            break;
        if (BIO_write(mem, tbuf, len) != len) {
            BIO_free(mem);
            return -1;
        }
        maxlen -= len;

        if (maxlen == 0)
            break;
    }
    ret = BIO_get_mem_data(mem, (char **)out);
    BIO_set_flags(mem, BIO_FLAGS_MEM_RDONLY);
    BIO_free(mem);
    return ret;
}

int pkey_ctrl_string(EVP_PKEY_CTX *ctx, char *value)
{
    int rv;
    char *stmp, *vtmp = NULL;
    stmp = BUF_strdup(value);
    if (!stmp)
        return -1;
    vtmp = strchr(stmp, ':');
    if (vtmp) {
        *vtmp = 0;
        vtmp++;
    }
    rv = EVP_PKEY_CTX_ctrl_str(ctx, stmp, vtmp);
    OPENSSL_free(stmp);
    return rv;
}

static void nodes_print(const char *name, STACK_OF(X509_POLICY_NODE) *nodes)
{
    X509_POLICY_NODE *node;
    int i;

    BIO_printf(bio_err, "%s Policies:", name);
    if (nodes) {
        BIO_puts(bio_err, "\n");
        for (i = 0; i < sk_X509_POLICY_NODE_num(nodes); i++) {
            node = sk_X509_POLICY_NODE_value(nodes, i);
            X509_POLICY_NODE_print(bio_err, node, 2);
        }
    } else
        BIO_puts(bio_err, " <empty>\n");
}

void policies_print(X509_STORE_CTX *ctx)
{
    X509_POLICY_TREE *tree;
    int explicit_policy;
    tree = X509_STORE_CTX_get0_policy_tree(ctx);
    explicit_policy = X509_STORE_CTX_get_explicit_policy(ctx);

    BIO_printf(bio_err, "Require explicit Policy: %s\n",
               explicit_policy ? "True" : "False");

    nodes_print("Authority", X509_policy_tree_get0_policies(tree));
    nodes_print("User", X509_policy_tree_get0_user_policies(tree));
}

#if !defined(OPENSSL_NO_JPAKE) && !defined(OPENSSL_NO_PSK)

static JPAKE_CTX *jpake_init(const char *us, const char *them,
                             const char *secret)
{
    BIGNUM *p = NULL;
    BIGNUM *g = NULL;
    BIGNUM *q = NULL;
    BIGNUM *bnsecret = BN_new();
    JPAKE_CTX *ctx;

    /* Use a safe prime for p (that we found earlier) */
    BN_hex2bn(&p,
              "F9E5B365665EA7A05A9C534502780FEE6F1AB5BD4F49947FD036DBD7E905269AF46EF28B0FC07487EE4F5D20FB3C0AF8E700F3A2FA3414970CBED44FEDFF80CE78D800F184BB82435D137AADA2C6C16523247930A63B85661D1FC817A51ACD96168E95898A1F83A79FFB529368AA7833ABD1B0C3AEDDB14D2E1A2F71D99F763F");
    g = BN_new();
    BN_set_word(g, 2);
    q = BN_new();
    BN_rshift1(q, p);

    BN_bin2bn((const unsigned char *)secret, strlen(secret), bnsecret);

    ctx = JPAKE_CTX_new(us, them, p, g, q, bnsecret);
    BN_free(bnsecret);
    BN_free(q);
    BN_free(g);
    BN_free(p);

    return ctx;
}

static void jpake_send_part(BIO *conn, const JPAKE_STEP_PART *p)
{
    BN_print(conn, p->gx);
    BIO_puts(conn, "\n");
    BN_print(conn, p->zkpx.gr);
    BIO_puts(conn, "\n");
    BN_print(conn, p->zkpx.b);
    BIO_puts(conn, "\n");
}

static void jpake_send_step1(BIO *bconn, JPAKE_CTX *ctx)
{
    JPAKE_STEP1 s1;

    JPAKE_STEP1_init(&s1);
    JPAKE_STEP1_generate(&s1, ctx);
    jpake_send_part(bconn, &s1.p1);
    jpake_send_part(bconn, &s1.p2);
    (void)BIO_flush(bconn);
    JPAKE_STEP1_release(&s1);
}

static void jpake_send_step2(BIO *bconn, JPAKE_CTX *ctx)
{
    JPAKE_STEP2 s2;

    JPAKE_STEP2_init(&s2);
    JPAKE_STEP2_generate(&s2, ctx);
    jpake_send_part(bconn, &s2);
    (void)BIO_flush(bconn);
    JPAKE_STEP2_release(&s2);
}

static void jpake_send_step3a(BIO *bconn, JPAKE_CTX *ctx)
{
    JPAKE_STEP3A s3a;

    JPAKE_STEP3A_init(&s3a);
    JPAKE_STEP3A_generate(&s3a, ctx);
    BIO_write(bconn, s3a.hhk, sizeof s3a.hhk);
    (void)BIO_flush(bconn);
    JPAKE_STEP3A_release(&s3a);
}

static void jpake_send_step3b(BIO *bconn, JPAKE_CTX *ctx)
{
    JPAKE_STEP3B s3b;

    JPAKE_STEP3B_init(&s3b);
    JPAKE_STEP3B_generate(&s3b, ctx);
    BIO_write(bconn, s3b.hk, sizeof s3b.hk);
    (void)BIO_flush(bconn);
    JPAKE_STEP3B_release(&s3b);
}

static void readbn(BIGNUM **bn, BIO *bconn)
{
    char buf[10240];
    int l;

    l = BIO_gets(bconn, buf, sizeof buf);
    assert(l > 0);
    assert(buf[l - 1] == '\n');
    buf[l - 1] = '\0';
    BN_hex2bn(bn, buf);
}

static void jpake_receive_part(JPAKE_STEP_PART *p, BIO *bconn)
{
    readbn(&p->gx, bconn);
    readbn(&p->zkpx.gr, bconn);
    readbn(&p->zkpx.b, bconn);
}

static void jpake_receive_step1(JPAKE_CTX *ctx, BIO *bconn)
{
    JPAKE_STEP1 s1;

    JPAKE_STEP1_init(&s1);
    jpake_receive_part(&s1.p1, bconn);
    jpake_receive_part(&s1.p2, bconn);
    if (!JPAKE_STEP1_process(ctx, &s1)) {
        ERR_print_errors(bio_err);
        exit(1);
    }
    JPAKE_STEP1_release(&s1);
}

static void jpake_receive_step2(JPAKE_CTX *ctx, BIO *bconn)
{
    JPAKE_STEP2 s2;

    JPAKE_STEP2_init(&s2);
    jpake_receive_part(&s2, bconn);
    if (!JPAKE_STEP2_process(ctx, &s2)) {
        ERR_print_errors(bio_err);
        exit(1);
    }
    JPAKE_STEP2_release(&s2);
}

static void jpake_receive_step3a(JPAKE_CTX *ctx, BIO *bconn)
{
    JPAKE_STEP3A s3a;
    int l;

    JPAKE_STEP3A_init(&s3a);
    l = BIO_read(bconn, s3a.hhk, sizeof s3a.hhk);
    assert(l == sizeof s3a.hhk);
    if (!JPAKE_STEP3A_process(ctx, &s3a)) {
        ERR_print_errors(bio_err);
        exit(1);
    }
    JPAKE_STEP3A_release(&s3a);
}

static void jpake_receive_step3b(JPAKE_CTX *ctx, BIO *bconn)
{
    JPAKE_STEP3B s3b;
    int l;

    JPAKE_STEP3B_init(&s3b);
    l = BIO_read(bconn, s3b.hk, sizeof s3b.hk);
    assert(l == sizeof s3b.hk);
    if (!JPAKE_STEP3B_process(ctx, &s3b)) {
        ERR_print_errors(bio_err);
        exit(1);
    }
    JPAKE_STEP3B_release(&s3b);
}

void jpake_client_auth(BIO *out, BIO *conn, const char *secret)
{
    JPAKE_CTX *ctx;
    BIO *bconn;

    BIO_puts(out, "Authenticating with JPAKE\n");

    ctx = jpake_init("client", "server", secret);

    bconn = BIO_new(BIO_f_buffer());
    BIO_push(bconn, conn);

    jpake_send_step1(bconn, ctx);
    jpake_receive_step1(ctx, bconn);
    jpake_send_step2(bconn, ctx);
    jpake_receive_step2(ctx, bconn);
    jpake_send_step3a(bconn, ctx);
    jpake_receive_step3b(ctx, bconn);

    BIO_puts(out, "JPAKE authentication succeeded, setting PSK\n");

    OPENSSL_free(psk_key);
    psk_key = BN_bn2hex(JPAKE_get_shared_key(ctx));

    BIO_pop(bconn);
    BIO_free(bconn);

    JPAKE_CTX_free(ctx);
}

void jpake_server_auth(BIO *out, BIO *conn, const char *secret)
{
    JPAKE_CTX *ctx;
    BIO *bconn;

    BIO_puts(out, "Authenticating with JPAKE\n");

    ctx = jpake_init("server", "client", secret);

    bconn = BIO_new(BIO_f_buffer());
    BIO_push(bconn, conn);

    jpake_receive_step1(ctx, bconn);
    jpake_send_step1(bconn, ctx);
    jpake_receive_step2(ctx, bconn);
    jpake_send_step2(bconn, ctx);
    jpake_receive_step3a(ctx, bconn);
    jpake_send_step3b(bconn, ctx);

    BIO_puts(out, "JPAKE authentication succeeded, setting PSK\n");

    OPENSSL_free(psk_key);
    psk_key = BN_bn2hex(JPAKE_get_shared_key(ctx));

    BIO_pop(bconn);
    BIO_free(bconn);

    JPAKE_CTX_free(ctx);
}

#endif

/*-
 * next_protos_parse parses a comma separated list of strings into a string
 * in a format suitable for passing to SSL_CTX_set_next_protos_advertised.
 *   outlen: (output) set to the length of the resulting buffer on success.
 *   err: (maybe NULL) on failure, an error message line is written to this BIO.
 *   in: a NUL termianted string like "abc,def,ghi"
 *
 *   returns: a malloced buffer or NULL on failure.
 */
unsigned char *next_protos_parse(unsigned short *outlen, const char *in)
{
    size_t len;
    unsigned char *out;
    size_t i, start = 0;

    len = strlen(in);
    if (len >= 65535)
        return NULL;

    out = app_malloc(strlen(in) + 1, "NPN buffer");
    for (i = 0; i <= len; ++i) {
        if (i == len || in[i] == ',') {
            if (i - start > 255) {
                OPENSSL_free(out);
                return NULL;
            }
            out[start] = i - start;
            start = i + 1;
        } else
            out[i + 1] = in[i];
    }

    *outlen = len + 1;
    return out;
}

void print_cert_checks(BIO *bio, X509 *x,
                       const char *checkhost,
                       const char *checkemail, const char *checkip)
{
    if (x == NULL)
        return;
    if (checkhost) {
        BIO_printf(bio, "Hostname %s does%s match certificate\n",
                   checkhost,
                   X509_check_host(x, checkhost, 0, 0, NULL) == 1
                       ? "" : " NOT");
    }

    if (checkemail) {
        BIO_printf(bio, "Email %s does%s match certificate\n",
                   checkemail, X509_check_email(x, checkemail, 0, 0)
                   ? "" : " NOT");
    }

    if (checkip) {
        BIO_printf(bio, "IP %s does%s match certificate\n",
                   checkip, X509_check_ip_asc(x, checkip, 0) ? "" : " NOT");
    }
}

/* Get first http URL from a DIST_POINT structure */

static const char *get_dp_url(DIST_POINT *dp)
{
    GENERAL_NAMES *gens;
    GENERAL_NAME *gen;
    int i, gtype;
    ASN1_STRING *uri;
    if (!dp->distpoint || dp->distpoint->type != 0)
        return NULL;
    gens = dp->distpoint->name.fullname;
    for (i = 0; i < sk_GENERAL_NAME_num(gens); i++) {
        gen = sk_GENERAL_NAME_value(gens, i);
        uri = GENERAL_NAME_get0_value(gen, &gtype);
        if (gtype == GEN_URI && ASN1_STRING_length(uri) > 6) {
            char *uptr = (char *)ASN1_STRING_data(uri);
            if (strncmp(uptr, "http://", 7) == 0)
                return uptr;
        }
    }
    return NULL;
}

/*
 * Look through a CRLDP structure and attempt to find an http URL to
 * downloads a CRL from.
 */

static X509_CRL *load_crl_crldp(STACK_OF(DIST_POINT) *crldp)
{
    int i;
    const char *urlptr = NULL;
    for (i = 0; i < sk_DIST_POINT_num(crldp); i++) {
        DIST_POINT *dp = sk_DIST_POINT_value(crldp, i);
        urlptr = get_dp_url(dp);
        if (urlptr)
            return load_crl(urlptr, FORMAT_HTTP);
    }
    return NULL;
}

/*
 * Example of downloading CRLs from CRLDP: not usable for real world as it
 * always downloads, doesn't support non-blocking I/O and doesn't cache
 * anything.
 */

static STACK_OF(X509_CRL) *crls_http_cb(X509_STORE_CTX *ctx, X509_NAME *nm)
{
    X509 *x;
    STACK_OF(X509_CRL) *crls = NULL;
    X509_CRL *crl;
    STACK_OF(DIST_POINT) *crldp;

    crls = sk_X509_CRL_new_null();
    if (!crls)
        return NULL;
    x = X509_STORE_CTX_get_current_cert(ctx);
    crldp = X509_get_ext_d2i(x, NID_crl_distribution_points, NULL, NULL);
    crl = load_crl_crldp(crldp);
    sk_DIST_POINT_pop_free(crldp, DIST_POINT_free);
    if (!crl)
        return NULL;
    sk_X509_CRL_push(crls, crl);
    /* Try to download delta CRL */
    crldp = X509_get_ext_d2i(x, NID_freshest_crl, NULL, NULL);
    crl = load_crl_crldp(crldp);
    sk_DIST_POINT_pop_free(crldp, DIST_POINT_free);
    if (crl)
        sk_X509_CRL_push(crls, crl);
    return crls;
}

void store_setup_crl_download(X509_STORE *st)
{
    X509_STORE_set_lookup_crls_cb(st, crls_http_cb);
}

/*
 * Platform-specific sections
 */
#if defined(_WIN32)
# ifdef fileno
#  undef fileno
#  define fileno(a) (int)_fileno(a)
# endif

# include <windows.h>
# include <tchar.h>

static int WIN32_rename(const char *from, const char *to)
{
    TCHAR *tfrom = NULL, *tto;
    DWORD err;
    int ret = 0;

    if (sizeof(TCHAR) == 1) {
        tfrom = (TCHAR *)from;
        tto = (TCHAR *)to;
    } else {                    /* UNICODE path */

        size_t i, flen = strlen(from) + 1, tlen = strlen(to) + 1;
        tfrom = malloc(sizeof(*tfrom) * (flen + tlen));
        if (tfrom == NULL)
            goto err;
        tto = tfrom + flen;
# if !defined(_WIN32_WCE) || _WIN32_WCE>=101
        if (!MultiByteToWideChar(CP_ACP, 0, from, flen, (WCHAR *)tfrom, flen))
# endif
            for (i = 0; i < flen; i++)
                tfrom[i] = (TCHAR)from[i];
# if !defined(_WIN32_WCE) || _WIN32_WCE>=101
        if (!MultiByteToWideChar(CP_ACP, 0, to, tlen, (WCHAR *)tto, tlen))
# endif
            for (i = 0; i < tlen; i++)
                tto[i] = (TCHAR)to[i];
    }

    if (MoveFile(tfrom, tto))
        goto ok;
    err = GetLastError();
    if (err == ERROR_ALREADY_EXISTS || err == ERROR_FILE_EXISTS) {
        if (DeleteFile(tto) && MoveFile(tfrom, tto))
            goto ok;
        err = GetLastError();
    }
    if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
        errno = ENOENT;
    else if (err == ERROR_ACCESS_DENIED)
        errno = EACCES;
    else
        errno = EINVAL;         /* we could map more codes... */
 err:
    ret = -1;
 ok:
    if (tfrom != NULL && tfrom != (TCHAR *)from)
        free(tfrom);
    return ret;
}
#endif

/* app_tminterval section */
#if defined(_WIN32)
double app_tminterval(int stop, int usertime)
{
    FILETIME now;
    double ret = 0;
    static ULARGE_INTEGER tmstart;
    static int warning = 1;
# ifdef _WIN32_WINNT
    static HANDLE proc = NULL;

    if (proc == NULL) {
        if (check_winnt())
            proc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,
                               GetCurrentProcessId());
        if (proc == NULL)
            proc = (HANDLE) - 1;
    }

    if (usertime && proc != (HANDLE) - 1) {
        FILETIME junk;
        GetProcessTimes(proc, &junk, &junk, &junk, &now);
    } else
# endif
    {
        SYSTEMTIME systime;

        if (usertime && warning) {
            BIO_printf(bio_err, "To get meaningful results, run "
                       "this program on idle system.\n");
            warning = 0;
        }
        GetSystemTime(&systime);
        SystemTimeToFileTime(&systime, &now);
    }

    if (stop == TM_START) {
        tmstart.u.LowPart = now.dwLowDateTime;
        tmstart.u.HighPart = now.dwHighDateTime;
    } else {
        ULARGE_INTEGER tmstop;

        tmstop.u.LowPart = now.dwLowDateTime;
        tmstop.u.HighPart = now.dwHighDateTime;

        ret = (__int64)(tmstop.QuadPart - tmstart.QuadPart) * 1e-7;
    }

    return (ret);
}
#elif defined(OPENSSL_SYS_NETWARE)
# include <time.h>

double app_tminterval(int stop, int usertime)
{
    static clock_t tmstart;
    static int warning = 1;
    double ret = 0;

    if (usertime && warning) {
        BIO_printf(bio_err, "To get meaningful results, run "
                   "this program on idle system.\n");
        warning = 0;
    }

    if (stop == TM_START)
        tmstart = clock();
    else
        ret = (clock() - tmstart) / (double)CLOCKS_PER_SEC;

    return (ret);
}


#elif defined(OPENSSL_SYSTEM_VXWORKS)
# include <time.h>

double app_tminterval(int stop, int usertime)
{
    double ret = 0;
# ifdef CLOCK_REALTIME
    static struct timespec tmstart;
    struct timespec now;
# else
    static unsigned long tmstart;
    unsigned long now;
# endif
    static int warning = 1;

    if (usertime && warning) {
        BIO_printf(bio_err, "To get meaningful results, run "
                   "this program on idle system.\n");
        warning = 0;
    }
# ifdef CLOCK_REALTIME
    clock_gettime(CLOCK_REALTIME, &now);
    if (stop == TM_START)
        tmstart = now;
    else
        ret = ((now.tv_sec + now.tv_nsec * 1e-9)
               - (tmstart.tv_sec + tmstart.tv_nsec * 1e-9));
# else
    now = tickGet();
    if (stop == TM_START)
        tmstart = now;
    else
        ret = (now - tmstart) / (double)sysClkRateGet();
# endif
    return (ret);
}

#elif defined(OPENSSL_SYSTEM_VMS)
# include <time.h>
# include <times.h>

double app_tminterval(int stop, int usertime)
{
    static clock_t tmstart;
    double ret = 0;
    clock_t now;
# ifdef __TMS
    struct tms rus;

    now = times(&rus);
    if (usertime)
        now = rus.tms_utime;
# else
    if (usertime)
        now = clock();          /* sum of user and kernel times */
    else {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        now = (clock_t)((unsigned long long)tv.tv_sec * CLK_TCK +
                        (unsigned long long)tv.tv_usec * (1000000 / CLK_TCK)
            );
    }
# endif
    if (stop == TM_START)
        tmstart = now;
    else
        ret = (now - tmstart) / (double)(CLK_TCK);

    return (ret);
}

#elif defined(_SC_CLK_TCK)      /* by means of unistd.h */
# include <sys/times.h>

double app_tminterval(int stop, int usertime)
{
    double ret = 0;
    struct tms rus;
    clock_t now = times(&rus);
    static clock_t tmstart;

    if (usertime)
        now = rus.tms_utime;

    if (stop == TM_START)
        tmstart = now;
    else {
        long int tck = sysconf(_SC_CLK_TCK);
        ret = (now - tmstart) / (double)tck;
    }

    return (ret);
}

#else
# include <sys/time.h>
# include <sys/resource.h>

double app_tminterval(int stop, int usertime)
{
    double ret = 0;
    struct rusage rus;
    struct timeval now;
    static struct timeval tmstart;

    if (usertime)
        getrusage(RUSAGE_SELF, &rus), now = rus.ru_utime;
    else
        gettimeofday(&now, NULL);

    if (stop == TM_START)
        tmstart = now;
    else
        ret = ((now.tv_sec + now.tv_usec * 1e-6)
               - (tmstart.tv_sec + tmstart.tv_usec * 1e-6));

    return ret;
}
#endif

int app_access(const char* name, int flag)
{
#ifdef _WIN32
    return _access(name, flag);
#else
    return access(name, flag);
#endif
}

int app_hex(char c)
{
    switch (c) {
    default:
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
          return 4;
    case '5':
          return 5;
    case '6':
          return 6;
    case '7':
          return 7;
    case '8':
          return 8;
    case '9':
          return 9;
    case 'a': case 'A':
          return 0x0A;
    case 'b': case 'B':
          return 0x0B;
    case 'c': case 'C':
          return 0x0C;
    case 'd': case 'D':
          return 0x0D;
    case 'e': case 'E':
          return 0x0E;
    case 'f': case 'F':
          return 0x0F;
    }
}

/* app_isdir section */
#ifdef _WIN32
int app_isdir(const char *name)
{
    HANDLE hList;
    WIN32_FIND_DATA FileData;
# if defined(UNICODE) || defined(_UNICODE)
    size_t i, len_0 = strlen(name) + 1;

    if (len_0 > OSSL_NELEM(FileData.cFileName))
        return -1;

#  if !defined(_WIN32_WCE) || _WIN32_WCE>=101
    if (!MultiByteToWideChar
        (CP_ACP, 0, name, len_0, FileData.cFileName, len_0))
#  endif
        for (i = 0; i < len_0; i++)
            FileData.cFileName[i] = (WCHAR)name[i];

    hList = FindFirstFile(FileData.cFileName, &FileData);
# else
    hList = FindFirstFile(name, &FileData);
# endif
    if (hList == INVALID_HANDLE_VALUE)
        return -1;
    FindClose(hList);
    return ((FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}
#else
# include <sys/stat.h>
# ifndef S_ISDIR
#  if defined(_S_IFMT) && defined(_S_IFDIR)
#   define S_ISDIR(a)   (((a) & _S_IFMT) == _S_IFDIR)
#  else
#   define S_ISDIR(a)   (((a) & S_IFMT) == S_IFDIR)
#  endif
# endif

int app_isdir(const char *name)
{
# if defined(S_ISDIR)
    struct stat st;

    if (stat(name, &st) == 0)
        return S_ISDIR(st.st_mode);
    else
        return -1;
# else
    return -1;
# endif
}
#endif

/* raw_read|write section */
#if defined(_WIN32) && defined(STD_INPUT_HANDLE)
int raw_read_stdin(void *buf, int siz)
{
    DWORD n;
    if (ReadFile(GetStdHandle(STD_INPUT_HANDLE), buf, siz, &n, NULL))
        return (n);
    else
        return (-1);
}
#else
int raw_read_stdin(void *buf, int siz)
{
    return read(fileno(stdin), buf, siz);
}
#endif

#if defined(_WIN32) && defined(STD_OUTPUT_HANDLE)
int raw_write_stdout(const void *buf, int siz)
{
    DWORD n;
    if (WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, siz, &n, NULL))
        return (n);
    else
        return (-1);
}
#else
int raw_write_stdout(const void *buf, int siz)
{
    return write(fileno(stdout), buf, siz);
}
#endif
