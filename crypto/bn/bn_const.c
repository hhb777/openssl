/* Insert boilerplate */

#include <openssl/bn.h>

#if BN_BITS2 == 64
#    define bn_pack2(a1,a2) ((0x##a1##ULL<<32)|0x##a2##ULL)
#elif BN_BITS2 == 32
#    define bn_pack2(a1,a2) 0x##a2##UL, 0x##a1##UL
#else
#    error "unsupported BN_BITS2"
#endif

/*-
 * "First Oakley Default Group" from RFC2409, section 6.1.
 *
 * The prime is: 2^768 - 2 ^704 - 1 + 2^64 * { [2^638 pi] + 149686 }
 *
 * RFC2409 specifies a generator of 2.
 * RFC2412 specifies a generator of of 22.
 */

static const BN_ULONG rfc2409_prime_768[] = {
    bn_pack2(FFFFFFFF, FFFFFFFF), bn_pack2(F44C42E9, A63A3620), bn_pack2(E485B576, 625E7EC6),
    bn_pack2(4FE1356D, 6D51C245), bn_pack2(302B0A6D, F25F1437), bn_pack2(EF9519B3, CD3A431B),
    bn_pack2(514A0879, 8E3404DD), bn_pack2(020BBEA6, 3B139B22), bn_pack2(29024E08, 8A67CC74),
    bn_pack2(C4C6628B, 80DC1CD1), bn_pack2(C90FDAA2, 2168C234), bn_pack2(FFFFFFFF, FFFFFFFF)
};

/*-
 * "Second Oakley Default Group" from RFC2409, section 6.2.
 *
 * The prime is: 2^1024 - 2^960 - 1 + 2^64 * { [2^894 pi] + 129093 }.
 *
 * RFC2409 specifies a generator of 2.
 * RFC2412 specifies a generator of 22.
 */

static const BN_ULONG rfc2409_prime_1024[] = {
    bn_pack2(FFFFFFFF, FFFFFFFF), bn_pack2(49286651, ECE65381), bn_pack2(AE9F2411, 7C4B1FE6),
    bn_pack2(EE386BFB, 5A899FA5), bn_pack2(0BFF5CB6, F406B7ED), bn_pack2(F44C42E9, A637ED6B),
    bn_pack2(E485B576, 625E7EC6), bn_pack2(4FE1356D, 6D51C245), bn_pack2(302B0A6D, F25F1437),
    bn_pack2(EF9519B3, CD3A431B), bn_pack2(514A0879, 8E3404DD), bn_pack2(020BBEA6, 3B139B22),
    bn_pack2(29024E08, 8A67CC74), bn_pack2(C4C6628B, 80DC1CD1), bn_pack2(C90FDAA2, 2168C234),
    bn_pack2(FFFFFFFF, FFFFFFFF)
};

/*-
 * "1536-bit MODP Group" from RFC3526, Section 2.
 *
 * The prime is: 2^1536 - 2^1472 - 1 + 2^64 * { [2^1406 pi] + 741804 }
 *
 * RFC3526 specifies a generator of 2.
 * RFC2312 specifies a generator of 22.
 */

static const BN_ULONG rfc3526_prime_1536[] = {
    bn_pack2(FFFFFFFF, FFFFFFFF), bn_pack2(F1746C08, CA237327), bn_pack2(670C354E, 4ABC9804),
    bn_pack2(9ED52907, 7096966D), bn_pack2(1C62F356, 208552BB), bn_pack2(83655D23, DCA3AD96),
    bn_pack2(69163FA8, FD24CF5F), bn_pack2(98DA4836, 1C55D39A), bn_pack2(C2007CB8, A163BF05),
    bn_pack2(49286651, ECE45B3D), bn_pack2(AE9F2411, 7C4B1FE6), bn_pack2(EE386BFB, 5A899FA5),
    bn_pack2(0BFF5CB6, F406B7ED), bn_pack2(F44C42E9, A637ED6B), bn_pack2(E485B576, 625E7EC6),
    bn_pack2(4FE1356D, 6D51C245), bn_pack2(302B0A6D, F25F1437), bn_pack2(EF9519B3, CD3A431B),
    bn_pack2(514A0879, 8E3404DD), bn_pack2(020BBEA6, 3B139B22), bn_pack2(29024E08, 8A67CC74),
    bn_pack2(C4C6628B, 80DC1CD1), bn_pack2(C90FDAA2, 2168C234), bn_pack2(FFFFFFFF, FFFFFFFF)
};

/*-
 * "2048-bit MODP Group" from RFC3526, Section 3.
 *
 * The prime is: 2^2048 - 2^1984 - 1 + 2^64 * { [2^1918 pi] + 124476 }
 *
 * RFC3526 specifies a generator of 2.
 */

static const BN_ULONG rfc3526_prime_2048[] = {
    bn_pack2(FFFFFFFF, FFFFFFFF), bn_pack2(15728E5A, 8AACAA68), bn_pack2(15D22618, 98FA0510),
    bn_pack2(3995497C, EA956AE5), bn_pack2(DE2BCBF6, 95581718), bn_pack2(B5C55DF0, 6F4C52C9),
    bn_pack2(9B2783A2, EC07A28F), bn_pack2(E39E772C, 180E8603), bn_pack2(32905E46, 2E36CE3B),
    bn_pack2(F1746C08, CA18217C), bn_pack2(670C354E, 4ABC9804), bn_pack2(9ED52907, 7096966D),
    bn_pack2(1C62F356, 208552BB), bn_pack2(83655D23, DCA3AD96), bn_pack2(69163FA8, FD24CF5F),
    bn_pack2(98DA4836, 1C55D39A), bn_pack2(C2007CB8, A163BF05), bn_pack2(49286651, ECE45B3D),
    bn_pack2(AE9F2411, 7C4B1FE6), bn_pack2(EE386BFB, 5A899FA5), bn_pack2(0BFF5CB6, F406B7ED),
    bn_pack2(F44C42E9, A637ED6B), bn_pack2(E485B576, 625E7EC6), bn_pack2(4FE1356D, 6D51C245),
    bn_pack2(302B0A6D, F25F1437), bn_pack2(EF9519B3, CD3A431B), bn_pack2(514A0879, 8E3404DD),
    bn_pack2(020BBEA6, 3B139B22), bn_pack2(29024E08, 8A67CC74), bn_pack2(C4C6628B, 80DC1CD1),
    bn_pack2(C90FDAA2, 2168C234), bn_pack2(FFFFFFFF, FFFFFFFF)
};

/*-
 * "3072-bit MODP Group" from RFC3526, Section 4.
 *
 * The prime is: 2^3072 - 2^3008 - 1 + 2^64 * { [2^2942 pi] + 1690314 }
 *
 * RFC3526 specifies a generator of 2.
 */
 
static const BN_ULONG rfc3526_prime_3072[] = {
    bn_pack2(FFFFFFFF, FFFFFFFF), bn_pack2(4B82D120, A93AD2CA), bn_pack2(43DB5BFC, E0FD108E),
    bn_pack2(08E24FA0, 74E5AB31), bn_pack2(770988C0, BAD946E2), bn_pack2(BBE11757, 7A615D6C),
    bn_pack2(521F2B18, 177B200C), bn_pack2(D8760273, 3EC86A64), bn_pack2(F12FFA06, D98A0864),
    bn_pack2(CEE3D226, 1AD2EE6B), bn_pack2(1E8C94E0, 4A25619D), bn_pack2(ABF5AE8C, DB0933D7),
    bn_pack2(B3970F85, A6E1E4C7), bn_pack2(8AEA7157, 5D060C7D), bn_pack2(ECFB8504, 58DBEF0A),
    bn_pack2(A85521AB, DF1CBA64), bn_pack2(AD33170D, 04507A33), bn_pack2(15728E5A, 8AAAC42D),
    bn_pack2(15D22618, 98FA0510), bn_pack2(3995497C, EA956AE5), bn_pack2(DE2BCBF6, 95581718),
    bn_pack2(B5C55DF0, 6F4C52C9), bn_pack2(9B2783A2, EC07A28F), bn_pack2(E39E772C, 180E8603),
    bn_pack2(32905E46, 2E36CE3B), bn_pack2(F1746C08, CA18217C), bn_pack2(670C354E, 4ABC9804),
    bn_pack2(9ED52907, 7096966D), bn_pack2(1C62F356, 208552BB), bn_pack2(83655D23, DCA3AD96),
    bn_pack2(69163FA8, FD24CF5F), bn_pack2(98DA4836, 1C55D39A), bn_pack2(C2007CB8, A163BF05),
    bn_pack2(49286651, ECE45B3D), bn_pack2(AE9F2411, 7C4B1FE6), bn_pack2(EE386BFB, 5A899FA5),
    bn_pack2(0BFF5CB6, F406B7ED), bn_pack2(F44C42E9, A637ED6B), bn_pack2(E485B576, 625E7EC6),
    bn_pack2(4FE1356D, 6D51C245), bn_pack2(302B0A6D, F25F1437), bn_pack2(EF9519B3, CD3A431B),
    bn_pack2(514A0879, 8E3404DD), bn_pack2(020BBEA6, 3B139B22), bn_pack2(29024E08, 8A67CC74),
    bn_pack2(C4C6628B, 80DC1CD1), bn_pack2(C90FDAA2, 2168C234), bn_pack2(FFFFFFFF, FFFFFFFF)
};

/*-
 * "4096-bit MODP Group" from RFC3526, Section 5.
 *
 * The prime is: 2^4096 - 2^4032 - 1 + 2^64 * { [2^3966 pi] + 240904 }
 *
 * RFC3526 specifies a generator of 2.
 */

static const BN_ULONG rfc3526_prime_4096[] = {
    bn_pack2(FFFFFFFF, FFFFFFFF), bn_pack2(4DF435C9, 34063199), bn_pack2(86FFB7DC, 90A6C08F),
    bn_pack2(93B4EA98, 8D8FDDC1), bn_pack2(D0069127, D5B05AA9), bn_pack2(B81BDD76, 2170481C),
    bn_pack2(1F612970, CEE2D7AF), bn_pack2(233BA186, 515BE7ED), bn_pack2(99B2964F, A090C3A2),
    bn_pack2(287C5947, 4E6BC05D), bn_pack2(2E8EFC14, 1FBECAA6), bn_pack2(DBBBC2DB, 04DE8EF9),
    bn_pack2(2583E9CA, 2AD44CE8), bn_pack2(1A946834, B6150BDA), bn_pack2(99C32718, 6AF4E23C),
    bn_pack2(88719A10, BDBA5B26), bn_pack2(1A723C12, A787E6D7), bn_pack2(4B82D120, A9210801),
    bn_pack2(43DB5BFC, E0FD108E), bn_pack2(08E24FA0, 74E5AB31), bn_pack2(770988C0, BAD946E2),
    bn_pack2(BBE11757, 7A615D6C), bn_pack2(521F2B18, 177B200C), bn_pack2(D8760273, 3EC86A64),
    bn_pack2(F12FFA06, D98A0864), bn_pack2(CEE3D226, 1AD2EE6B), bn_pack2(1E8C94E0, 4A25619D),
    bn_pack2(ABF5AE8C, DB0933D7), bn_pack2(B3970F85, A6E1E4C7), bn_pack2(8AEA7157, 5D060C7D),
    bn_pack2(ECFB8504, 58DBEF0A), bn_pack2(A85521AB, DF1CBA64), bn_pack2(AD33170D, 04507A33),
    bn_pack2(15728E5A, 8AAAC42D), bn_pack2(15D22618, 98FA0510), bn_pack2(3995497C, EA956AE5),
    bn_pack2(DE2BCBF6, 95581718), bn_pack2(B5C55DF0, 6F4C52C9), bn_pack2(9B2783A2, EC07A28F),
    bn_pack2(E39E772C, 180E8603), bn_pack2(32905E46, 2E36CE3B), bn_pack2(F1746C08, CA18217C),
    bn_pack2(670C354E, 4ABC9804), bn_pack2(9ED52907, 7096966D), bn_pack2(1C62F356, 208552BB),
    bn_pack2(83655D23, DCA3AD96), bn_pack2(69163FA8, FD24CF5F), bn_pack2(98DA4836, 1C55D39A),
    bn_pack2(C2007CB8, A163BF05), bn_pack2(49286651, ECE45B3D), bn_pack2(AE9F2411, 7C4B1FE6),
    bn_pack2(EE386BFB, 5A899FA5), bn_pack2(0BFF5CB6, F406B7ED), bn_pack2(F44C42E9, A637ED6B),
    bn_pack2(E485B576, 625E7EC6), bn_pack2(4FE1356D, 6D51C245), bn_pack2(302B0A6D, F25F1437),
    bn_pack2(EF9519B3, CD3A431B), bn_pack2(514A0879, 8E3404DD), bn_pack2(020BBEA6, 3B139B22),
    bn_pack2(29024E08, 8A67CC74), bn_pack2(C4C6628B, 80DC1CD1), bn_pack2(C90FDAA2, 2168C234),
    bn_pack2(FFFFFFFF, FFFFFFFF)
};

/*-
 * "6144-bit MODP Group" from RFC3526, Section 6.
 *
 * The prime is: 2^6144 - 2^6080 - 1 + 2^64 * { [2^6014 pi] + 929484 }
 *
 * RFC3526 specifies a generator of 2.
 */
 
static const BN_ULONG rfc3526_prime_6144[] = {
    bn_pack2(FFFFFFFF, FFFFFFFF), bn_pack2(E694F91E, 6DCC4024), bn_pack2(12BF2D5B, 0B7474D6),
    bn_pack2(043E8F66, 3F4860EE), bn_pack2(387FE8D7, 6E3C0468), bn_pack2(DA56C9EC, 2EF29632),
    bn_pack2(EB19CCB1, A313D55C), bn_pack2(F550AA3D, 8A1FBFF0), bn_pack2(06A1D58B, B7C5DA76),
    bn_pack2(A79715EE, F29BE328), bn_pack2(14CC5ED2, 0F8037E0), bn_pack2(CC8F6D7E, BF48E1D8),
    bn_pack2(4BD407B2, 2B4154AA), bn_pack2(0F1D45B7, FF585AC5), bn_pack2(23A97A7E, 36CC88BE),
    bn_pack2(59E7C97F, BEC7E8F3), bn_pack2(B5A84031, 900B1C9E), bn_pack2(D55E702F, 46980C82),
    bn_pack2(F482D7CE, 6E74FEF6), bn_pack2(F032EA15, D1721D03), bn_pack2(5983CA01, C64B92EC),
    bn_pack2(6FB8F401, 378CD2BF), bn_pack2(33205151, 2BD7AF42), bn_pack2(DB7F1447, E6CC254B),
    bn_pack2(44CE6CBA, CED4BB1B), bn_pack2(DA3EDBEB, CF9B14ED), bn_pack2(179727B0, 865A8918),
    bn_pack2(B06A53ED, 9027D831), bn_pack2(E5DB382F, 413001AE), bn_pack2(F8FF9406, AD9E530E),
    bn_pack2(C9751E76, 3DBA37BD), bn_pack2(C1D4DCB2, 602646DE), bn_pack2(36C3FAB4, D27C7026),
    bn_pack2(4DF435C9, 34028492), bn_pack2(86FFB7DC, 90A6C08F), bn_pack2(93B4EA98, 8D8FDDC1),
    bn_pack2(D0069127, D5B05AA9), bn_pack2(B81BDD76, 2170481C), bn_pack2(1F612970, CEE2D7AF),
    bn_pack2(233BA186, 515BE7ED), bn_pack2(99B2964F, A090C3A2), bn_pack2(287C5947, 4E6BC05D),
    bn_pack2(2E8EFC14, 1FBECAA6), bn_pack2(DBBBC2DB, 04DE8EF9), bn_pack2(2583E9CA, 2AD44CE8),
    bn_pack2(1A946834, B6150BDA), bn_pack2(99C32718, 6AF4E23C), bn_pack2(88719A10, BDBA5B26),
    bn_pack2(1A723C12, A787E6D7), bn_pack2(4B82D120, A9210801), bn_pack2(43DB5BFC, E0FD108E),
    bn_pack2(08E24FA0, 74E5AB31), bn_pack2(770988C0, BAD946E2), bn_pack2(BBE11757, 7A615D6C),
    bn_pack2(521F2B18, 177B200C), bn_pack2(D8760273, 3EC86A64), bn_pack2(F12FFA06, D98A0864),
    bn_pack2(CEE3D226, 1AD2EE6B), bn_pack2(1E8C94E0, 4A25619D), bn_pack2(ABF5AE8C, DB0933D7),
    bn_pack2(B3970F85, A6E1E4C7), bn_pack2(8AEA7157, 5D060C7D), bn_pack2(ECFB8504, 58DBEF0A),
    bn_pack2(A85521AB, DF1CBA64), bn_pack2(AD33170D, 04507A33), bn_pack2(15728E5A, 8AAAC42D),
    bn_pack2(15D22618, 98FA0510), bn_pack2(3995497C, EA956AE5), bn_pack2(DE2BCBF6, 95581718),
    bn_pack2(B5C55DF0, 6F4C52C9), bn_pack2(9B2783A2, EC07A28F), bn_pack2(E39E772C, 180E8603),
    bn_pack2(32905E46, 2E36CE3B), bn_pack2(F1746C08, CA18217C), bn_pack2(670C354E, 4ABC9804),
    bn_pack2(9ED52907, 7096966D), bn_pack2(1C62F356, 208552BB), bn_pack2(83655D23, DCA3AD96),
    bn_pack2(69163FA8, FD24CF5F), bn_pack2(98DA4836, 1C55D39A), bn_pack2(C2007CB8, A163BF05),
    bn_pack2(49286651, ECE45B3D), bn_pack2(AE9F2411, 7C4B1FE6), bn_pack2(EE386BFB, 5A899FA5),
    bn_pack2(0BFF5CB6, F406B7ED), bn_pack2(F44C42E9, A637ED6B), bn_pack2(E485B576, 625E7EC6),
    bn_pack2(4FE1356D, 6D51C245), bn_pack2(302B0A6D, F25F1437), bn_pack2(EF9519B3, CD3A431B),
    bn_pack2(514A0879, 8E3404DD), bn_pack2(020BBEA6, 3B139B22), bn_pack2(29024E08, 8A67CC74),
    bn_pack2(C4C6628B, 80DC1CD1), bn_pack2(C90FDAA2, 2168C234), bn_pack2(FFFFFFFF, FFFFFFFF)
};

/*-
 * "8192-bit MODP Group" from RFC3526, Section 7.
 *
 * The prime is: 2^8192 - 2^8128 - 1 + 2^64 * { [2^8062 pi] + 4743158 }
 *
 * RFC3526 specifies a generator of 2.
 */
 
static const BN_ULONG rfc3526_prime_8192[] = {
    bn_pack2(FFFFFFFF, FFFFFFFF), bn_pack2(60C980DD, 98EDD3DF), bn_pack2(C81F56E8, 80B96E71),
    bn_pack2(9E3050E2, 765694DF), bn_pack2(9558E447, 5677E9AA), bn_pack2(C9190DA6, FC026E47),
    bn_pack2(889A002E, D5EE382B), bn_pack2(4009438B, 481C6CD7), bn_pack2(359046F4, EB879F92),
    bn_pack2(FAF36BC3, 1ECFA268), bn_pack2(B1D510BD, 7EE74D73), bn_pack2(F9AB4819, 5DED7EA1),
    bn_pack2(64F31CC5, 0846851D), bn_pack2(4597E899, A0255DC1), bn_pack2(DF310EE0, 74AB6A36),
    bn_pack2(6D2A13F8, 3F44F82D), bn_pack2(062B3CF5, B3A278A6), bn_pack2(79683303, ED5BDD3A),
    bn_pack2(FA9D4B7F, A2C087E8), bn_pack2(4BCBC886, 2F8385DD), bn_pack2(3473FC64, 6CEA306B),
    bn_pack2(13EB57A8, 1A23F0C7), bn_pack2(22222E04, A4037C07), bn_pack2(E3FDB8BE, FC848AD9),
    bn_pack2(238F16CB, E39D652D), bn_pack2(3423B474, 2BF1C978), bn_pack2(3AAB639C, 5AE4F568),
    bn_pack2(2576F693, 6BA42466), bn_pack2(741FA7BF, 8AFC47ED), bn_pack2(3BC832B6, 8D9DD300),
    bn_pack2(D8BEC4D0, 73B931BA), bn_pack2(38777CB6, A932DF8C), bn_pack2(74A3926F, 12FEE5E4),
    bn_pack2(E694F91E, 6DBE1159), bn_pack2(12BF2D5B, 0B7474D6), bn_pack2(043E8F66, 3F4860EE),
    bn_pack2(387FE8D7, 6E3C0468), bn_pack2(DA56C9EC, 2EF29632), bn_pack2(EB19CCB1, A313D55C),
    bn_pack2(F550AA3D, 8A1FBFF0), bn_pack2(06A1D58B, B7C5DA76), bn_pack2(A79715EE, F29BE328),
    bn_pack2(14CC5ED2, 0F8037E0), bn_pack2(CC8F6D7E, BF48E1D8), bn_pack2(4BD407B2, 2B4154AA),
    bn_pack2(0F1D45B7, FF585AC5), bn_pack2(23A97A7E, 36CC88BE), bn_pack2(59E7C97F, BEC7E8F3),
    bn_pack2(B5A84031, 900B1C9E), bn_pack2(D55E702F, 46980C82), bn_pack2(F482D7CE, 6E74FEF6),
    bn_pack2(F032EA15, D1721D03), bn_pack2(5983CA01, C64B92EC), bn_pack2(6FB8F401, 378CD2BF),
    bn_pack2(33205151, 2BD7AF42), bn_pack2(DB7F1447, E6CC254B), bn_pack2(44CE6CBA, CED4BB1B),
    bn_pack2(DA3EDBEB, CF9B14ED), bn_pack2(179727B0, 865A8918), bn_pack2(B06A53ED, 9027D831),
    bn_pack2(E5DB382F, 413001AE), bn_pack2(F8FF9406, AD9E530E), bn_pack2(C9751E76, 3DBA37BD),
    bn_pack2(C1D4DCB2, 602646DE), bn_pack2(36C3FAB4, D27C7026), bn_pack2(4DF435C9, 34028492),
    bn_pack2(86FFB7DC, 90A6C08F), bn_pack2(93B4EA98, 8D8FDDC1), bn_pack2(D0069127, D5B05AA9),
    bn_pack2(B81BDD76, 2170481C), bn_pack2(1F612970, CEE2D7AF), bn_pack2(233BA186, 515BE7ED),
    bn_pack2(99B2964F, A090C3A2), bn_pack2(287C5947, 4E6BC05D), bn_pack2(2E8EFC14, 1FBECAA6),
    bn_pack2(DBBBC2DB, 04DE8EF9), bn_pack2(2583E9CA, 2AD44CE8), bn_pack2(1A946834, B6150BDA),
    bn_pack2(99C32718, 6AF4E23C), bn_pack2(88719A10, BDBA5B26), bn_pack2(1A723C12, A787E6D7),
    bn_pack2(4B82D120, A9210801), bn_pack2(43DB5BFC, E0FD108E), bn_pack2(08E24FA0, 74E5AB31),
    bn_pack2(770988C0, BAD946E2), bn_pack2(BBE11757, 7A615D6C), bn_pack2(521F2B18, 177B200C),
    bn_pack2(D8760273, 3EC86A64), bn_pack2(F12FFA06, D98A0864), bn_pack2(CEE3D226, 1AD2EE6B),
    bn_pack2(1E8C94E0, 4A25619D), bn_pack2(ABF5AE8C, DB0933D7), bn_pack2(B3970F85, A6E1E4C7),
    bn_pack2(8AEA7157, 5D060C7D), bn_pack2(ECFB8504, 58DBEF0A), bn_pack2(A85521AB, DF1CBA64),
    bn_pack2(AD33170D, 04507A33), bn_pack2(15728E5A, 8AAAC42D), bn_pack2(15D22618, 98FA0510),
    bn_pack2(3995497C, EA956AE5), bn_pack2(DE2BCBF6, 95581718), bn_pack2(B5C55DF0, 6F4C52C9),
    bn_pack2(9B2783A2, EC07A28F), bn_pack2(E39E772C, 180E8603), bn_pack2(32905E46, 2E36CE3B),
    bn_pack2(F1746C08, CA18217C), bn_pack2(670C354E, 4ABC9804), bn_pack2(9ED52907, 7096966D),
    bn_pack2(1C62F356, 208552BB), bn_pack2(83655D23, DCA3AD96), bn_pack2(69163FA8, FD24CF5F),
    bn_pack2(98DA4836, 1C55D39A), bn_pack2(C2007CB8, A163BF05), bn_pack2(49286651, ECE45B3D),
    bn_pack2(AE9F2411, 7C4B1FE6), bn_pack2(EE386BFB, 5A899FA5), bn_pack2(0BFF5CB6, F406B7ED),
    bn_pack2(F44C42E9, A637ED6B), bn_pack2(E485B576, 625E7EC6), bn_pack2(4FE1356D, 6D51C245),
    bn_pack2(302B0A6D, F25F1437), bn_pack2(EF9519B3, CD3A431B), bn_pack2(514A0879, 8E3404DD),
    bn_pack2(020BBEA6, 3B139B22), bn_pack2(29024E08, 8A67CC74), bn_pack2(C4C6628B, 80DC1CD1),
    bn_pack2(C90FDAA2, 2168C234), bn_pack2(FFFFFFFF, FFFFFFFF)
};

/* Macro to make a BIGNUM from static data */

# define make_prime_bn(x) static const BIGNUM _bignum_##x = { (BN_ULONG *) x, \
                            sizeof(x)/sizeof(x[0]), \
                            sizeof(x)/sizeof(x[0]), \
                            0, BN_FLG_STATIC_DATA };

#define make_get_bn(x) make_prime_bn(x) \
                        BIGNUM *BN_get_##x(BIGNUM *bn){ \
                        if(bn) return BN_copy(bn, &_bignum_##x); \
                        return BN_dup(&_bignum_##x); }
                        
make_get_bn(rfc2409_prime_768)
make_get_bn(rfc2409_prime_1024)
make_get_bn(rfc3526_prime_1536)
make_get_bn(rfc3526_prime_2048)
make_get_bn(rfc3526_prime_3072)
make_get_bn(rfc3526_prime_4096)
make_get_bn(rfc3526_prime_6144)
make_get_bn(rfc3526_prime_8192)