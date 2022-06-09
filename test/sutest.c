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

#include "sutest.h"

#include <stdio.h>  /* for printf() */
#include <stdlib.h> /* for exit() */

/* is this Windows OS? */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* for DebugBreak() */
#define DBG_BREAK() DebugBreak()

#else /* not Windows */

#define DBG_BREAK() ((void)0)

#endif

/*..........................................................................*/
static int l_test_count;

/*..........................................................................*/
void TEST(char const *title) {
    if (l_test_count > 0) {
        tearDown();
        printf(" PASSED\n");
    }
    printf("test[%2d] | \"%s\"", l_test_count, title);
    setUp();
    printf("%s", "...");
    ++l_test_count;
}

/*..........................................................................*/
void TEST_fail_(char const *cond, char const *file, int line) {
    printf(" FAILED in %s:%d\n"
        "expected : \"%s\"\n"
        "-------------------------------------------------------\n"
        "%d test(s)\n"
        "FAILED\n",
        file, line, cond, l_test_count);

    DBG_BREAK();
    exit(l_test_count);
}

/*..........................................................................*/
int main(void) {
    printf("\n%s\n%s\n",
           "\"SUTEST\": Simple Unit Testing (github.com/QuantumLeaps)",
           "-------------------------------------------------------");

    TEST_onRun();

    if (l_test_count > 0) {
        tearDown();
        printf("%s\n", " PASSED");
    }
    printf("-------------------------------------------------------\n"
        "%d test(s)\nOK\n", l_test_count);

    return 0; /* success */
}
