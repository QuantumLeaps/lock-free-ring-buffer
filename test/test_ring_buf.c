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
#include "sutest.h"

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

void setUp(void) {
}

void tearDown(void) {
}

void TEST_onRun(void) {

    RingBuf_ctor(&rb, buf, ARRAY_LEN(buf));

    TEST("RingBuf_num_free");
    {
        EXPECT(RingBuf_num_free(&rb) == ARRAY_LEN(buf) - 1U);
    }

    TEST("RingBuf_put 3");
    {
        RingBuf_put(&rb, 0xAAU);
        RingBuf_put(&rb, 0xBBU);
        RingBuf_put(&rb, 0xCCU);
        EXPECT(RingBuf_num_free(&rb) == ARRAY_LEN(buf) - 1U - 3U);
    }

    TEST("RingBuf_get");
    {
        RingBufElement el;
        EXPECT(true == RingBuf_get(&rb, &el));
        EXPECT(0xAAU == el);
        EXPECT(true == RingBuf_get(&rb, &el));
        EXPECT(0xBBU == el);
        EXPECT(true == RingBuf_get(&rb, &el));
        EXPECT(0xCCU == el);
        EXPECT(false == RingBuf_get(&rb, &el));
    }

    TEST("RingBuf_process_all test_data");
    {
        for (RingBufCtr i = 0U; i < ARRAY_LEN(test_data); ++i) {
            RingBuf_put(&rb, test_data[i]);
        }
        test_idx = 0U;
        RingBuf_process_all(&rb, &rb_handler);
        EXPECT(RingBuf_num_free(&rb) == ARRAY_LEN(buf) - 1U);
    }
}

static void rb_handler(RingBufElement const el) {
    EXPECT(test_data[test_idx] == el);
    ++test_idx;
}

