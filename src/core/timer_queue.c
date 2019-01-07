#include    <core/timer_queue.h>


static
int timer_compare(const void *lhs, const void *rhs) {
    return a6i_s_timer_compare(lhs, rhs);
}
