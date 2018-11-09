#include    <common/tid.h>

static __thread long tid_local = -1;

long do_gettid(void) {
    return (tid_local > 0) ? tid_local : (tid_local = syscall(__NR_gettid));
}
