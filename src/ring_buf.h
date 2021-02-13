/**
* @file
* @brief Lock-free ring buffer
*/
/*****************************************************************************
* Last updated on  2021-02-12
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

typedef uint8_t RingBufElement;
typedef uint16_t RingBufCtr;

typedef struct {
    RingBufElement *buf; /*!< pointer to the start of the ring buffer */
    RingBufCtr     end;  /*!< offset of the end of the ring buffer */
    RingBufCtr     head; /*!< offset to where next byte will be inserted */
    RingBufCtr     tail; /*!< offset of where next byte will be extracted */
} RingBuf;

typedef void (*RingBufHandler)(RingBufElement const el);

void RingBuf_ctor(RingBuf * const me,
                  RingBufElement sto[], RingBufCtr sto_len);
bool RingBuf_put(RingBuf * const me, RingBufElement const el);
bool RingBuf_get(RingBuf * const me, RingBufElement *pel);
void RingBuf_process_all(RingBuf * const me, RingBufHandler handler);
RingBufCtr RingBuf_num_free(RingBuf * const me);

#endif /* RING_BUF */
