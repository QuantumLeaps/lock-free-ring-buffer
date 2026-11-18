/*============================================================================
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2021 Quantum Leaps, LLC. All rights reserved.
*
* SPDX-License-Identifier: MIT
*
* Contact information:
* <www.state-machine.com>
* <info@state-machine.com>
============================================================================*/
#include <stdint.h>
#include <stdbool.h>

#include "ring_buf.h"
#include "et.h" /* ET: embedded test */

RingBufElement buf[8];
RingBuf rb;

/* ring-buffer "handler" function for RingBuf_process_all() */
static void rb_handler(RingBufElement const el);

static RingBufElement test_data[] = {
    0xAAU,
    0xBBU,
    0xCCU,
    0xDDU
};
static RingBufCtr test_idx;

/* additional fixtures for the independent boundary tests ..................
* Each of the tests below constructs its own ring buffer (rb2), so it does
* not depend on the state left by any other test.
*/
static RingBufElement buf2[8];
static RingBuf rb2;

/* recording handler for RingBuf_process_all() */
static void rec_handler(RingBufElement const el);
static RingBufElement rec_data[16];
static RingBufCtr rec_cnt;

/* advance the head and tail of an *empty* buffer by n positions
* (using only the public API), so that the buffer stays empty
*/
static void rb_advance(RingBuf * const me, RingBufCtr n) {
    RingBufElement el;
    for (; n > 0U; --n) {
        VERIFY(true == RingBuf_put(me, 0x00U));
        VERIFY(true == RingBuf_get(me, &el));
    }
}

void setup(void) {
    /* executed before *every* non-skipped test */
}

void teardown(void) {
    /* executed after *every* non-skipped and non-failing test */
}

/* test group --------------------------------------------------------------*/
TEST_GROUP("lock-free ring buffer") {

RingBuf_ctor(&rb, buf, ARRAY_NELEM(buf));

TEST("RingBuf_num_free") {
    VERIFY(RingBuf_num_free(&rb) == ARRAY_NELEM(buf) - 1U);
}

TEST("RingBuf_put 3") {
    RingBuf_put(&rb, 0xAAU);
    RingBuf_put(&rb, 0xBBU);
    RingBuf_put(&rb, 0xCCU);
    VERIFY(RingBuf_num_free(&rb) == ARRAY_NELEM(buf) - 1U - 3U);
}

TEST("RingBuf_get") {
    RingBufElement el;
    VERIFY(true == RingBuf_get(&rb, &el));
    VERIFY(0xAAU == el);
    VERIFY(true == RingBuf_get(&rb, &el));
    VERIFY(0xBBU == el);
    VERIFY(true == RingBuf_get(&rb, &el));
    VERIFY(0xCCU == el);
    VERIFY(false == RingBuf_get(&rb, &el));
}

TEST("RingBuf_process_all test_data") {
    for (RingBufCtr i = 0U; i < ARRAY_NELEM(test_data); ++i) {
        RingBuf_put(&rb, test_data[i]);
    }
    test_idx = 0U;
    RingBuf_process_all(&rb, &rb_handler);
    VERIFY(RingBuf_num_free(&rb) == ARRAY_NELEM(buf) - 1U);
}

/* boundary tests (each one independent) ----------------------------------*/
TEST("ctor: new buffer is empty with capacity sto_len - 1") {
    RingBufElement el = 0x5AU;
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U);
    VERIFY(false == RingBuf_get(&rb2, &el));
    VERIFY(0x5AU == el); /* output not touched */
}

TEST("put: buffer fills completely with sto_len - 1 elements") {
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    for (RingBufCtr i = 0U; i < ARRAY_NELEM(buf2) - 1U; ++i) {
        VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U - i);
        VERIFY(true == RingBuf_put(&rb2, (RingBufElement)(0x10U + i)));
    }
    VERIFY(RingBuf_num_free(&rb2) == 0U);
}

TEST("put: on a full buffer returns false and keeps the contents") {
    RingBufElement el;
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    for (RingBufCtr i = 0U; i < ARRAY_NELEM(buf2) - 1U; ++i) {
        VERIFY(true == RingBuf_put(&rb2, (RingBufElement)(0x10U + i)));
    }
    VERIFY(false == RingBuf_put(&rb2, 0xEEU));
    VERIFY(false == RingBuf_put(&rb2, 0xEEU)); /* repeated: still full */
    VERIFY(RingBuf_num_free(&rb2) == 0U);
    for (RingBufCtr i = 0U; i < ARRAY_NELEM(buf2) - 1U; ++i) {
        VERIFY(true == RingBuf_get(&rb2, &el));
        VERIFY((RingBufElement)(0x10U + i) == el); /* FIFO, 0xEE dropped */
    }
    VERIFY(false == RingBuf_get(&rb2, &el));
}

TEST("put: on a full buffer with wrapped head returns false") {
    RingBufElement el;
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    rb_advance(&rb2, 5U); /* head == tail == 5 */
    for (RingBufCtr i = 0U; i < ARRAY_NELEM(buf2) - 1U; ++i) {
        VERIFY(true == RingBuf_put(&rb2, (RingBufElement)(0x20U + i)));
    }
    VERIFY(false == RingBuf_put(&rb2, 0xEEU)); /* full: head is tail - 1 */
    VERIFY(RingBuf_num_free(&rb2) == 0U);
    for (RingBufCtr i = 0U; i < ARRAY_NELEM(buf2) - 1U; ++i) {
        VERIFY(true == RingBuf_get(&rb2, &el));
        VERIFY((RingBufElement)(0x20U + i) == el);
    }
}

TEST("get: on an empty buffer returns false and leaves output untouched") {
    RingBufElement el;
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    VERIFY(true == RingBuf_put(&rb2, 0x11U));
    VERIFY(true == RingBuf_get(&rb2, &el));
    VERIFY(0x11U == el);
    el = 0x5AU;
    VERIFY(false == RingBuf_get(&rb2, &el)); /* drained -> empty */
    VERIFY(0x5AU == el);
    VERIFY(false == RingBuf_get(&rb2, &el)); /* repeated: still empty */
    VERIFY(0x5AU == el);
    VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U);
}

TEST("put/get: exactly one element") {
    RingBufElement el = 0U;
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    VERIFY(true == RingBuf_put(&rb2, 0x00U)); /* element value 0 */
    VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 2U);
    el = 0xFFU;
    VERIFY(true == RingBuf_get(&rb2, &el));
    VERIFY(0x00U == el);
    VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U);
}

TEST("num_free: correct for every fill level at every start index") {
    RingBufElement el;
    for (RingBufCtr start = 0U; start < ARRAY_NELEM(buf2); ++start) {
        RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
        rb_advance(&rb2, start);
        for (RingBufCtr n = 0U; n < ARRAY_NELEM(buf2); ++n) {
            /* n elements stored: covers head > tail, head < tail, == */
            VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U - n);
            if (n < ARRAY_NELEM(buf2) - 1U) {
                VERIFY(true == RingBuf_put(&rb2, (RingBufElement)n));
            }
        }
        for (RingBufCtr n = 0U; n < ARRAY_NELEM(buf2) - 1U; ++n) {
            VERIFY(true == RingBuf_get(&rb2, &el));
            VERIFY((RingBufElement)n == el);
            VERIFY(RingBuf_num_free(&rb2) == n + 1U);
        }
    }
}

TEST("put/get: head and tail wrap around over many cycles") {
    RingBufElement el;
    RingBufElement wr = 0U; /* next value to write (wraps at 256) */
    RingBufElement rd = 0U; /* next value expected to read */
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    /* 3 is co-prime with 8, so every head/tail position is visited;
    * 70000 cycles > 65536 to exercise far more operations than RingBufCtr
    * can count and wrap the 8-bit element value many times
    */
    for (uint32_t cycle = 0U; cycle < 70000U; ++cycle) {
        for (uint32_t i = 0U; i < 3U; ++i) {
            VERIFY(true == RingBuf_put(&rb2, wr));
            ++wr;
        }
        VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U - 3U);
        for (uint32_t i = 0U; i < 3U; ++i) {
            VERIFY(true == RingBuf_get(&rb2, &el));
            VERIFY(rd == el);
            ++rd;
        }
        VERIFY(false == RingBuf_get(&rb2, &el));
        VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U);
    }
}

TEST("put/get: full/empty cycles with wrap-around") {
    RingBufElement el;
    RingBufElement wr = 0U;
    RingBufElement rd = 0U;
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    for (uint32_t cycle = 0U; cycle < 2U * ARRAY_NELEM(buf2); ++cycle) {
        while (RingBuf_put(&rb2, wr)) {
            ++wr;
        }
        VERIFY(RingBuf_num_free(&rb2) == 0U);
        while (RingBuf_get(&rb2, &el)) {
            VERIFY(rd == el);
            ++rd;
        }
        VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U);
        VERIFY(rd == wr);
    }
    VERIFY((RingBufElement)(2U * ARRAY_NELEM(buf2)
                            * (ARRAY_NELEM(buf2) - 1U)) == wr);
}

TEST("process_all: on an empty buffer does not call the handler") {
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    rec_cnt = 0U;
    RingBuf_process_all(&rb2, &rec_handler);
    VERIFY(0U == rec_cnt);
    VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U);

    rb_advance(&rb2, ARRAY_NELEM(buf2) - 1U); /* empty, indexes at end-1 */
    RingBuf_process_all(&rb2, &rec_handler);
    VERIFY(0U == rec_cnt);
    VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U);
}

TEST("process_all: full buffer with wrapped indexes, in FIFO order") {
    RingBufElement el;
    RingBuf_ctor(&rb2, buf2, ARRAY_NELEM(buf2));
    rb_advance(&rb2, 6U); /* tail near the end, so the content wraps */
    for (RingBufCtr i = 0U; i < ARRAY_NELEM(buf2) - 1U; ++i) {
        VERIFY(true == RingBuf_put(&rb2, (RingBufElement)(0x30U + i)));
    }
    rec_cnt = 0U;
    RingBuf_process_all(&rb2, &rec_handler);
    VERIFY(ARRAY_NELEM(buf2) - 1U == rec_cnt);
    for (RingBufCtr i = 0U; i < rec_cnt; ++i) {
        VERIFY((RingBufElement)(0x30U + i) == rec_data[i]);
    }
    VERIFY(RingBuf_num_free(&rb2) == ARRAY_NELEM(buf2) - 1U);
    VERIFY(false == RingBuf_get(&rb2, &el));
    /* buffer is fully usable after process_all() */
    VERIFY(true == RingBuf_put(&rb2, 0x44U));
    VERIFY(true == RingBuf_get(&rb2, &el));
    VERIFY(0x44U == el);
}

TEST("smallest useful buffer (sto_len 2) holds exactly one element") {
    RingBufElement sto[2];
    RingBuf rbs;
    RingBufElement el;
    RingBuf_ctor(&rbs, sto, ARRAY_NELEM(sto));
    VERIFY(RingBuf_num_free(&rbs) == 1U);
    VERIFY(false == RingBuf_get(&rbs, &el));
    for (RingBufCtr i = 0U; i < 5U; ++i) { /* indexes wrap every 2 */
        VERIFY(true == RingBuf_put(&rbs, (RingBufElement)(0xA0U + i)));
        VERIFY(RingBuf_num_free(&rbs) == 0U);
        VERIFY(false == RingBuf_put(&rbs, 0xEEU)); /* full */
        VERIFY(true == RingBuf_get(&rbs, &el));
        VERIFY((RingBufElement)(0xA0U + i) == el);
        VERIFY(false == RingBuf_get(&rbs, &el)); /* empty */
        VERIFY(RingBuf_num_free(&rbs) == 1U);
    }
    VERIFY(true == RingBuf_put(&rbs, 0xB1U));
    rec_cnt = 0U;
    RingBuf_process_all(&rbs, &rec_handler);
    VERIFY(1U == rec_cnt);
    VERIFY(0xB1U == rec_data[0]);
    VERIFY(RingBuf_num_free(&rbs) == 1U);
}

TEST("degenerate buffer (sto_len 1) has zero capacity") {
    RingBufElement sto[1] = { 0x5AU };
    RingBuf rbs;
    RingBufElement el = 0xC3U;
    RingBuf_ctor(&rbs, sto, ARRAY_NELEM(sto));
    VERIFY(RingBuf_num_free(&rbs) == 0U);
    VERIFY(false == RingBuf_put(&rbs, 0xEEU));
    VERIFY(0x5AU == sto[0]); /* storage not written */
    VERIFY(false == RingBuf_get(&rbs, &el));
    VERIFY(0xC3U == el);
    rec_cnt = 0U;
    RingBuf_process_all(&rbs, &rec_handler);
    VERIFY(0U == rec_cnt);
}

#ifdef Q_HOST /* 64KB of storage: too big for the small target MCU */
TEST("largest buffer (sto_len 0xFFFF): indexes wrap at RingBufCtr max") {
    static RingBufElement sto[0xFFFFU];
    RingBuf rbl;
    RingBufElement el;
    RingBufCtr const cap = (RingBufCtr)(ARRAY_NELEM(sto) - 1U); /* 0xFFFE */
    RingBuf_ctor(&rbl, sto, (RingBufCtr)ARRAY_NELEM(sto));
    VERIFY(RingBuf_num_free(&rbl) == cap);
    for (uint32_t i = 0U; i < cap; ++i) {
        VERIFY(true == RingBuf_put(&rbl, (RingBufElement)i));
    }
    VERIFY(RingBuf_num_free(&rbl) == 0U);
    VERIFY(false == RingBuf_put(&rbl, 0xEEU));
    for (uint32_t i = 0U; i < cap - 1U; ++i) {
        VERIFY(true == RingBuf_get(&rbl, &el));
        VERIFY((RingBufElement)i == el);
    }
    /* head == 0xFFFE, tail == 0xFFFD; next puts wrap head over 0xFFFF */
    VERIFY(RingBuf_num_free(&rbl) == cap - 1U);
    VERIFY(true == RingBuf_put(&rbl, 0x77U)); /* head 0xFFFE -> 0 */
    VERIFY(true == RingBuf_put(&rbl, 0x88U)); /* head 0 -> 1 */
    VERIFY(RingBuf_num_free(&rbl) == cap - 3U);
    VERIFY(true == RingBuf_get(&rbl, &el));   /* tail 0xFFFD -> 0xFFFE */
    VERIFY((RingBufElement)(cap - 1U) == el);
    VERIFY(true == RingBuf_get(&rbl, &el));   /* tail 0xFFFE -> 0 */
    VERIFY(0x77U == el);
    VERIFY(true == RingBuf_get(&rbl, &el));   /* tail 0 -> 1 */
    VERIFY(0x88U == el);
    VERIFY(false == RingBuf_get(&rbl, &el));
    VERIFY(RingBuf_num_free(&rbl) == cap);
}
#endif /* Q_HOST */

} /* TEST_GROUP() */

static void rb_handler(RingBufElement const el) {
    VERIFY(test_data[test_idx] == el);
    ++test_idx;
}

static void rec_handler(RingBufElement const el) {
    VERIFY(rec_cnt < ARRAY_NELEM(rec_data));
    rec_data[rec_cnt] = el;
    ++rec_cnt;
}
