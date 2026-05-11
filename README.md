# *This project has been created as part of the 42 curriculum by ykhalouk*

# Codexion

Codexion is a multithreaded concurrency simulation project where multiple coders compete for shared USB dongles in order to compile code before burning out.

The project focuses on low-level thread synchronization, resource sharing, precise timing, scheduling strategies, and race condition prevention using the POSIX threads (`pthread`) library in C.

---

# Description

In Codexion, several coders run simultaneously in separate threads.

Each coder must:
1. Acquire two USB dongles
2. Compile
3. Debug
4. Refactor
5. Repeat the cycle

A coder burns out if they fail to compile before `time_to_burnout` expires.

The project simulates high-contention concurrent environments and solves synchronization problems such as:

- Deadlocks
- Starvation
- Race conditions
- Timing inaccuracies
- Unsafe concurrent logging

The simulation also includes advanced behaviors such as:
- FIFO and EDF scheduling
- Dongle cooldown handling
- Real-time monitoring
- Thread-safe communication
- Precise burnout detection

---

# Features

- Multithreaded simulation using `pthread`
- FIFO scheduler
- EDF (Earliest Deadline First) scheduler
- Precise millisecond timing
- Deadlock prevention
- Starvation reduction
- Dongle cooldown system
- Real-time monitor thread
- Thread-safe logging
- Graceful shutdown handling
- Fully written in C

---

# Instructions

## Compilation

```bash
make
```
## Run

```bash
examples:
./codexion 5 1000 100 50 50 5 150 fifo
./codexion 30 2000 100 50 50 5 150 edf
```


# Resources

## Documentation

- POSIX thread Pages  
  https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html

---

## AI Usage

AI tools were used as learning support during the project.

AI assistance included:
- Understanding how mutexes work
- Understanding condition variables
- Learning thread synchronization concepts

The final implementation and project architecture were fully written manually.