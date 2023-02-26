## Brought to you by:
[![Quantum Leaps](https://www.state-machine.com/attachments/logo_ql_400.png)](https://www.state-machine.com)
<hr>

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/QuantumLeaps/Embedded-Test)](https://github.com/QuantumLeaps/Embedded-Test/releases/latest)
[![GitHub](https://img.shields.io/github/license/QuantumLeaps/Embedded-Test)](https://github.com/QuantumLeaps/Embedded-Test/blob/master/LICENSE)


# ET - The Super-Simple Embedded Test
[Embedded Test (ET)](file:///C:/GitHub/Embedded-Test) is a super-simple, no-nonsense unit test framework in C for **bare metal** embedded systems without any dependencies on standard libraries or header files. ET is as simple as possible, but not simpler.

<p align="center"><img src="../img/logo_et-chip.png"/></p>

- no `printf()/sprintf()`, `malloc()`, `longjmp()`
- no `<stdio.h>` or any other standard header files
- no "test runners" or code-generating scripts to run in order to test code
- no myriads of various "test assertions" (just common `VERIFY()`)

> **NOTE**<br>
In spite of the completely bare-metal design, ET can execute most tests (those without "mocking") from the [Unity framework](https://github.com/ThrowTheSwitch/Unity) as well as from the book ["Test-Driven Development for Embedded C" by James W. Grenning](https://wingman-sw.com/test-driven-development-for-embedded-c-book). For instance, the [LedDriver example](examples/leddriver) demonstrates some tests from the Embedded-TDD book.

