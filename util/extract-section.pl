#! /usr/bin/env perl
# Copyright 2004-2016 The OpenSSL Project Authors. All Rights Reserved.
#
# Licensed under the OpenSSL license (the "License").  You may not use
# this file except in compliance with the License.  You can obtain a copy
# in the file LICENSE in the source distribution or at
# https://www.openssl.org/source/license.html

while(<STDIN>) {
	if (/=for\s+comment\s+openssl_manual_section:(\S+)/)
		{
		print "$1\n";
		exit 0;
		}
}

print "$ARGV[0]\n";

