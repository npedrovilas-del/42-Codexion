*This project has been created as part of the 42 curriculum by pneto-vi.*

# Codexion

## Description

**Codexion** is a concurrency simulation written in C. One or more coders sit in a circular co-working hub and share USB dongles to compile quantum code.

- Compiling requires **two dongles at the same time** (left and right).
- After compiling, a coder **debugs**, then **refactors**, then tries to compile again.
- Each coder must start compiling again within `time_to_burnout` ms of their last compile start (or of simulation start); otherwise they **burn out**.

Compared to a classic dining-philosophers setup, this project also requires:

1. **Dongle cooldown** — after release, a dongle cannot be taken again until `dongle_cooldown` ms have elapsed.
2. **Fair arbitration** — when several coders wait for the same dongle, access follows `fifo` or `edf`.
3. **Precise burnout detection** — a monitor thread must log burnout within **10 ms** of the real deadline.
4. A **hand-written priority queue (heap)** for FIFO/EDF (no standard-library priority queue).

The simulation stops when a coder burns out **or** when every coder has compiled at least `number_of_compiles_required` times.

Shared state lives in a stack-allocated `t_hub` (no global variables). Each dongle keeps its own mutex, condition variable and waiter heap. Coders acquire dongles in ascending index order to break circular wait. Waiters use `pthread_cond_timedwait` so they wake for grants, cooldown expiry and simulation stop without busy-waiting.

## Instructions

### Requirements

- C compiler with POSIX threads (`cc`, `pthread`)
- Unix-like OS (`gettimeofday`, `pthread`)

### Build

```bash
make
```

Builds `./codexion` with `-Wall -Wextra -Werror -pthread`.

| Target        | Action                     |
|---------------|----------------------------|
| `make` / `all`| Build `codexion`           |
| `make clean`  | Remove object files        |
| `make fclean` | Remove objects and binary  |
| `make re`     | Full rebuild               |

### Run

```bash
./codexion number_of_coders time_to_burnout time_to_compile \
           time_to_debug time_to_refactor number_of_compiles_required \
           dongle_cooldown scheduler
```

All arguments are mandatory. Invalid inputs (wrong count, non-integers, negatives where rejected, scheduler other than `fifo` / `edf`) are rejected.

| Argument | Meaning |
|----------|---------|
| `number_of_coders` | Number of coders **and** dongles (≥ 1) |
| `time_to_burnout` | Max ms since last compile start before burnout |
| `time_to_compile` | Time spent compiling while holding two dongles |
| `time_to_debug` | Debug phase duration |
| `time_to_refactor` | Refactor phase duration |
| `number_of_compiles_required` | Stop when every coder reached this compile count |
| `dongle_cooldown` | Ms a dongle stays unavailable after release |
| `scheduler` | Exactly `fifo` or `edf` |

### Examples

```bash
./codexion 5 800 200 200 200 7 50 fifo
./codexion 5 800 200 200 200 7 50 edf
./codexion 1 200 100 100 100 5 0 fifo
./codexion 4 310 200 100 100 10 0 fifo
```

### Log format

```
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

Each `is compiling` is preceded by two `has taken a dongle` lines for that coder (except the single-coder case, which has only one dongle).

## Project structure

```
.
├── Makefile
├── README.md
└── src/
    ├── codexion.h      # shared types and prototypes
    ├── main.c          # setup, threads, teardown
    ├── parser.c        # argument parsing / validation
    ├── time.c          # timestamps and sleep helpers
    ├── init.c          # hub mutexes, alloc, seating
    ├── init_dongles.c  # dongle init / destroy helpers
    ├── heap.c          # FIFO/EDF compare + sift
    ├── heap_ops.c      # heap push / pop / remove
    ├── dongles.c       # take / release + cooldown
    ├── coders.c        # coder lifecycle
    ├── monitor.c       # burnout + quota stop
    └── logs.c          # serialized logging + hub_stop
```

## Blocking cases handled

### Deadlock prevention (Coffman)

- **Mutual exclusion** — each dongle’s state is protected by its own `pthread_mutex_t`.
- **No circular wait** — with several coders, dongles are always acquired in **ascending index order** (`first`, then `second`).
- **Single coder** — only one dongle exists; the coder takes it once and waits until burnout (avoids locking the same non-recursive mutex twice).
- Hold-and-wait still exists by design (a coder may hold one dongle while waiting for the second), but without circular wait the system cannot deadlock.

### Starvation / fairness

- **FIFO** — waiters are ordered by arrival `seq` on that dongle.
- **EDF** — waiters are ordered by earliest deadline (`last_compile + time_burnout`); equal deadlines break on `seq`, then on `id_code`. Under feasible parameters, urgent coders are not starved.

### Dongle cooldown

- On release: `cooldown = now + dongle_cooldown`.
- A waiter is granted the dongle only if it is free, they are the heap head, **and** cooldown has expired.

### Precise burnout detection

- A dedicated **monitor** thread polls burnout and the compile quota.
- Burnout is logged within **10 ms** of the actual deadline; then the simulation is stopped and waiters are woken.

### Log serialization

- All state messages go through a logging mutex so two lines never interleave.

### Clean stop / no leaks

- Stopping sets a shared flag and broadcasts every dongle condition variable so blocked takes can exit.
- Heaps, mutexes, condvars and allocations are destroyed on exit.

## Thread synchronization mechanisms

| Primitive | Where | Role |
|-----------|--------|------|
| `pthread_mutex_t` (per dongle) | `t_dongle.mutex` | Protects holder, cooldown, waiter heap |
| `pthread_cond_t` (per dongle) | `t_dongle.condition` | Waiters sleep until grant / cooldown / stop |
| `pthread_mutex_t` | `t_hub.stop_mutex` | Guards the `stopped` flag |
| `pthread_mutex_t` | `t_hub.mutex_state` | Protects `last_compile` / compile counts |
| `pthread_mutex_t` | `t_hub.mutex` | Serializes log output |
| Custom binary heap | `t_dongle.wait` | FIFO / EDF priority queue (no libc PQ) |

**Coder ↔ dongle:** lock dongle → push request → `pthread_cond_timedwait` until eligible → pop → set holder → unlock → log take.

**Release:** clear holder → set cooldown → `pthread_cond_broadcast` so waiters re-check eligibility.

**Monitor ↔ coders:** monitor sets `stopped` and broadcasts all dongle condvars; coders observe the flag and leave take/sleep loops so joins finish cleanly.

**Race prevention examples:**

- Two threads never both believe they own a dongle: grant happens only under `dongle.mutex` with `holder` checked under that lock.
- Compile count / `last_compile` updates are protected by `mutex_state`, which the monitor also uses when checking burnout and quota.

## Resources

### References

- [POSIX threads (pthread) — man7](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [pthread_mutex / pthread_cond](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- Dining philosophers problem (deadlock / Coffman conditions)
- Liu & Layland — *Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment* (EDF)
- 42 Codexion subject PDF (project rules and log format)

### AI usage

AI was used as a study and review aid: clarifying pthread / heap concepts, checking edge cases against the subject, drafting this README structure, and helping verify builds with Valgrind.
