#pragma once

#include    <common/chrono.h>
#include    <core/timer.h>

static inline
struct a6_timer a6i_s_timer_epoch(uint64_t tval) {
    struct a6_timer t;
    (t.alarm.type = A6_TIMER_EPOCH), (t.alarm.unit = A6_TIMER_UNIT_MS), (t.alarm.val.epoch = tval);
    return t;
}

static inline
struct a6_timer a6i_s_timer_diff(int tval) {
    struct a6_timer t;
    (t.alarm.type = A6_TIMER_DIFF), (t.alarm.unit = A6_TIMER_UNIT_MS), (t.alarm.val.diff = tval);
    return t;
}

#define a6i_s_timer_now \
    a6i_s_timer_epoch(a6i_epoch_ms_now())
#define a6i_s_timer_diff(d_) \
    a6i_s_timer_epoch(a6i_epoch_ms_diff((d_)))
