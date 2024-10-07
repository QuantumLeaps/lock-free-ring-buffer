# Research

While a variable that is a single instruction load/store operations on a processor does provide some level of atomicity, there are a few additional considerations:

> Atomicity vs. Memory Ordering

**Atomicity:** Ensures that a read or write operation completes without interruption. For up to uint32_t variables, this is generally true on ARM Cortex-M processors.

**Memory Ordering:** Ensures the correct sequence of operations across multiple threads. This is where std::memory_order comes into play.

**C Sequence Points:** Define points in the code where all side effects of previous evaluations are complete, and no side effects of subsequent evaluations have started.

While sequence points help with ordering within a single thread, they don’t provide guarantees for memory visibility across multiple threads.

> Key reasons why memory ordering is considered relevant:

**Reordering by the Compiler:**
The C and C++ standards allow the compiler to reorder instructions for optimization unless explicitly told not to. For example, without memory barriers or memory ordering, the compiler could:

- Write the head pointer (indicating that new data is available) before the data itself is written to the buffer.
- Read the head pointer before reading the data, leading to the consumer seeing stale or partially written data.
Even if write_index and buffer[] are uint32_t, this reordering could result in the producer marking the data as ready (by updating the write_index) before the data itself has been fully written.

**Reordering by the Processor:**
While a Cortex-M4 has a simpler memory model than modern multi-core processors, it's still possible for the memory subsystem to perform some reordering, particularly with peripheral memory accesses.

**Interrupts:**
ISR could preempt the main thread at any point, including in the middle of a read-modify-write sequence, leading to inconsistent states if proper synchronization isn't enforced. If the write_index is being updated at the same time the main thread reads it, the value read by the main thread could be incomplete or inconsistent.

**Sequence Points in C:**
C sequence points (or in C++, sequencing rules) provide guarantees about when side effects (like memory writes) occur relative to other operations within the same thread. However, these guarantees do not extend across multiple threads or between an interrupt handler and the main thread. This is why memory ordering is required to ensure that changes made by one thread (or ISR) are visible to another in the correct order.

Even though variables up to unin32_t are single instruction load/store, without explicit memory ordering, there's no guarantee that:
- The producer writes data to the buffer before updating the write_index.
- The consumer reads the buffer data only after it sees the updated write_index.

Use of acquire-release semantics ensures that:
- **Release semantics (memory_order_release)** in the put() function guarantee that any prior writes (to the buffer) are visible before the write_index is updated.
- **Acquire semantics (memory_order_acquire)** in the get() function ensure that after reading the write_index, the consumer sees the correct data in the buffer.

With all this I ended up concluding there is indeed an issue that needs consideration for my use case, at least for ARM Cortex m4 and m7 that I am targeting.

# C Ring Buffer Implementation

The rationale behind the specific atomic access types in `ring_buf.c` implementation is based on the following principles of atomicity and memory ordering:

### Key Points of Atomic Operations:
1. **Relaxed Memory Order** (`memory_order_relaxed`): This is used when the operation needs to be atomic, but doesn’t require any synchronization with other threads. This is typically used for operations when only updating a value that doesn’t affect memory visibility between threads.

2. **Acquire Memory Order** (`memory_order_acquire`): This ensures that all subsequent memory reads and writes are done **after** the acquire operation. Used for a read of a shared variable, ensuring that the application also sees any side effects that happened in another thread before writing to that variable.

3. **Release Memory Order** (`memory_order_release`): This ensures that all previous memory writes are completed **before** the release operation. This is used when writing to a shared variable to ensure that any data written prior to this write is visible to other threads that subsequently acquire this variable.

### Ring Buffer Scenario:
The ring buffer operates in a single-producer, single-consumer (SPSC) context, where:
- **Producer** (`put()`) is adding elements.
- **Consumer** (`get()`) is removing elements.
- **Producer and consumer** run in different contexts (e.g., main thread vs. interrupt handler).

In this context, the atomicity and memory ordering serve two purposes:
- Ensure **safe concurrent access** (i.e., no data corruption or race conditions).
- Ensure **correct memory visibility** (i.e., the consumer always sees the correct state of the buffer after the producer has written data, and vice versa).

### Rationale for Specific Atomic Operations in `ring_buf.c`:

#### 1. `RingBuf_put()`
```c
RingBufCtr head = atomic_load_explicit(&me->head, memory_order_relaxed);
```
- **Why `memory_order_relaxed`?**
  - We are only reading the `head` index to calculate where to write the next element. This read doesn’t involve any synchronization with the consumer (which reads `tail`). The key point here is that this read is purely for internal logic, and the synchronization comes later when the `head` is updated. Therefore, `memory_order_relaxed` is sufficient for this read.

```c
RingBufCtr tail = atomic_load_explicit(&me->tail, memory_order_acquire);
```
- **Why `memory_order_acquire`?**
  - This is a critical read. Before we write a new element to the buffer, we need to ensure that any writes to the buffer that occurred before this point (from the consumer) are visible. By using `memory_order_acquire`, we ensure that the consumer's operations (which may have modified `tail`) are fully visible to the producer. The acquire order ensures that the producer sees the most up-to-date value of `tail` and any prior operations done by the consumer.

```c
me->buf[me->head] = el;
```
- **Why no atomic operation on `buf[]`?**
  - The buffer itself (`buf[]`) does not need to be accessed atomically. The synchronization is achieved through atomic accesses to `head` and `tail`. Once `head` and `tail` are updated atomically, the buffer access is guaranteed to be valid.

```c
atomic_store_explicit(&me->head, head, memory_order_release);
```
- **Why `memory_order_release`?**
  - This is the key update in `put()`. By storing `head` with a `memory_order_release`, we ensure that all prior memory writes (i.e., the write to `buf[me->head]`) are completed before `head` is updated. This guarantees that once the consumer reads `head`, it will see all the writes to the buffer that happened before this update. This ensures the consumer sees the correct data in the buffer.

#### 2. `RingBuf_get()`
```c
RingBufCtr tail = atomic_load_explicit(&me->tail, memory_order_relaxed);
```
- **Why `memory_order_relaxed`?**
  - This is similar to the `head` read in `put()`. We are reading `tail` for internal logic, and this does not require synchronization with the producer. The critical synchronization point is when `head` is read, which comes next.

```c
RingBufCtr head = atomic_load_explicit(&me->head, memory_order_acquire);
```
- **Why `memory_order_acquire`?**
  - The consumer needs to ensure that any writes performed by the producer (to both `head` and the buffer) are fully visible before the consumer reads the data. By using `memory_order_acquire`, we guarantee that the consumer will see all the memory writes performed by the producer up to the point where `head` was updated. This ensures that the consumer reads the correct data from the buffer.

```c
atomic_store_explicit(&me->tail, tail, memory_order_release);
```
- **Why `memory_order_release`?**
  - This update to `tail` indicates that the consumer has processed an element from the buffer. We use `memory_order_release` to ensure that all memory writes (e.g., modifications to `pel`) are completed before this update. This guarantees that when the producer reads `tail`, it will see the effects of the consumer's operations.

#### 3. `RingBuf_num_free()`
```c
RingBufCtr head = atomic_load_explicit(&me->head, memory_order_acquire);
```
- **Why `memory_order_acquire`?**
  - The purpose of `RingBuf_num_free()` is to provide an accurate count of the available space in the buffer. To do so, it needs to ensure that it has the most up-to-date value of `head`, especially when called by the consumer. Using `memory_order_acquire` ensures that any previous writes to `head` by the producer are visible to this function.

```c
RingBufCtr tail = atomic_load_explicit(&me->tail, memory_order_relaxed);
```
- **Why `memory_order_relaxed`?**
  - This is a non-critical read. We are simply using it for calculating available space, and there’s no need for synchronization at this point. We’ve already synchronized with `head`, which is the key variable for space calculation.

#### 4. `RingBuf_process_all()`
```c
RingBufCtr head = atomic_load_explicit(&me->head, memory_order_acquire);
```
- **Why `memory_order_acquire`?**
  - This function processes elements from the buffer and needs to ensure it sees the most up-to-date value of `head`, reflecting the producer's actions. By using `memory_order_acquire`, it guarantees that any data written to the buffer by the producer is visible to the consumer.

```c
atomic_store_explicit(&me->tail, tail, memory_order_release);
```
- **Why `memory_order_release`?**
  - After processing the elements, the consumer updates `tail` to indicate that it has finished processing. Using `memory_order_release` ensures that all memory writes performed by the consumer (e.g., processing the buffer elements) are visible to the producer when it next reads `tail`.

---

### Summary of Memory Ordering:

- **`memory_order_acquire`**: Used when reading the head or tail to ensure visibility of previous operations performed by the other thread.
- **`memory_order_release`**: Used when updating the head or tail to ensure that all preceding memory writes are visible to the other thread.
- **`memory_order_relaxed`**: Used for non-synchronization reads or internal logic, where memory ordering is not a concern.

These atomic operations ensure that both the producer and consumer have consistent views of the shared buffer and indexes (`head` and `tail`), preventing race conditions and ensuring correct data flow in the multithreaded environment.