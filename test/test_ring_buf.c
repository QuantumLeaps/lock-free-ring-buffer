#include <stdint.h>
#include <stdbool.h>

#include "ring_buf.h"
#include "sutest.h"

#include <stdio.h>


RingBufElement buf[8];
RingBuf rb;

static void rb_handler(RingBufElement const el);

static RingBufElement test_data[] = {
    0xAAU,
    0xBBU,
    0xCCU,
    0xDDU
};
static RingBufCtr test_idx;

void TEST_onRun(void) {
    RingBufElement el;
    RingBufCtr i;

    RingBuf_ctor(&rb, buf, ARRAY_LEN(buf));

    TEST("RingBuf_num_free");
    EXPECT(RingBuf_num_free(&rb) == ARRAY_LEN(buf) - 1U);

    TEST("RingBuf_put 3");
    RingBuf_put(&rb, 0xAAU);
    RingBuf_put(&rb, 0xBBU);
    RingBuf_put(&rb, 0xCCU);
    EXPECT(RingBuf_num_free(&rb) == ARRAY_LEN(buf) - 1U - 3U);

    TEST("RingBuf_get");
    EXPECT(true == RingBuf_get(&rb, &el));
    EXPECT(0xAAU == el);
    EXPECT(true == RingBuf_get(&rb, &el));
    EXPECT(0xBBU == el);
    EXPECT(true == RingBuf_get(&rb, &el));
    EXPECT(0xCCU == el);
    EXPECT(false == RingBuf_get(&rb, &el));

    TEST("RingBuf_process_all test_data");
    for (i = 0U; i < ARRAY_LEN(test_data); ++i) {
        RingBuf_put(&rb, test_data[i]);
    }
    test_idx = 0U;
    RingBuf_process_all(&rb, &rb_handler);
    EXPECT(RingBuf_num_free(&rb) == ARRAY_LEN(buf) - 1U);
}

static void rb_handler(RingBufElement const el) {
    EXPECT(test_data[test_idx] == el);
    ++test_idx;
}

