# What is it?
"Lock-Free Ring Buffer" (LFRB) is a minimal, customizable implementation
of a ring buffer (a.k.a. circular buffer) in C, specifically suitable
for embedded systems.


# Lock-Free Restrictions
The ring buffer does not require any "locking" (mutual exclusion
mechanism) as long as the following restrictions are met:

1. Only one thread/interrupt can produce data into the ring buffer
2. Only one thread/interrupt can consume data from the ring buffer

In other words, a given LFRB can be used only by one pair of producer/
consumer threads/interrupts. Fortunately, this is the most frequently
encountered scenario.

There are also some additional assumptions about the "atomicity" of the
LFRB counters, which are spelled out in the [ring_buf.h](src/ring_buf.h)
header file.

# Code Structure
The ring buffer implementation consists of two files located in the
src directory:

- [ring_buf.h](src/ring_buf.h)  - contains the interface
- [ring_buf.c](src/ring_buf.c)  - contains the implementation

The ring buffer holds elements of they type RingBufElement, which
can be customized (typically `uint8_t`, `uint16_t`, `uint32_t`, `float`, 
`void*` (pointers), etc.)


# Test/Example of Use
The directory `test` contains a very "Simple Unit Testing (SUTEST)" framework
for unit testing, consisting of the files:

- [sutest.h](test/sutest.h)  - SUTEST interface
- [sutest.c](test/sutest.c)  - SUTEST implementation

The SUTEST framework is used to test the LFRB. Specifically, the tests
and examples of use of LFRB are located in the file:

- [test_ring_buf.c](test/test_ring_buf.c) - example of use and tests the LFRB.


# Licensing
The LFRB is [licensed](LICENSE.txt) under the MIT open source license.


# Comments/Discussion
If you'd like to discuss LFRB or related subjects, plese use the
["Issues" tab](https://github.com/QuantumLeaps/lock-free-ring-buffer/issues).


# Contact Information
[state-machine.com](https://www.state-machine.com)
