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
/*!
* @date Last updated on: 2022-06-09
*
* @file
* @brief Lock-free ring buffer
*/
#ifndef RING_BUF_H
#define RING_BUF_H

/*! Ring buffer counter/index
*
* @attention
* The following RingBufCtr type is assumed to be "atomic" in a target CPU,
* meaning that the CPU needs to write the whole RingBufCtr in one machine
* instruction. An example of violating this assumption would be an 8-bit
* CPU, which writes uint16_t (2-bytes) in 2 machine instructions. For such
* 8-bit CPU, the maximum size of RingBufCtr would be uint8_t (1-byte).
*
* Another case of violating the "atomic" writes to RingBufCtr type would
* be misalignment of a RingBufCtr variable in memory, which could cause
* the compiler to generate multiple instructions to write a RingBufCtr value.
* Therefore, it is further assumed that all RingBufCtr variables in the
* following RingBuf struct *are* correctly aligned for "atomic" access.
* In practice, most C compilers should provide such natural alignment
* (by inserting some padding into the struct, if necessary).
*/
typedef uint16_t RingBufCtr;

/*! Ring buffer element type
*
* @details
* The type of the ring buffer elements is not critical for the lock-free
* operation and does not need to be "atomic". For example, it can be
* an integer type (uint16_t, uint32_t, uint64_t), a pointer type,
* or even a struct type. However, the bigger the type the more RAM is
* needed for the ring buffer and more CPU cycles are needed to copy it
* into and out of the buffer memory.
*/
typedef uint8_t RingBufElement;

/*! Ring buffer struct */
typedef struct {
    RingBufElement *buf; /*!< pointer to the start of the ring buffer */
    RingBufCtr     end;  /*!< offset of the end of the ring buffer */
    RingBufCtr volatile head; /*!< offset to where next el. will be inserted */
    RingBufCtr volatile tail; /*!< offset of where next el. will be removed */
} RingBuf;

void RingBuf_ctor(RingBuf * const me,
                  RingBufElement sto[], RingBufCtr sto_len);
RingBufCtr RingBuf_num_free(RingBuf * const me);
bool RingBuf_put(RingBuf * const me, RingBufElement const el);
bool RingBuf_get(RingBuf * const me, RingBufElement *pel);

/*! Ring buffer callback function for RingBuf_process_all()
*
* @details
* The callback processes one element and runs in the context of
* RingBuf_process_all().
*/
typedef void (*RingBufHandler)(RingBufElement const el);

void RingBuf_process_all(RingBuf * const me, RingBufHandler handler);

#endif /* RING_BUF_H */
