_This project has been created as part of the 42 curriculum by rhlou._

# Codexion

> *Master the race for resources before the deadline masters you.*
A **dining-philosophers-style concurrency simulation** in C, reframed as coders competing for USB dongles in a co-working hub. Built as part of the 42 / 1337 curriculum.

---

## Description

- **N coders** sit in a circle, each running as a **pthread thread**.
- **N dongles** lie between adjacent coders (one between each pair).
- To **compile**, a coder must hold **both** their left and right dongles simultaneously.
- After compiling → **debug** → **refactor** → repeat.
- A coder **burns out** if they go more than `time_to_burnout` ms without starting a compile.
- The simulation ends when a coder burns out, or when all coders have reached `number_of_compiles_required`.

---


## Instructions

```
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> \
           <time_to_debug> <time_to_refactor> <number_of_compiles_required> \
           <dongle_cooldown> <scheduler>
```

### Arguments

| Argument | Unit | Description |
|---|---|---|
| `number_of_coders` | — | Number of coders and dongles |
| `time_to_burnout` | ms | Max time without compiling before a coder burns out |
| `time_to_compile` | ms | Duration of the compile phase (holds both dongles) |
| `time_to_debug` | ms | Duration of the debug phase |
| `time_to_refactor` | ms | Duration of the refactor phase |
| `number_of_compiles_required` | — | Target compile count per coder |
| `dongle_cooldown` | ms | Time a dongle is unavailable after being released |
| `scheduler` | — | `fifo` (arrival order) or `edf` (earliest deadline first) |

### Examples

```bash
# 5 coders, FIFO scheduler, 3 compiles each
./codexion 5 800 200 100 100 3 50 fifo

# 3 coders, EDF scheduler, 10 compiles each
./codexion 3 500 100 50 50 10 0 edf
```

---

## Output Format

Every event is printed to stdout as:
```
timestamp_ms  coder_id  event
```

Events:
```
142 3 is compiling
342 3 is debugging
442 3 is refactoring
801 1 burned out
```

- Timestamps are in milliseconds from simulation start.
- Output is protected by a mutex — lines never interleave.
- The burnout message appears within 10ms of the actual burnout.

---

## Architecture

```
src/
├── main.c        — arg validation, init, thread launch and join
├── init.c        — allocate/init sim, coders, dongles, mutexes
├── coder.c       — coder thread routine (compile/debug/refactor loop)
├── monitor.c     — monitor thread watching for burnout
├── dongle.c      — dongle_acquire / dongle_release with wait queue
├── scheduler.c       — min-heap priority queue (EDF & FIFO support)
├── scheduler_utils.c — heap helper functions
├── log.c             — thread-safe log_event
├── utils.c           — time and state helpers
└── utils2.c          — string helpers (atoi)

include/
└── codexion.h    — all structs and prototypes
```

---

## Thread synchronization mechanisms

### Deadlock Prevention and Coffman’s Conditions
Deadlocks occur when Coffman's four conditions are met: Mutual Exclusion, Hold and Wait, No Preemption, and Circular Wait. This project actively breaks the **Circular Wait** condition through asymmetric dongle acquisition. Instead of every coder reaching for their left dongle first, the acquisition order depends on the coder's ID:
- **Even ID coders**: grab left dongle first, then right.
- **Odd ID coders**: grab right dongle first, then left.
This structural asymmetry mathematically guarantees that a circular wait dependency can never form, preventing classical deadlocks.

### Wait Queue — Min-Heap Per Dongle
Each dongle contains its own state protected by a `pthread_mutex_t mutex` and a `pthread_cond_t cond` variable. When a coder attempts to acquire a dongle:
1. They push their request onto the dongle's **min-heap priority queue**.
2. They block via `pthread_cond_wait`, yielding the CPU.
3. They are only allowed to acquire the dongle when they are at the top of the heap, the dongle is available, and the cooldown has expired.

### Log Serialization
To prevent garbled or interleaved terminal output, all logging is serialized through a dedicated, global `pthread_mutex_t log_mutex`. Every state change calls a `log_event()` function which explicitly locks this mutex, calculates the precise timestamp, prints the log line via `printf`, and unlocks the mutex. This ensures every message is written atomically.

---

## Blocking cases handled

### Starvation Prevention
Starvation occurs when a coder is perpetually denied access to dongles. We solve this via strict scheduling policies:
1. **FIFO (First In, First Out)**: Utilizes a global atomic counter `fifo_counter` protected by `fifo_mutex`. Older requests are mathematically guaranteed to be served before newer requests.
2. **EDF (Earliest Deadline First)**: Prioritizes coders closest to burning out (`last_compile_start + time_to_burnout`).
3. **Secondary Tie-Breaker**: If two coders have the identical priority (e.g., exact same deadline), the min-heap tie-breaks using `compile_count`. The coder who has compiled *less* wins the tie, heavily favoring starving coders and mathematically preventing indefinite starvation.

### Precise Cooldowns and Sleep Architecture
When a coder releases a dongle, they record the `released_at` timestamp. The dongle cannot be acquired again until `get_time_ms() - released_at >= dongle_cooldown`. 
- To wait for cooldowns without constant polling, `wait_for_dongle` calculates the exact absolute expiry time and uses `pthread_cond_timedwait`. This allows the coder to gracefully yield the CPU and wake up autonomously the exact millisecond their cooldown ends.
- For all other phases (compiling, debugging, refactoring), coders use `ft_msleep`, which uses a `usleep(100)` polling loop to ensure sub-millisecond precision.

### Precise Burnout Detection
A dedicated monitor thread (`monitor_routine`) loops continually, sleeping for 1 ms intervals to minimize CPU strain. It reads each coder's `last_compile_start` timestamp to calculate elapsed time. Because reading the timestamp and checking the `simulation_over` flag are both tightly optimized and protected by mutexes, the monitor can detect a burnout and print the death log well within the strict 10 millisecond tolerance limit. If a single-coder configuration is provided, the coder correctly holds their single dongle and waits for the monitor to formally log their burnout, preventing structural hangs.

## Advanced Features & Optimizations

### Thread Creation Failure Handling
If the OS restricts thread creation (e.g., via `ulimit -u`), the simulation catches `pthread_create` failures safely, aborting the process instead of hanging or segfaulting in cleanup.

### Starvation Race Condition Prevention
A classic multi-threading edge case exists where a starving coder wakes up to grab a dongle at the exact millisecond their burnout expires, updating their `last_compile_start` a microsecond before the monitor thread checks on them, effectively "cheating death". This project explicitly handles this: coders self-check their burnout deadline before updating their timer, yielding to the monitor if they starved.

### Efficient Resource Management
The simulation implements memory-efficient priority queues pre-allocated based on the number of coders, adhering to strict libc function constraints (avoiding `realloc`) while eliminating allocation overhead during runtime.

---

## Resources

- Documentation for `pthread` API
- POSIX standard references for threading behavior
- Generative AI was used extensively during the development and iteration of this codebase. Antigravity AI analyzed edge cases, proposed concurrency designs (like EDF wait queues and even-odd deadlock prevention), diagnosed thread-safety races, and maintained code formatting against the 42 Norm standards.

## Build

```bash
make        # build the binary
make clean  # remove object files
make fclean # remove objects + binary
make re     # fclean + build
```

Object files are output to `objects/`.

---

## 👤 Author

**rhlou** — rhlou@student.42.fr — 1337 / 42 Network
