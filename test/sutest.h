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
* @brief Simple Unit Testing ("SUTEST") framework
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

void setUp(void);
void tearDown(void);

#ifdef __cplusplus
}
#endif

#endif /* SUTEST_H */
