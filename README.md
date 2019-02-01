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
* FFI (e.g. Perl adaptor)
* Cancellation points.
* Limited preemptive rescheduling.

## Non-goals
* Interthread communication support (e.g. channels, mailboxes).

## Build
For build:
```
mkdir -p m4
autoreconf -ifv
./configure   # optional feature --enable-arch-specified-impl for optimized implementation (currently available for `uname -m` = "x86_64" only)
make   # or make check for test (the same test suites on CI platforms)
```

For install of libraries (both archive and shared library):
```
make install
```

## Examples
See [tests](https://github.com/Alpacius/a6/tree/master/test) for examples.

## API

### User-level thread pool (swarm) management 

#### Construct & Destruct

##### Synopsis
```C
#include    <a6/core/swarm.h>

struct a6_swarm *a6_swarm_create(uint32_t size);
int a6_swarm_launch(struct a6_swarm *swarm);

void a6_swarm_destroy(struct a6_swarm *swarm);
```

##### Description
```
// TODO implementation
```

##### Return Value
```
// TODO implementation
```

##### Thread Safety
```
// TODO Safe
```

#

#### Launch a user-level thread

##### Synopsis
```C
#include    <a6/core/swarm.h>

int a6_swarm_launch(struct a6_swarm *swarm);
```

##### Description
```
// TODO implementation
```

##### Return Value
```
// TODO implementation
```

##### Thread Safety
```
// TODO Safe
```

#

#### I/O barriers

##### Synopsis
```C
#include    <a6/core/swarm.h>

int a6_read_barrier_simple(int fd, uint32_t options);
int a6_write_barrier_simple(int fd, uint32_t options);
int a6_read_barrier_oneshot(int fd, uint32_t options);
int a6_write_barrier_oneshot(int fd, uint32_t options);
```

##### Description
```
// TODO implementation
```

##### Return Value
```
// TODO implementation
```

##### Thread Safety
```
// TODO Safe
```
