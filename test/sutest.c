/**
* @file
* @brief Simple Unit Testing ("SUTEST") framework
*/
/*****************************************************************************
* Last updated on  2022-06-08
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
#include "sutest.h"

#include <stdio.h>  /* for printf() */
#include <stdlib.h> /* for exit() */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* for DebugBreak() */
#define DBG_BREAK() DebugBreak()

#else

#define DBG_BREAK() ((void)0)

#endif

/*..........................................................................*/
static int l_test_count;

/*..........................................................................*/
void TEST(char const *title) {
    if (l_test_count > 0) {
        printf(" PASSED\n");
    }
    printf("test: \"%s\" ...", title);
    ++l_test_count;
}

/*..........................................................................*/
void TEST_fail_(char const *cond, char const *file, int line) {
    printf(" FAILED in %s:%d\n"
        "%s\n"
        "---------------------------------------------\n"
        "%d test(s)\n"
        "FAILED\n",
        file, line, cond, l_test_count);

    DBG_BREAK();
    exit(l_test_count);
}

/*..........................................................................*/
int main(void) {
    printf("\n%s\n", "Simple Unit Internal Testing -- SUTEST");

    TEST_onRun();

    if (l_test_count > 0) {
        printf("%s\n", " PASSED");
    }
    printf("---------------------------------------------\n"
        "%d test(s)\nOK\n", l_test_count);

    return 0; /* success */
}
