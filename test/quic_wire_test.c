/*
 * Copyright 2015-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "internal/packet.h"
#include "internal/quic_wire.h"
#include "testutil.h"

struct encode_test_case {
    int (*serializer)(WPACKET *pkt);
    const uint8_t *expect_buf;
    size_t expect_buf_len;
    int (*deserializer)(PACKET *pkt);
};

/* 1. PADDING */
static int encode_case_1_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_padding(pkt, 3), 1))
        return 0;

    return 1;
}

static int encode_case_1_dec(PACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_decode_frame_padding(pkt), 3))
        return 0;

    return 1;
}

static const uint8_t encode_case_1_expect[] = {
    0, 0, 0
};

/* 2. PING */
static int encode_case_2_enc(WPACKET *pkt)
{

    if (!TEST_int_eq(ossl_quic_wire_encode_frame_ping(pkt), 1))
        return 0;

    return 1;
}

static int encode_case_2_dec(PACKET *pkt)
{

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_ping(pkt), 1))
        return 0;

    return 1;
}

static const uint8_t encode_case_2_expect[] = {
    0x01
};

/* 3. ACK */
static const OSSL_ACKM_ACK_RANGE encode_case_3_ranges[] = {
    { 20, 30 },
    {  0, 10 }
};

static const OSSL_ACKM_ACK encode_case_3_f = {
    (OSSL_ACKM_ACK_RANGE *)encode_case_3_ranges,
    OSSL_NELEM(encode_case_3_ranges),
    OSSL_TIME_MS,
    60, 70, 80, 1
};

static int encode_case_3_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_ack(pkt, 3, &encode_case_3_f), 1))
        return 0;

    return 1;
}

static int encode_case_3_dec(PACKET *pkt)
{
    OSSL_ACKM_ACK_RANGE ranges[4] = {0};
    OSSL_ACKM_ACK f = {0};
    uint64_t total_ranges = 0;

    f.ack_ranges        = ranges;
    f.num_ack_ranges    = OSSL_NELEM(ranges);

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_ack(pkt, 3, &f, &total_ranges), 1))
        return 0;

    if (!TEST_uint64_t_eq(total_ranges, 2))
        return 0;

    if (!TEST_mem_eq(f.ack_ranges, f.num_ack_ranges * sizeof(OSSL_ACKM_ACK_RANGE),
                     encode_case_3_f.ack_ranges,
                     encode_case_3_f.num_ack_ranges * sizeof(OSSL_ACKM_ACK_RANGE)))
        return 0;

    if (!TEST_uint64_t_eq(f.delay_time, encode_case_3_f.delay_time))
        return 0;

    if (!TEST_true(f.ecn_present))
        return 0;

    if (!TEST_uint64_t_eq(f.ect0, encode_case_3_f.ect0))
        return 0;

    if (!TEST_uint64_t_eq(f.ect1, encode_case_3_f.ect1))
        return 0;

    if (!TEST_uint64_t_eq(f.ecnce, encode_case_3_f.ecnce))
        return 0;

    return 1;
}

static const uint8_t encode_case_3_expect[] = {
    0x03,                   /* Type */
    0x1E,                   /* Largest Acknowledged */
    0x40, 0x7d,             /* ACK Delay */
    1,                      /* ACK Range Count */
    10,                     /* First ACK Range */

    8,                      /* Gap */
    10,                     /* Length */

    0x3c,                   /* ECT0 Count */
    0x40, 0x46,             /* ECT1 Count */
    0x40, 0x50,             /* ECNCE Count */
};

/* 4. RESET_STREAM */
static const OSSL_QUIC_FRAME_RESET_STREAM encode_case_4_f = {
    0x1234, 0x9781, 0x11717
};

static int encode_case_4_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_reset_stream(pkt,
                                                              &encode_case_4_f), 1))
        return 0;

    return 1;
}

static int encode_case_4_dec(PACKET *pkt)
{
    OSSL_QUIC_FRAME_RESET_STREAM f = {0};

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_reset_stream(pkt, &f), 1))
        return 0;

    if (!TEST_mem_eq(&f, sizeof(f), &encode_case_4_f, sizeof(encode_case_4_f)))
        return 0;

    return 1;
}

static const uint8_t encode_case_4_expect[] = {
    0x04,                   /* Type */
    0x52, 0x34,             /* Stream ID */
    0x80, 0x00, 0x97, 0x81, /* App Error Code */
    0x80, 0x01, 0x17, 0x17, /* Final Size */
};

/* 5. STOP_SENDING */
static const OSSL_QUIC_FRAME_STOP_SENDING encode_case_5_f = {
    0x1234, 0x9781
};

static int encode_case_5_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_stop_sending(pkt,
                                                              &encode_case_5_f), 1))
        return 0;

    return 1;
}

static int encode_case_5_dec(PACKET *pkt)
{
    OSSL_QUIC_FRAME_STOP_SENDING f = {0};

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_stop_sending(pkt, &f), 1))
        return 0;

    if (!TEST_mem_eq(&f, sizeof(f), &encode_case_5_f, sizeof(encode_case_5_f)))
        return 0;

    return 1;
}

static const uint8_t encode_case_5_expect[] = {
    0x05,                   /* Type */
    0x52, 0x34,             /* Stream ID */
    0x80, 0x00, 0x97, 0x81  /* App Error Code */
};

/* 6. CRYPTO */
static const uint8_t encode_case_6_data[] = {
    93, 18, 17, 102, 33
};

static const OSSL_QUIC_FRAME_CRYPTO encode_case_6_f = {
    0x1234, sizeof(encode_case_6_data), encode_case_6_data
};

static int encode_case_6_enc(WPACKET *pkt)
{
    if (!TEST_ptr(ossl_quic_wire_encode_frame_crypto(pkt,
                                                     &encode_case_6_f)))
        return 0;

    return 1;
}

static int encode_case_6_dec(PACKET *pkt)
{
    OSSL_QUIC_FRAME_CRYPTO f = {0};

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_crypto(pkt, &f), 1))
        return 0;

    if (!TEST_uint64_t_eq(f.offset, 0x1234))
        return 0;

    if (!TEST_mem_eq(f.data, f.len, encode_case_6_data, sizeof(encode_case_6_data)))
        return 0;

    return 1;
}

static const uint8_t encode_case_6_expect[] = {
    0x06,                   /* Type */
    0x52, 0x34,             /* Offset */
    0x05,                   /* Length */
    93, 18, 17, 102, 33     /* Data */
};

/* 7. NEW_TOKEN */
static const uint8_t encode_case_7_token[] = {
    0xde, 0x06, 0xcb, 0x76, 0x5d, 0xb1, 0xa7, 0x71,
    0x78, 0x09, 0xbb, 0xe8, 0x50, 0x19, 0x12, 0x9a
};

static int encode_case_7_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_new_token(pkt,
                                                        encode_case_7_token,
                                                        sizeof(encode_case_7_token)), 1))
        return 0;

    return 1;
}

static int encode_case_7_dec(PACKET *pkt)
{
    const uint8_t *token = NULL;
    size_t token_len = 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_new_token(pkt,
                                                           &token,
                                                           &token_len), 1))
        return 0;

    if (!TEST_mem_eq(token, token_len,
                     encode_case_7_token, sizeof(encode_case_7_token)))
        return 0;

    return 1;
}

static const uint8_t encode_case_7_expect[] = {
    0x07,                   /* Type */
    0x10,                   /* Length */
    0xde, 0x06, 0xcb, 0x76, 0x5d, 0xb1, 0xa7, 0x71, /* Token */
    0x78, 0x09, 0xbb, 0xe8, 0x50, 0x19, 0x12, 0x9a
};

/* 8. STREAM (no length, no offset, no fin) */
static const uint8_t encode_case_8_data[] = {
    0xde, 0x06, 0xcb, 0x76, 0x5d
};
static const OSSL_QUIC_FRAME_STREAM encode_case_8_f = {
   0x1234, 0, 5, encode_case_8_data, 0, 0
};

static int encode_case_8_enc(WPACKET *pkt)
{
    if (!TEST_ptr(ossl_quic_wire_encode_frame_stream(pkt,
                                                     &encode_case_8_f)))
        return 0;

    return 1;
}

static int encode_case_8_dec(PACKET *pkt)
{
    OSSL_QUIC_FRAME_STREAM f = {0};

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_stream(pkt, &f), 1))
        return 0;

    if (!TEST_mem_eq(f.data, f.len,
                     encode_case_8_data, sizeof(encode_case_8_data)))
        return 0;

    if (!TEST_uint64_t_eq(f.stream_id, 0x1234))
        return 0;

    if (!TEST_uint64_t_eq(f.offset, 0))
        return 0;

    if (!TEST_int_eq(f.has_len, 0))
        return 0;

    if (!TEST_int_eq(f.is_fin, 0))
        return 0;

    return 1;
}

static const uint8_t encode_case_8_expect[] = {
    0x08,                           /* Type (OFF=0, LEN=0, FIN=0) */
    0x52, 0x34,                     /* Stream ID */
    0xde, 0x06, 0xcb, 0x76, 0x5d    /* Data */
};

/* 9. STREAM (length, offset, fin) */
static const uint8_t encode_case_9_data[] = {
    0xde, 0x06, 0xcb, 0x76, 0x5d
};
static const OSSL_QUIC_FRAME_STREAM encode_case_9_f = {
   0x1234, 0x39, 5, encode_case_9_data, 1, 1
};

static int encode_case_9_enc(WPACKET *pkt)
{
    if (!TEST_ptr(ossl_quic_wire_encode_frame_stream(pkt,
                                                     &encode_case_9_f)))
        return 0;

    return 1;
}

static int encode_case_9_dec(PACKET *pkt)
{
    OSSL_QUIC_FRAME_STREAM f = {0};

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_stream(pkt, &f), 1))
        return 0;

    if (!TEST_mem_eq(f.data, f.len,
                     encode_case_9_data, sizeof(encode_case_9_data)))
        return 0;

    if (!TEST_uint64_t_eq(f.stream_id, 0x1234))
        return 0;

    if (!TEST_uint64_t_eq(f.offset, 0x39))
        return 0;

    if (!TEST_int_eq(f.has_len, 1))
        return 0;

    if (!TEST_int_eq(f.is_fin, 1))
        return 0;

    return 1;
}

static const uint8_t encode_case_9_expect[] = {
    0x0f,                           /* Type (OFF=1, LEN=1, FIN=1) */
    0x52, 0x34,                     /* Stream ID */
    0x39,                           /* Offset */
    0x05,                           /* Length */
    0xde, 0x06, 0xcb, 0x76, 0x5d    /* Data */
};

/* 10. MAX_DATA */
static int encode_case_10_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_max_data(pkt, 0x1234), 1))
        return 0;

    return 1;
}

static int encode_case_10_dec(PACKET *pkt)
{
    uint64_t max_data = 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_max_data(pkt, &max_data), 1))
        return 0;

    if (!TEST_uint64_t_eq(max_data, 0x1234))
        return 0;

    return 1;
}

static const uint8_t encode_case_10_expect[] = {
    0x10,                           /* Type */
    0x52, 0x34,                     /* Max Data */
};

/* 11. MAX_STREAM_DATA */
static int encode_case_11_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_max_stream_data(pkt,
                                                                 0x1234,
                                                                 0x9781), 1))
        return 0;

    return 1;
}

static int encode_case_11_dec(PACKET *pkt)
{
    uint64_t stream_id = 0, max_data = 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_max_stream_data(pkt,
                                                                 &stream_id,
                                                                 &max_data), 1))
        return 0;

    if (!TEST_uint64_t_eq(stream_id, 0x1234))
        return 0;

    if (!TEST_uint64_t_eq(max_data, 0x9781))
        return 0;

    return 1;
}

static const uint8_t encode_case_11_expect[] = {
    0x11,                           /* Type */
    0x52, 0x34,                     /* Stream ID */
    0x80, 0x00, 0x97, 0x81,         /* Max Data */
};

/* 12. MAX_STREAMS */
static int encode_case_12_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_max_streams(pkt, 0, 0x1234), 1))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_encode_frame_max_streams(pkt, 1, 0x9781), 1))
        return 0;

    return 1;
}

static int encode_case_12_dec(PACKET *pkt)
{
    uint64_t max_streams_1 = 0, max_streams_2 = 0,
            frame_type_1 = 0, frame_type_2 = 0;

    if (!TEST_int_eq(ossl_quic_wire_peek_frame_header(pkt, &frame_type_1), 1))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_max_streams(pkt,
                                                             &max_streams_1), 1))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_peek_frame_header(pkt, &frame_type_2), 1))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_max_streams(pkt,
                                                             &max_streams_2), 1))
        return 0;

    if (!TEST_uint64_t_eq(frame_type_1, OSSL_QUIC_FRAME_TYPE_MAX_STREAMS_BIDI))
        return 0;

    if (!TEST_uint64_t_eq(max_streams_1, 0x1234))
        return 0;

    if (!TEST_uint64_t_eq(frame_type_2, OSSL_QUIC_FRAME_TYPE_MAX_STREAMS_UNI))
        return 0;

    if (!TEST_uint64_t_eq(max_streams_2, 0x9781))
        return 0;

    return 1;
}

static const uint8_t encode_case_12_expect[] = {
    0x12,                           /* Type (MAX_STREAMS Bidirectional) */
    0x52, 0x34,                     /* Max Streams */
    0x13,                           /* Type (MAX_STREAMS Unidirectional) */
    0x80, 0x00, 0x97, 0x81,         /* Max Streams */
};

/* 13. DATA_BLOCKED */
static int encode_case_13_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_data_blocked(pkt, 0x1234), 1))
        return 0;

    return 1;
}

static int encode_case_13_dec(PACKET *pkt)
{
    uint64_t max_data = 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_data_blocked(pkt,
                                                              &max_data), 1))
        return 0;

    if (!TEST_uint64_t_eq(max_data, 0x1234))
        return 0;

    return 1;
}

static const uint8_t encode_case_13_expect[] = {
    0x14,                           /* Type */
    0x52, 0x34,                     /* Max Data */
};

/* 14. STREAM_DATA_BLOCKED */
static int encode_case_14_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_stream_data_blocked(pkt,
                                                                     0x1234,
                                                                     0x9781), 1))
        return 0;

    return 1;
}

static int encode_case_14_dec(PACKET *pkt)
{
    uint64_t stream_id = 0, max_data = 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_stream_data_blocked(pkt,
                                                                     &stream_id,
                                                                     &max_data), 1))
        return 0;

    if (!TEST_uint64_t_eq(stream_id, 0x1234))
        return 0;

    if (!TEST_uint64_t_eq(max_data, 0x9781))
        return 0;

    return 1;
}

static const uint8_t encode_case_14_expect[] = {
    0x15,                           /* Type */
    0x52, 0x34,                     /* Stream ID */
    0x80, 0x00, 0x97, 0x81,         /* Max Data */
};

/* 15. STREAMS_BLOCKED */
static int encode_case_15_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_streams_blocked(pkt, 0, 0x1234), 1))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_encode_frame_streams_blocked(pkt, 1, 0x9781), 1))
        return 0;

    return 1;
}

static int encode_case_15_dec(PACKET *pkt)
{
    uint64_t max_streams_1 = 0, max_streams_2 = 0,
            frame_type_1 = 0, frame_type_2 = 0;

    if (!TEST_int_eq(ossl_quic_wire_peek_frame_header(pkt, &frame_type_1), 1))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_streams_blocked(pkt,
                                                                 &max_streams_1), 1))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_peek_frame_header(pkt, &frame_type_2), 1))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_streams_blocked(pkt,
                                                                 &max_streams_2), 1))
        return 0;

    if (!TEST_uint64_t_eq(frame_type_1, OSSL_QUIC_FRAME_TYPE_STREAMS_BLOCKED_BIDI))
        return 0;

    if (!TEST_uint64_t_eq(max_streams_1, 0x1234))
        return 0;

    if (!TEST_uint64_t_eq(frame_type_2, OSSL_QUIC_FRAME_TYPE_STREAMS_BLOCKED_UNI))
        return 0;

    if (!TEST_uint64_t_eq(max_streams_2, 0x9781))
        return 0;

    return 1;
}

static const uint8_t encode_case_15_expect[] = {
    0x16,                           /* Type (STREAMS_BLOCKED Bidirectional) */
    0x52, 0x34,                     /* Max Streams */
    0x17,                           /* Type (STREAMS_BLOCKED Unidirectional) */
    0x80, 0x00, 0x97, 0x81,         /* Max Streams */
};

/* 16. NEW_CONNECTION_ID */
static const uint8_t encode_case_16_conn_id[] = {
    0x33, 0x44, 0x55, 0x66
};

static const OSSL_QUIC_FRAME_NEW_CONN_ID encode_case_16_f = {
    0x1234,
    0x9781,
    encode_case_16_conn_id,
    sizeof(encode_case_16_conn_id),
    {
        0xde, 0x06, 0xcb, 0x76, 0x5d, 0xb1, 0xa7, 0x71,
        0x78, 0x09, 0xbb, 0xe8, 0x50, 0x19, 0x12, 0x9a
    }
};

static int encode_case_16_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_new_conn_id(pkt,
                                                             &encode_case_16_f), 1))
        return 0;

    return 1;
}

static int encode_case_16_dec(PACKET *pkt)
{
    OSSL_QUIC_FRAME_NEW_CONN_ID f = {0};

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_new_conn_id(pkt, &f), 1))
        return 0;

    if (!TEST_uint64_t_eq(f.seq_num, 0x1234))
        return 0;

    if (!TEST_uint64_t_eq(f.retire_prior_to, 0x9781))
        return 0;

    if (!TEST_uint64_t_eq(f.conn_id_len, sizeof(encode_case_16_conn_id)))
        return 0;

    if (!TEST_mem_eq(f.conn_id, f.conn_id_len,
                     encode_case_16_conn_id, sizeof(encode_case_16_conn_id)))
        return 0;

    if (!TEST_mem_eq(f.stateless_reset_token,
                     sizeof(f.stateless_reset_token),
                     encode_case_16_f.stateless_reset_token,
                     sizeof(encode_case_16_f.stateless_reset_token)))
        return 0;

    return 1;
}

static const uint8_t encode_case_16_expect[] = {
    0x18,                           /* Type */
    0x52, 0x34,                     /* Sequence Number */
    0x80, 0x00, 0x97, 0x81,         /* Retire Prior To */
    0x04,                           /* Connection ID Length */
    0x33, 0x44, 0x55, 0x66,         /* Connection ID */
    0xde, 0x06, 0xcb, 0x76, 0x5d, 0xb1, 0xa7, 0x71, /* Stateless Reset Token */
    0x78, 0x09, 0xbb, 0xe8, 0x50, 0x19, 0x12, 0x9a
};

/* 17. RETIRE_CONNECTION_ID */
static int encode_case_17_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_retire_conn_id(pkt, 0x1234), 1))
        return 0;

    return 1;
}

static int encode_case_17_dec(PACKET *pkt)
{
    uint64_t seq_num = 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_retire_conn_id(pkt, &seq_num), 1))
        return 0;

    if (!TEST_uint64_t_eq(seq_num, 0x1234))
        return 0;

    return 1;
}

static const uint8_t encode_case_17_expect[] = {
    0x19,                           /* Type */
    0x52, 0x34,                     /* Seq Num */
};

/* 18. PATH_CHALLENGE */
static const uint64_t encode_case_18_data
    = (((uint64_t)0x5f4b12)<<40) | (uint64_t)0x731834UL;

static int encode_case_18_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_path_challenge(pkt,
                                                                encode_case_18_data), 1))
        return 0;

    return 1;
}

static int encode_case_18_dec(PACKET *pkt)
{
    uint64_t challenge = 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_path_challenge(pkt, &challenge), 1))
        return 0;

    if (!TEST_uint64_t_eq(challenge, encode_case_18_data))
        return 0;

    return 1;
}

static const uint8_t encode_case_18_expect[] = {
    0x1A,                                           /* Type */
    0x5f, 0x4b, 0x12, 0x00, 0x00, 0x73, 0x18, 0x34, /* Data */
};

/* 19. PATH_RESPONSE */
static const uint64_t encode_case_19_data
    = (((uint64_t)0x5f4b12)<<40) | (uint64_t)0x731834UL;

static int encode_case_19_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_path_response(pkt,
                                                               encode_case_19_data), 1))
        return 0;

    return 1;
}

static int encode_case_19_dec(PACKET *pkt)
{
    uint64_t challenge = 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_path_response(pkt, &challenge), 1))
        return 0;

    if (!TEST_uint64_t_eq(challenge, encode_case_19_data))
        return 0;

    return 1;
}

static const uint8_t encode_case_19_expect[] = {
    0x1B,                                           /* Type */
    0x5f, 0x4b, 0x12, 0x00, 0x00, 0x73, 0x18, 0x34, /* Data */
};

/* 20. CONNECTION_CLOSE (transport) */
static const char encode_case_20_reason[] = {
    /* "reason for closure" */
    0x72, 0x65, 0x61, 0x73, 0x6f, 0x6e, 0x20, 0x66, 0x6f,
    0x72, 0x20, 0x63, 0x6c, 0x6f, 0x73, 0x75, 0x72, 0x65
};

static const OSSL_QUIC_FRAME_CONN_CLOSE encode_case_20_f = {
    0,
    0x1234,
    0x9781,
    encode_case_20_reason,
    sizeof(encode_case_20_reason)
};

static int encode_case_20_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_frame_conn_close(pkt,
                                                            &encode_case_20_f), 1))
        return 0;

    return 1;
}

static int encode_case_20_dec(PACKET *pkt)
{
    OSSL_QUIC_FRAME_CONN_CLOSE f = {0};

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_conn_close(pkt, &f), 1))
        return 0;

    if (!TEST_int_eq(f.is_app, 0))
        return 0;

    if (!TEST_uint64_t_eq(f.error_code, 0x1234))
        return 0;

    if (!TEST_uint64_t_eq(f.frame_type, 0x9781))
        return 0;

    if (!TEST_size_t_eq(f.reason_len, 18))
        return 0;

    if (!TEST_mem_eq(f.reason, f.reason_len,
                     encode_case_20_f.reason, encode_case_20_f.reason_len))
        return 0;

    return 1;
}

static const uint8_t encode_case_20_expect[] = {
    0x1C,                           /* Type */
    0x52, 0x34,                     /* Sequence Number */
    0x80, 0x00, 0x97, 0x81,         /* Frame Type */
    0x12,                           /* Reason Length */
    0x72, 0x65, 0x61, 0x73, 0x6f, 0x6e, 0x20, 0x66, 0x6f, /* Reason */
    0x72, 0x20, 0x63, 0x6c, 0x6f, 0x73, 0x75, 0x72, 0x65
};

/* 21. HANDSHAKE_DONE */
static int encode_case_21_enc(WPACKET *pkt)
{

    if (!TEST_int_eq(ossl_quic_wire_encode_frame_handshake_done(pkt), 1))
        return 0;

    return 1;
}

static int encode_case_21_dec(PACKET *pkt)
{

    if (!TEST_int_eq(ossl_quic_wire_decode_frame_handshake_done(pkt), 1))
        return 0;

    return 1;
}

static const uint8_t encode_case_21_expect[] = {
    0x1E
};

/* 22. Buffer Transport Parameter */
static const uint8_t encode_case_22_data[] = {0x55,0x77,0x32,0x46,0x99};

static int encode_case_22_enc(WPACKET *pkt)
{
    uint8_t *p;

    if (!TEST_ptr(ossl_quic_wire_encode_transport_param_bytes(pkt, 0x1234,
                                                              encode_case_22_data,
                                                              sizeof(encode_case_22_data))))
        return 0;

    if (!TEST_ptr(p = ossl_quic_wire_encode_transport_param_bytes(pkt, 0x9781,
                                                                  NULL, 2)))
        return 0;

    p[0] = 0x33;
    p[1] = 0x44;

    return 1;
}

static int encode_case_22_dec(PACKET *pkt)
{
    uint64_t id = 0;
    size_t len = 0;
    const uint8_t *p;
    static const uint8_t data[] = {0x33, 0x44};

    if (!TEST_int_eq(ossl_quic_wire_peek_transport_param(pkt, &id), 1))
        return 0;

    if (!TEST_uint64_t_eq(id, 0x1234))
        return 0;

    id = 0;

    if (!TEST_ptr(p = ossl_quic_wire_decode_transport_param_bytes(pkt,
                                                                  &id, &len)))
        return 0;

    if (!TEST_uint64_t_eq(id, 0x1234))
        return 0;

    if (!TEST_mem_eq(p, len, encode_case_22_data, sizeof(encode_case_22_data)))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_peek_transport_param(pkt, &id), 1))
        return 0;

    if (!TEST_uint64_t_eq(id, 0x9781))
        return 0;

    id = 0;

    if (!TEST_ptr(p = ossl_quic_wire_decode_transport_param_bytes(pkt,
                                                                  &id, &len)))
        return 0;

    if (!TEST_uint64_t_eq(id, 0x9781))
        return 0;

    if (!TEST_mem_eq(p, len, data, sizeof(data)))
        return 0;

    return 1;
}

static const uint8_t encode_case_22_expect[] = {
    0x52, 0x34,                         /* ID */
    0x05,                               /* Length */
    0x55, 0x77, 0x32, 0x46, 0x99,       /* Data */

    0x80, 0x00, 0x97, 0x81,             /* ID */
    0x02,                               /* Length */
    0x33, 0x44                          /* Data */
};

/* 23. Integer Transport Parameter */
static int encode_case_23_enc(WPACKET *pkt)
{
    if (!TEST_int_eq(ossl_quic_wire_encode_transport_param_int(pkt, 0x1234, 0x9781), 1))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_encode_transport_param_int(pkt, 0x2233, 0x4545), 1))
        return 0;

    return 1;
}

static int encode_case_23_dec(PACKET *pkt)
{
    uint64_t id = 0, value = 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_transport_param_int(pkt,
                                                               &id, &value), 1))
        return 0;

    if (!TEST_uint64_t_eq(id, 0x1234))
        return 0;

    if (!TEST_uint64_t_eq(value, 0x9781))
        return 0;

    if (!TEST_int_eq(ossl_quic_wire_decode_transport_param_int(pkt,
                                                               &id, &value), 1))
        return 0;

    if (!TEST_uint64_t_eq(id, 0x2233))
        return 0;

    if (!TEST_uint64_t_eq(value, 0x4545))
        return 0;

    return 1;
}

static const uint8_t encode_case_23_expect[] = {
    0x52, 0x34,
    0x04,
    0x80, 0x00, 0x97, 0x81,

    0x62, 0x33,
    0x04,
    0x80, 0x00, 0x45, 0x45,
};

#define ENCODE_CASE(n)                          \
    {                                           \
      encode_case_##n##_enc,                    \
      encode_case_##n##_expect,                 \
      OSSL_NELEM(encode_case_##n##_expect),     \
      encode_case_##n##_dec                     \
    },

static const struct encode_test_case encode_cases[] = {
    ENCODE_CASE(1)
    ENCODE_CASE(2)
    ENCODE_CASE(3)
    ENCODE_CASE(4)
    ENCODE_CASE(5)
    ENCODE_CASE(6)
    ENCODE_CASE(7)
    ENCODE_CASE(8)
    ENCODE_CASE(9)
    ENCODE_CASE(10)
    ENCODE_CASE(11)
    ENCODE_CASE(12)
    ENCODE_CASE(13)
    ENCODE_CASE(14)
    ENCODE_CASE(15)
    ENCODE_CASE(16)
    ENCODE_CASE(17)
    ENCODE_CASE(18)
    ENCODE_CASE(19)
    ENCODE_CASE(20)
    ENCODE_CASE(21)
    ENCODE_CASE(22)
    ENCODE_CASE(23)
};

static int test_wire_encode(int idx)
{
    int testresult = 0;
    WPACKET wpkt;
    PACKET pkt;
    BUF_MEM *buf = NULL;
    size_t written;
    const struct encode_test_case *c = &encode_cases[idx];
    int have_wpkt = 0;

    if (!TEST_ptr(buf = BUF_MEM_new()))
        goto err;

    if (!TEST_int_eq(WPACKET_init(&wpkt, buf), 1))
        goto err;

    have_wpkt = 1;
    if (!TEST_int_eq(c->serializer(&wpkt), 1))
        goto err;

    if (!TEST_int_eq(WPACKET_get_total_written(&wpkt, &written), 1))
        goto err;

    if (!TEST_mem_eq(buf->data, written, c->expect_buf, c->expect_buf_len))
        goto err;

    if (!TEST_int_eq(PACKET_buf_init(&pkt, (unsigned char *)buf->data, written), 1))
        goto err;

    if (!TEST_int_eq(c->deserializer(&pkt), 1))
        goto err;

    if (!TEST_false(pkt.remaining))
        goto err;

    testresult = 1;
err:
    if (have_wpkt)
        WPACKET_finish(&wpkt);
    BUF_MEM_free(buf);
    return testresult;
}

struct ack_test_case {
    const uint8_t  *input_buf;
    size_t          input_buf_len;
    int           (*deserializer)(PACKET *pkt);
    int             expect_fail;
};

/* ACK Frame with Excessive First ACK Range Field */
static const uint8_t ack_case_1_input[] = {
    0x02,           /* ACK Without ECN */
    0x08,           /* Largest Acknowledged */
    0x01,           /* ACK Delay */
    0x00,           /* ACK Range Count */
    0x09,           /* First ACK Range */
};

/* ACK Frame with Valid ACK Range Field */
static const uint8_t ack_case_2_input[] = {
    0x02,           /* ACK Without ECN */
    0x08,           /* Largest Acknowledged */
    0x01,           /* ACK Delay */
    0x00,           /* ACK Range Count */
    0x08,           /* First ACK Range */
};

/* ACK Frame with Excessive ACK Range Gap */
static const uint8_t ack_case_3_input[] = {
    0x02,           /* ACK Without ECN */
    0x08,           /* Largest Acknowledged */
    0x01,           /* ACK Delay */
    0x01,           /* ACK Range Count */
    0x01,           /* First ACK Range */

    0x05,           /* Gap */
    0x01,           /* ACK Range Length */
};

/* ACK Frame with Valid ACK Range */
static const uint8_t ack_case_4_input[] = {
    0x02,           /* ACK Without ECN */
    0x08,           /* Largest Acknowledged */
    0x01,           /* ACK Delay */
    0x01,           /* ACK Range Count */
    0x01,           /* First ACK Range */

    0x04,           /* Gap */
    0x01,           /* ACK Range Length */
};

/* ACK Frame with Excessive ACK Range Length */
static const uint8_t ack_case_5_input[] = {
    0x02,           /* ACK Without ECN */
    0x08,           /* Largest Acknowledged */
    0x01,           /* ACK Delay */
    0x01,           /* ACK Range Count */
    0x01,           /* First ACK Range */

    0x04,           /* Gap */
    0x02,           /* ACK Range Length */
};

/* ACK Frame with Multiple ACK Ranges, Final Having Excessive Length */
static const uint8_t ack_case_6_input[] = {
    0x02,           /* ACK Without ECN */
    0x08,           /* Largest Acknowledged */
    0x01,           /* ACK Delay */
    0x02,           /* ACK Range Count */
    0x01,           /* First ACK Range */

    0x01,           /* Gap */
    0x02,           /* ACK Range Length */

    0x00,           /* Gap */
    0x01,           /* ACK Range Length */
};

/* ACK Frame with Multiple ACK Ranges, Valid */
static const uint8_t ack_case_7_input[] = {
    0x02,           /* ACK Without ECN */
    0x08,           /* Largest Acknowledged */
    0x01,           /* ACK Delay */
    0x02,           /* ACK Range Count */
    0x01,           /* First ACK Range */

    0x01,           /* Gap */
    0x02,           /* ACK Range Length */

    0x00,           /* Gap */
    0x00,           /* ACK Range Length */
};

static int ack_generic_decode(PACKET *pkt)
{
    OSSL_ACKM_ACK_RANGE ranges[8] = {0};
    OSSL_ACKM_ACK f = {0};
    uint64_t total_ranges = 0;
    int r;
    size_t i;

    f.ack_ranges        = ranges;
    f.num_ack_ranges    = OSSL_NELEM(ranges);

    r = ossl_quic_wire_decode_frame_ack(pkt, 3, &f, &total_ranges);
    if (r == 0)
        return 0;

    for (i = 0; i < f.num_ack_ranges; ++i) {
        if (!TEST_uint64_t_le(f.ack_ranges[i].start, f.ack_ranges[i].end))
            return 0;
        if (!TEST_uint64_t_lt(f.ack_ranges[i].end, 1000))
            return 0;
    }

    return 1;
}

#define ACK_CASE(n, expect_fail, dec)   \
    {                                   \
        ack_case_##n##_input,           \
        sizeof(ack_case_##n##_input),   \
        (dec),                          \
        (expect_fail)                   \
    },

static const struct ack_test_case ack_cases[] = {
    ACK_CASE(1, 1, ack_generic_decode)
    ACK_CASE(2, 0, ack_generic_decode)
    ACK_CASE(3, 1, ack_generic_decode)
    ACK_CASE(4, 0, ack_generic_decode)
    ACK_CASE(5, 1, ack_generic_decode)
    ACK_CASE(6, 1, ack_generic_decode)
    ACK_CASE(7, 0, ack_generic_decode)
};

static int test_wire_ack(int idx)
{
    int testresult = 0, r;
    PACKET pkt;
    const struct ack_test_case *c = &ack_cases[idx];

    if (!TEST_int_eq(PACKET_buf_init(&pkt,
                                     (unsigned char *)c->input_buf,
                                     c->input_buf_len), 1))
        goto err;

    r = c->deserializer(&pkt);
    if (c->expect_fail) {
        if (!TEST_int_eq(r, 0))
            goto err;
    } else {
        if (!TEST_int_eq(r, 1))
            goto err;

        if (!TEST_false(pkt.remaining))
            goto err;
    }

    testresult = 1;
err:
    return testresult;
}

int setup_tests(void)
{
    ADD_ALL_TESTS(test_wire_encode, OSSL_NELEM(encode_cases));
    ADD_ALL_TESTS(test_wire_ack,    OSSL_NELEM(ack_cases));
    return 1;
}
