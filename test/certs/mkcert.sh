#! /bin/bash
#
# Copyright (c) 2016 Viktor Dukhovni <openssl-users@dukhovni.org>.
# All rights reserved.
#
# Contributed to the OpenSSL project under the terms of the OpenSSL license
# included with the version of the OpenSSL software that includes this module.

# 100 years should be enough for now
#
DAYS=36525

stderr_onerror() {
    (
        err=$("$@" >&3 2>&1) || {
            printf "%s\n" "$err" >&2
            exit 1
        }
    ) 3>&1
}

key() {
    local key=$1; shift

    local alg=rsa
    if [ -n "$OPENSSL_KEYALG" ]; then
        alg=$OPENSSL_KEYALG
    fi

    local bits=2048
    if [ -n "$OPENSSL_KEYBITS" ]; then
        bits=$OPENSSL_KEYBITS
    fi

    if [ ! -f "${key}.pem" ]; then
        args=(-algorithm "$alg")
        case $alg in
        rsa) args=("${args[@]}" -pkeyopt rsa_keygen_bits:$bits );;
        ecdsa) args=("${args[@]}" -pkeyopt "ec_paramgen_curve:$bits")
               args=("${args[@]}" -pkeyopt ec_param_enc:named_curve);;
        *) printf "Unsupported key algorithm: %s\n" "$alg" >&2; return 1;;
        esac
        stderr_onerror \
            openssl genpkey "${args[@]}" -out "${key}.pem"
    fi
}

req() {
    local key=$1; shift
    local cn=$1; shift

    key "$key"
    local errs

    stderr_onerror \
        openssl req -new -sha256 -key "${key}.pem" \
            -config <(printf "[req]\n%s\n%s\n[dn]\nCN=%s\n" \
		      "prompt = no" "distinguished_name = dn" "${cn}")
}

req_nocn() {
    local key=$1; shift

    key "$key"
    stderr_onerror \
        openssl req -new -sha256 -subj / -key "${key}.pem" \
            -config <(printf "[req]\n%s\n[dn]\nCN_default =\n" \
		      "distinguished_name = dn")
}

cert() {
    local cert=$1; shift
    local exts=$1; shift

    stderr_onerror \
        openssl x509 -req -sha256 -out "${cert}.pem" \
            -extfile <(printf "%s\n" "$exts") "$@"
}

genroot() {
    local cn=$1; shift
    local key=$1; shift
    local cert=$1; shift
    local skid="subjectKeyIdentifier = hash"
    local akid="authorityKeyIdentifier = keyid"

    exts=$(printf "%s\n%s\n%s\n" "$skid" "$akid" "basicConstraints = CA:true")
    for eku in "$@"
    do
        exts=$(printf "%s\nextendedKeyUsage = %s\n" "$exts" "$eku")
    done
    csr=$(req "$key" "$cn") || return 1
    echo "$csr" |
       cert "$cert" "$exts" -signkey "${key}.pem" -set_serial 1 -days "${DAYS}"
}

genca() {
    local cn=$1; shift
    local key=$1; shift
    local cert=$1; shift
    local cakey=$1; shift
    local cacert=$1; shift
    local skid="subjectKeyIdentifier = hash"
    local akid="authorityKeyIdentifier = keyid"

    exts=$(printf "%s\n%s\n%s\n" "$skid" "$akid" "basicConstraints = CA:true")
    for eku in "$@"
    do
        exts=$(printf "%s\nextendedKeyUsage = %s\n" "$exts" "$eku")
    done
    csr=$(req "$key" "$cn") || return 1
    echo "$csr" |
        cert "$cert" "$exts" -CA "${cacert}.pem" -CAkey "${cakey}.pem" \
	    -set_serial 2 -days "${DAYS}"
}

genee() {
    local OPTIND=1
    local purpose=serverAuth

    while getopts p: o
    do
        case $o in
        p) purpose="$OPTARG";;
        *) echo "Usage: $0 genee [-p EKU] cn keyname certname cakeyname cacertname" >&2
           return 1;;
        esac
    done

    shift $((OPTIND - 1))
    local cn=$1; shift
    local key=$1; shift
    local cert=$1; shift
    local cakey=$1; shift
    local ca=$1; shift

    exts=$(printf "%s\n%s\n%s\n%s\n%s\n[alts]\n%s\n" \
	    "subjectKeyIdentifier = hash" \
	    "authorityKeyIdentifier = keyid, issuer" \
	    "basicConstraints = CA:false" \
	    "extendedKeyUsage = $purpose" \
	    "subjectAltName = @alts" "DNS=${cn}")
    csr=$(req "$key" "$cn") || return 1
    echo "$csr" |
	cert "$cert" "$exts" -CA "${ca}.pem" -CAkey "${cakey}.pem" \
	    -set_serial 2 -days "${DAYS}" "$@"
}

genss() {
    local cn=$1; shift
    local key=$1; shift
    local cert=$1; shift

    exts=$(printf "%s\n%s\n%s\n%s\n%s\n[alts]\n%s\n" \
	    "subjectKeyIdentifier   = hash" \
	    "authorityKeyIdentifier = keyid, issuer" \
	    "basicConstraints = CA:false" \
	    "extendedKeyUsage = serverAuth" \
	    "subjectAltName = @alts" "DNS=${cn}")
    csr=$(req "$key" "$cn") || return 1
    echo "$csr" |
        cert "$cert" "$exts" -signkey "${key}.pem" \
            -set_serial 1 -days "${DAYS}" "$@"
}

gennocn() {
    local key=$1; shift
    local cert=$1; shift

    csr=$(req_nocn "$key") || return 1
    echo "$csr" |
	cert "$cert" "" -signkey "${key}.pem" -set_serial 1 -days -1 "$@"
}

"$@"
