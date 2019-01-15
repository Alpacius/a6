#include    <core/timer_queue.h>

static
int timer_compare(const void *lhs, const void *rhs) {
    return a6i_s_timer_compare(lhs, rhs);
}

void timer_queue_init(struct a6_timer_queue *q) {
    rbt_init(q, timer_compare);
}

void timer_queue_add(struct a6_timer_queue *q, struct a6_timer *t) {
    rbt_insert(q, mapindex_from_ptr(t));
}
