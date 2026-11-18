##############################################################################
# Product: Makefile for ET (embedded test) for C CUT on the HOST
# GitHub: https://github.com/QuantumLeaps/ET
#
#                    Q u a n t u m  L e a P s
#                    ------------------------
#                    Modern Embedded Software
#
# Copyright (C) 2005 Quantum Leaps, <state-machine.com>.
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
##############################################################################
#
# examples of invoking this Makefile:
# make         # build, run the tests, and report the code coverage
# make norun   # only build but not run the tests
# make clean   # cleanup the build and the coverage reports
#
# The tests are always built with structural coverage instrumentation.
# After the run, gcov reports the coverage of the code under test (CUT,
# ring_buf.c) in the file ring_buf.cov (also shown on the console) and
# in the annotated source file ring_buf.c.gcov.
#
# NOTE:
# To use this Makefile on Windows, you will need the GNU make utility, which
# is included in the QTools collection for Windows, see:
#    https://github.com/QuantumLeaps/qtools
#

#-----------------------------------------------------------------------------
# project name:
PROJECT := test_ring_buf
ET_DIR  := ../et

# code under test (CUT), whose coverage is reported (without extension)
CUT     := ring_buf

# list of all source directories used by this project
VPATH := . \
	../src \
	$(ET_DIR)

# list of all include directories needed by this project
INCLUDES := -I. \
	-I../src \
	-I$(ET_DIR)

#-----------------------------------------------------------------------------
# project files:
#

# C source files...
C_SRCS := \
	ring_buf.c \
	test_ring_buf.c \
	et.c \
	et_host.c

# C++ source files...
CPP_SRCS :=

LIB_DIRS :=
LIBS     :=

# defines...
DEFINES  :=

#============================================================================
# Typically you should not need to change anything below this line

#-----------------------------------------------------------------------------
# GNU toolset:
#
# NOTE:
# GNU toolset (MinGW) is included in the QTools collection for Windows, see:
#     https://www.state-machine.com/qtools
# It is assumed that %QTOOLS%\bin directory is added to the PATH
#
CC    := gcc
CPP   := g++
LINK  := gcc    # for C programs
#LINK  := g++   # for C++ programs

#-----------------------------------------------------------------------------
# basic utilities (depends on the OS this Makefile runs on):
#
ifeq ($(OS),Windows_NT)
	MKDIR      := mkdir
	RM         := rm
	CAT        := type
	TARGET_EXT := .exe
else ifeq ($(OSTYPE),cygwin)
	MKDIR      := mkdir -p
	RM         := rm -f
	CAT        := cat
	TARGET_EXT := .exe
else
	MKDIR      := mkdir -p
	RM         := rm -f
	CAT        := cat
	TARGET_EXT :=
endif

#-----------------------------------------------------------------------------
# structural coverage options, selected by the compiler version (not the OS):
# condition coverage (-fcondition-coverage, gcov -g) requires gcc 14 or newer
#
GCC_MAJOR := $(firstword $(subst ., ,$(shell $(CC) -dumpversion)))
ifeq ($(filter 1 2 3 4 5 6 7 8 9 10 11 12 13,$(GCC_MAJOR)),)
	COV        := gcov -abcgum
	CODE_COV   := --coverage -fcondition-coverage  # gcc 14 or newer
else
	COV        := gcov -b
	CODE_COV   := -fprofile-arcs -ftest-coverage   # old gcc (version < 14)
endif

#-----------------------------------------------------------------------------
# build options...

BIN_DIR := build_host

CFLAGS  := -c -g -O -fno-pie -std=c11 -pedantic -Wall -Wextra -W \
	$(CODE_COV) $(INCLUDES) $(DEFINES) -DQ_HOST

CPPFLAGS := -c -g -O -fno-pie -std=c++11 -pedantic -Wall -Wextra \
	-fno-rtti -fno-exceptions \
	$(CODE_COV) $(INCLUDES) $(DEFINES) -DQ_HOST

LINKFLAGS := $(CODE_COV)

# -no-pie (gcc 6 or newer) matches the -fno-pie compile option
ifndef GCC_OLD
	LINKFLAGS += -no-pie
endif

#-----------------------------------------------------------------------------
C_OBJS       := $(patsubst %.c,%.o,   $(C_SRCS))
CPP_OBJS     := $(patsubst %.cpp,%.o, $(CPP_SRCS))

TARGET_EXE   := $(BIN_DIR)/$(PROJECT)$(TARGET_EXT)
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o,%.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o,%.d, $(CPP_OBJS_EXT))

#-----------------------------------------------------------------------------
# rules
#

.PHONY : norun clean show

ifeq ($(MAKECMDGOALS),norun)
all : $(TARGET_EXE)
norun : all
else
all : $(TARGET_EXE) run
endif

$(TARGET_EXE) : $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

run : $(TARGET_EXE)
	-$(RM) $(BIN_DIR)/*.gcda
	$(TARGET_EXE)
	$(COV) -o $(BIN_DIR) $(CUT) > $(CUT).cov
	$(CAT) $(CUT).cov

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

# create BIN_DIR and include dependencies only if needed
ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),show)
     ifneq ($(MAKECMDGOALS),debug)
ifeq ("$(wildcard $(BIN_DIR))","")
$(shell $(MKDIR) $(BIN_DIR))
endif
-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)
     endif
  endif
endif

clean :
	-$(RM) -rf $(BIN_DIR)
	-$(RM) -f *.gcov $(CUT).cov

show :
	@echo PROJECT      = $(PROJECT)
	@echo TARGET_EXE   = $(TARGET_EXE)
	@echo VPATH        = $(VPATH)
	@echo C_SRCS       = $(C_SRCS)
	@echo CPP_SRCS     = $(CPP_SRCS)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo C_OBJS_EXT   = $(C_OBJS_EXT)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo CPP_OBJS_EXT = $(CPP_OBJS_EXT)
	@echo LIB_DIRS     = $(LIB_DIRS)
	@echo LIBS         = $(LIBS)
	@echo DEFINES      = $(DEFINES)
	@echo CUT          = $(CUT)
	@echo COV          = $(COV)
	@echo CODE_COV     = $(CODE_COV)

