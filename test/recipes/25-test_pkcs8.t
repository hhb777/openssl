#! /usr/bin/env perl
# Copyright 2022 The OpenSSL Project Authors. All Rights Reserved.
#
# Licensed under the Apache License 2.0 (the "License").  You may not use
# this file except in compliance with the License.  You can obtain a copy
# in the file LICENSE in the source distribution or at
# https://www.openssl.org/source/license.html

use strict;
use warnings;

use OpenSSL::Test::Utils;
use File::Compare qw(compare_text);
use OpenSSL::Test qw/:DEFAULT srctop_file ok_nofips is_nofips/;

setup("test_pkcs8");

plan tests => 9;

ok(run(app(([ 'openssl', 'pkcs8', '-topk8',
              '-in', srctop_file('test', 'certs', 'pc5-key.pem'),
              '-out', 'pbkdf2_default_saltlen.pem',
              '-passout', 'pass:password']))),
   "Convert a private key to PKCS5 v2.0 format using PBKDF2 with the default saltlen");

# We expect the output to be of the form "0:d=0  hl=2 l=  16 prim: OCTET STRING      [HEX DUMP]:FAC7F37508E6B7A805BF4B13861B3687"
# i.e. 2 byte header + 16 byte salt.
ok(run(app(([ 'openssl', 'asn1parse',
              '-in', 'pbkdf2_default_saltlen.pem',
              '-offset', '34', '-length', '18']))),
   "Check the default size of the PBKDF2 PARAM 'salt length' is 16");

SKIP: {
    skip "scrypt is not supported by this OpenSSL build", 2
        if disabled("scrypt");

    ok(run(app(([ 'openssl', 'pkcs8', '-topk8',
                  '-in', srctop_file('test', 'certs', 'pc5-key.pem'),
                  '-scrypt',
                  '-out', 'scrypt_default_saltlen.pem',
                  '-passout', 'pass:password']))),
       "Convert a private key to PKCS5 v2.0 format using scrypt with the default saltlen");

# We expect the output to be of the form "0:d=0  hl=2 l=  8 prim: OCTET STRING      [HEX DUMP]:FAC7F37508E6B7A805BF4B13861B3687"
# i.e. 2 byte header + 16 byte salt.
    ok(run(app(([ 'openssl', 'asn1parse',
                  '-in', 'scrypt_default_saltlen.pem',
                  '-offset', '34', '-length', '18']))),
       "Check the default size of the SCRYPT PARAM 'salt length' = 16");
}

SKIP: {
    skip "legacy provider is not supported by this OpenSSL build", 2
        if disabled('legacy') || disabled("des");

    ok(run(app(([ 'openssl', 'pkcs8', '-topk8',
                  '-in', srctop_file('test', 'certs', 'pc5-key.pem'),
                  '-v1', "PBE-MD5-DES",
                  '-provider', 'legacy',
                  '-provider', 'default',
                  '-out', 'pbe1.pem',
                  '-passout', 'pass:password']))),
       "Convert a private key to PKCS5 v1.5 format using pbeWithMD5AndDES-CBC with the default saltlen");

    ok(run(app(([ 'openssl', 'asn1parse',
                  '-in', 'pbe1.pem',
                  '-offset', '19', '-length', '10']))),
       "Check the default size of the PBE PARAM 'salt length' = 8");
};

SKIP: {
    skip "SM2, SM3 or SM4 is not supported by this OpenSSL build", 3
        if disabled("sm2") || disabled("sm3") || disabled("sm4");

    ok_nofips(run(app(([ 'openssl', 'pkcs8', '-topk8',
                      '-in', srctop_file('test', 'certs', 'sm2.key'),
                      '-out', 'sm2-pbes2-sm4-hmacWithSM3.key',
                      '-passout', 'pass:password',
                      '-v2', 'sm4', '-v2prf', 'hmacWithSM3']))),
                      "Convert a private key to PKCS#5 v2.0 format using SM4 and hmacWithSM3");

    ok_nofips(run(app(([ 'openssl', 'pkcs8', '-topk8',
                      '-in', 'sm2-pbes2-sm4-hmacWithSM3.key',
                      '-out', 'sm2.key',
                      '-passin', 'pass:password', '-nocrypt',
                      '-v2', 'sm4', '-v2prf', 'hmacWithSM3']))),
                      "Convert from PKCS#5 v2.0 format to PKCS#8 unencrypted format");

    is_nofips(compare_text(srctop_file('test', 'certs', 'sm2.key'), 'sm2.key',
        sub {
            my $in1 = $_[0];
            my $in2 = $_[1];
            $in1 =~ s/\r\n/\n/g;
            $in2 =~ s/\r\n/\n/g;
            $in1 ne $in2
        }), 0, "compare test/certs/sm2.key to sm2.key")
}
