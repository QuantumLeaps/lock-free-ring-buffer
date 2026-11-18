# Unit-test strengthening: QuantumLeaps/lock-free-ring-buffer

- Repo: https://github.com/QuantumLeaps/lock-free-ring-buffer, commit `0d6354a` ("4.0.0")
- Code under test: `src/ring_buf.c`, `src/ring_buf.h`. **I did not change either file.**
- The only file I changed is `test/test_ring_buf.c` (see `tests.patch`).
- Host toolchain: gcc 13.3.0 (Ubuntu 24.04), GNU make, gcovr 8.8. The repo's own `test/Makefile` was used unchanged: `make` to run the tests and `make GCOV=1` for coverage (flags `-g -O -std=c11 -pedantic -Wall -Wextra -W -DQ_HOST`).

## 1. Baseline (the existing tests, unchanged)

`cd test && make` → 4 tests, `OK`, exit code 0 (`coverage/baseline-run.txt`).

Coverage of `src/ring_buf.c` (`make GCOV=1`, then `gcovr -r .. --filter ../src/ring_buf.c --txt-metric line --txt-metric branch build_host`):

| | Lines | Branches |
|---|---|---|
| Baseline | 40/44 = **90 %** | 11/16 = **68 %** |

Lines that never ran (from `coverage/baseline-ring_buf.c.gcov`):

| Line | Code | What is missing |
|---|---|---|
| 49 | `head = 0U;` in `RingBuf_put` | the head index never wraps |
| 52 | `if (head != tail)` in `RingBuf_put`: false branch | put on a full buffer is never run |
| 69 | `tail = 0U;` in `RingBuf_get` | the tail index never wraps |
| 86 | `return tail - head - 1U;` in `RingBuf_num_free` | `head < tail` (wrapped contents) is never run |
| 101 | `tail = 0U;` in `RingBuf_process_all` | the tail index never wraps |

The existing tests also never check that `RingBuf_put` returns `true`, and they share one global buffer, so each test depends on the ones before it.

## 2. Added tests

I added 14 tests in the existing ET style (`TEST("...") { VERIFY(...); }` inside the same `TEST_GROUP`). Each new test calls `RingBuf_ctor()` on its own buffer (`rb2`/`buf2[8]` or a local one), so it does not depend on any other test. I did not modify the 4 original tests. Helpers:

- `rb_advance()`: moves head and tail of an empty buffer forward with put/get pairs, using only the public API. This places the indexes at any start position.
- `rec_handler()`: records the elements that `RingBuf_process_all()` passes to it.

| # | Test | Boundary / behaviour checked |
|---|---|---|
| 5 | ctor: new buffer is empty with capacity sto_len - 1 | initial state; a failing get leaves `*pel` unchanged |
| 6 | put: buffer fills completely with sto_len - 1 elements | **filling completely**; `num_free` checked after every put |
| 7 | put: on a full buffer returns false and keeps the contents | **put on full** (twice); the rejected element is not stored; FIFO order kept |
| 8 | put: on a full buffer with wrapped head returns false | full state when head has wrapped (`head == tail-1`, head < tail) |
| 9 | get: on an empty buffer returns false and leaves output untouched | **get on empty** after draining, run twice |
| 10 | put/get: exactly one element | one element; element value 0x00 |
| 11 | num_free: correct for every fill level at every start index | all 8 start indexes × 8 fill levels, covering `head==tail`, `head>tail` and `head<tail` |
| 12 | put/get: head and tail wrap around over many cycles | **wrap-around**: 70 000 cycles × 3 puts/3 gets (210 000 ops, more than 2^16). 3 and 8 share no factor, so every index position is visited. FIFO data checked; the 8-bit element values also wrap |
| 13 | put/get: full/empty cycles with wrap-around | 16 cycles of fill-to-full/drain-to-empty, with the indexes moving each cycle |
| 14 | process_all: on an empty buffer does not call the handler | **process_all on empty**, both fresh and with indexes at `end-1` |
| 15 | process_all: full buffer with wrapped indexes, in FIFO order | the handler sees every element in order across the wrap; the buffer is still usable afterwards |
| 16 | smallest useful buffer (sto_len 2) holds exactly one element | **smallest useful size**: capacity 1, full/empty, index wrap every 2 operations, process_all |
| 17 | degenerate buffer (sto_len 1) has zero capacity | sto_len 1: put always fails, storage is not written, get fails, the handler is never called |
| 18 | largest buffer (sto_len 0xFFFF): indexes wrap at RingBufCtr max | the `uint16_t` index limit: fill 0xFFFE elements, put on full, head 0xFFFE→0, tail 0xFFFE→0. Guarded by `#ifdef Q_HOST` because it needs 64 KB of RAM |

## 3. Final results

`cd test && make GCOV=1` → **18 tests, 0 skipped, `OK`**, exit code 0, no compiler warnings. The full console output is in `test-output.txt`.

| | Tests | Lines | Branches |
|---|---|---|---|
| Before | 4 | 40/44 = 90 % | 11/16 = 68 % |
| **After** | **18** | **44/44 = 100 %** | **16/16 = 100 %** |

- **Cross-check at -O0:** the project builds at `-O`, which can merge branches. So I also built with `-O0` and coverage. The result was the same: baseline 41/46 lines and 11/16 branches; final 46/46 lines and 16/16 branches. `gcovr --decisions` also reports 16/16 decisions.
- **Target build (compile and link only):** with `arm-none-eabi-gcc` 13.2 and the repo's `test/nucleo-c031c6.mak` (`GNU_ARM=/usr USB=<dummy>`), the new test file builds for Cortex-M0+ (text 8768 B, bss 2160 B). Because of the `Q_HOST` guard, the 64 KB test is left out of this build. I did not run it on hardware.

### Mutation check (do the new tests catch real faults?)

100 % coverage alone does not show that the tests check results. So I made 11 single-change mutants of a *copy* of `ring_buf.c` and ran the old and new test suites against each one. The script is `coverage/mutate.sh` and the results are in `coverage/mutation-results.txt`.

| Mutant | Old suite | New suite |
|---|---|---|
| put: no head wrap | survived | killed |
| put: no full check | survived | killed |
| put: off-by-one full check | survived | killed |
| get: no tail wrap | survived | killed |
| get: no empty check | killed | killed |
| num_free: empty returns `end` | killed | killed |
| num_free head<tail: missing `-1` | survived | killed |
| num_free head>tail: missing `-1` | killed | killed |
| num_free: `<` swapped to `>` | killed | killed |
| process_all: wrong tail wrap | survived | killed |
| process_all: tail not stored | killed | killed |

The old suite killed 5 of 11 mutants; the new suite kills all 11.

## 4. Bugs found

**None.** All 14 new tests pass against the unmodified code, so there is no failing test and no fix patch.

Observations that are not bug reports (I could not show any of them failing with a test):

- `sto_len == 0` is not checked. `RingBuf_put` would then write `sto[0]`. `ring_buf.c` does not use the repo's `dbc_assert.h`. A precondition assertion such as `sto_len >= 2` in `RingBuf_ctor` would make this misuse fail early. That would be a design change, not a fix.
- The usable capacity is `sto_len - 1`, because one slot is kept empty to tell "full" from "empty". Tests 6, 16 and 17 pin this down. `sto_len == 1` gives a buffer that can never hold an element.
- These host tests are single-threaded. They cannot check the lock-free / memory-order properties (`acquire`/`release` between producer and consumer). That would need a two-thread stress test (for example pthreads plus ThreadSanitizer) or a test on the target with an ISR as producer.

## 5. Reproduce

```sh
git clone https://github.com/QuantumLeaps/lock-free-ring-buffer && cd lock-free-ring-buffer
git apply /path/to/tests.patch
cd test && make GCOV=1
gcovr -r .. --filter ../src/ring_buf.c --txt-metric line --txt-metric branch build_host
```

## Deliverables (`/mnt/session/outputs/`)

- `tests.patch`: `git diff` of the change (only `test/test_ring_buf.c`); `git apply --check` passes on a clean clone
- `test_ring_buf.c`: the complete updated test file
- `test-output.txt`: console output of the final `make GCOV=1` build and test run
- `report.md`: this report
- `coverage/`: baseline and final gcovr summaries and annotated `ring_buf.c.gcov` files, the baseline run log, and the mutation script with its results
