## Brought to you by:
[![Quantum Leaps](https://www.state-machine.com/attachments/logo_ql_400.png)](https://www.state-machine.com)
<hr>

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/QuantumLeaps/lock-free-ring-buffer)](https://github.com/QuantumLeaps/lock-free-ring-buffer/releases/latest)
[![GitHub](https://img.shields.io/github/license/QuantumLeaps/lock-free-ring-buffer)](https://github.com/QuantumLeaps/lock-free-ring-buffer/blob/master/LICENSE)

# What is it?
"Lock-Free Ring Buffer" (LFRB) is a minimal, customizable implementation
of a ring buffer (a.k.a. circular buffer) in C, specifically suitable
for embedded systems, such as single-core microcontrollers with:
- ARM Cortex-M CPU
- MSP430 CPU
- PIC24/dsPIC CPU
- other similar 16/32-bit CPUs

> **NOTE** The presented LFRB implementation is intended for deeply embedded
systems, such as single-core ARM Cortex-M MCUs. The presented implementation
might not be appropriate for complex multi-core SoCs with cache memory, etc.
Please refer to the literature, for example:
[<i>Correct and Efficient Bounded FIFO Queues</i>](https://www.irif.fr/~guatto/papers/sbac13.pdf).

# Lock-Free Restrictions
The ring buffer does not require any "locking" (mutual exclusion
mechanism) as long as the following restrictions are met:

1. Only one thread/interrupt can produce data into the ring buffer
2. Only one thread/interrupt can consume data from the ring buffer
3. Both the producer and consumer run on the same CPU
4. The system has strong memory consistency
5. Additional assumptions about the "atomicity" of the LFRB counters
are spelled out in the [ring_buf.h](src/ring_buf.h) header file

In other words, LFRB is a **single-producer, single-consumer FIFO**.
Fortunately, this is the most frequently encountered scenario in deeply
embedded applications.

# Code Structure
The ring buffer implementation consists of two files located in the
src directory:

- [ring_buf.h](src/ring_buf.h)  - contains the interface
- [ring_buf.c](src/ring_buf.c)  - contains the implementation

The ring buffer holds elements of they type RingBufElement, which
can be customized (typically `uint8_t`, `uint16_t`, `uint32_t`, `float`,
`void*` (pointers), etc.)


# Test/Example of Use
The directory `ET` contains the
[<b>Embedded Test (ET)</b>](https://github.com/QuantumLeaps/Embedded-Test)
super-simple testing framework. ET is used to test the LFRB.

<p align="center">
<a title="ET on GitHub" href="https://github.com/QuantumLeaps/Embedded-Test"><img src="img/logo_et-chip.png"/></a>
</p>

Specifically, the ET tests and examples of use of LFRB are located in the file:

- [test_ring_buf.c](test/test_ring_buf.c) - example of use and tests the LFRB.


# Licensing
The LFRB is [licensed](LICENSE) under the MIT open source license.


# Comments/Discussion
If you'd like to discuss LFRB or related subjects, plese use the
["Issues" tab](https://github.com/QuantumLeaps/lock-free-ring-buffer/issues).

# How to Help this Project?
Please feel free to clone, fork, and make pull requests to improve LFRB.
If you like this project, please give it a star
(in the upper-right corner of your browser window):

<p align="center"><img src="img/github-star.jpg"/></p>
