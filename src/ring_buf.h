/**
* @file
* @brief Lock-free ring buffer
*/
/*****************************************************************************
* Last updated on  2022-02-22
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2021 Quantum Leaps, LLC. All rights reserved.
*
* This software is licensed under the following open source MIT license:
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*
* Contact information:
* <www.state-machine.com>
* <info@state-machine.com>
*/
#ifndef RING_BUF_H
#define RING_BUF_H

/*
* ASSUMPTION:
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

typedef uint8_t RingBufElement;

typedef struct {
    RingBufElement *buf; /*!< pointer to the start of the ring buffer */
    RingBufCtr     end;  /*!< offset of the end of the ring buffer */
    RingBufCtr volatile head; /*!< offset to where next el. will be inserted */
    RingBufCtr volatile tail; /*!< offset of where next el. will be extracted */
} RingBuf;

typedef void (*RingBufHandler)(RingBufElement const el);

void RingBuf_ctor(RingBuf * const me,
                  RingBufElement sto[], RingBufCtr sto_len);
bool RingBuf_put(RingBuf * const me, RingBufElement const el);
bool RingBuf_get(RingBuf * const me, RingBufElement *pel);
void RingBuf_process_all(RingBuf * const me, RingBufHandler handler);
RingBufCtr RingBuf_num_free(RingBuf * const me);

#endif /* RING_BUF */
