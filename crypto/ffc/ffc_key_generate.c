/*
 * Copyright 2019-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "internal/ffc.h"

/*
 * SP800-56Ar3 5.6.1.1.4 Key pair generation by testing candidates.
 * Generates a private key in the interval [1, min(2 ^ N - 1, q - 1)].
 *
 * ctx must be set up with a libctx (for fips mode).
 * params contains the FFC domain parameters p, q and g (for DH or DSA).
 * N is the maximum bit length of the generated private key,
 * s is the security strength.
 * priv_key is the returned private key,
 */
int ffc_generate_private_key(BN_CTX *ctx, const FFC_PARAMS *params,
                             int N, int s, BIGNUM *priv)
{
#ifdef FIPS_MODE
    return ffc_generate_private_key_fips(ctx, params, N, s, priv);
#else
    do {
        if (!BN_priv_rand_range_ex(priv, params->q, ctx))
            return 0;
    } while (BN_is_zero(priv) || BN_is_one(priv));
    return 1;
#endif /* FIPS_MODE */
}

int ffc_generate_private_key_fips(BN_CTX *ctx, const FFC_PARAMS *params,
                                  int N, int s, BIGNUM *priv)
{
    int ret = 0, qbits = BN_num_bits(params->q);
    BIGNUM *m, *two_powN = NULL;

    /* Step (2) : check range of N */
    if (N < 2 * s || N > qbits)
        return 0;

    /* Deal with the edge case where the value of N is not set */
    if (N == 0) {
        N = qbits;
        s = N / 2;
    }

    two_powN = BN_new();
    /* 2^N */
    if (two_powN == NULL || !BN_lshift(two_powN, BN_value_one(), N))
        goto err;

    /* Step (5) : M = min(2 ^ N, q) */
    m = (BN_cmp(two_powN, params->q) > 0) ? params->q : two_powN;

    do {
        /* Steps (3, 4 & 7) :  c + 1 = 1 + random[0..2^N - 1] */
        if (!BN_priv_rand_range_ex(priv, two_powN, ctx)
            || !BN_add_word(priv, 1))
            goto err;
        /* Step (6) : loop if c > M - 2 (i.e. c + 1 >= M) */
        if (BN_cmp(priv, m) < 0)
            break;
    } while (1);

    ret = 1;
err:
    BN_free(two_powN);
    return ret;
}
