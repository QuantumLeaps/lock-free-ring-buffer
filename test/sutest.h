/**
* @file
* @brief Simple Unit Testing ("SUTEST") framework
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
#ifndef SUTEST_H
#define SUTEST_H

/* macro to check an expectation */
#define EXPECT(cond_) ((cond_) \
        ? (void)0 : TEST_fail_(#cond_, __FILE__, __LINE__))
#define ARRAY_LEN(a_) (sizeof(a_)/sizeof(a_[0]))

#ifdef __cplusplus
extern "C" {
#endif

void TEST(char const *title);
void TEST_fail_(char const *cond, char const *module, int line);
void TEST_onRun(void); /* user-defined callback to run the tests */

#ifdef __cplusplus
}
#endif

#endif /* SUTEST_H */
