#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>

#define     A6_TIMER_DIFF           0
#define     A6_TIMER_EPOCH          1

#define     A6_TIMER_UNIT_MS        1

#ifdef      a6_timer_index_ctl
struct a6_timer {
    a6_timer_index_ctl;
    struct {
        int type;
        uint64_t unit;
        union {
            uint64_t epoch;
            int diff;
        } val;
    } alarm;
};
#else
#error      "no definition for a6_timer_index_ctl"
#endif

#define     a6i_timer_tval(t_)      ((t_).alarm.val.epoch)
