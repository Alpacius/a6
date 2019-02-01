# liba6 - linux-based user-thread executor library

|CI           |Build on OS  |Host OS      |Build Status     |
|:------------|:------------|:------------|:----------------|
|**Travis CI**|Ubuntu Trusty|Ubuntu Trusty|[![Build Status](https://travis-ci.org/Alpacius/a6.svg?branch=master)](https://travis-ci.org/Alpacius/a6)|

|Compiler     |Context Switch Implementation  |Comments     |
|:------------|:------------------------------|:------------|
|**GCC 4.9**  |POSIX ucontext                 |--           |
|**GCC 5**    |POSIX ucontext                 |--           |
|**GCC 6**    |POSIX ucontext                 |--           |
|**GCC 7**    |POSIX ucontext                 |--           |
|**GCC 8**    |POSIX ucontext                 |--           |
|**GCC 4.9**  |X86-64                         |--           |
|**GCC 8**    |X86-64                         |--           |

Liba6 is a lightweight user-level thread library, playing a role as an alternative to 
traditional thread pool solution for (mostly) isolated & concurrency tasks.

Currently, liba6 is only supported on Linux due to platform-specified implementation of I/O poller.

## Build prerequisites
* Linux >= 2.5 (for epoll APIs)
* GCC >= 4.9 (for `__auto_type` extension)

## Features
* Stackful user-level threads.
* Barrier-based semi-auto thread switch APIs for blocking & pollable fd-based I/O.
* Thread-pool-styled APIs for user-level thread management.
* Fast context switch under x86-64.

## Further Works
* Timeouts on I/O barriers.
* Promise API for blocking & non-pollable external operations.

## May-be Goals
* User-level lock support.
