#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <common/rbt.h>

#define     A6_TIMER_DIFF           0
#define     A6_TIMER_EPOCH          1

#define     A6_TIMER_UNIT_MS        1

struct a6_timer {
    map_indexed;
    struct {
        int type;
        uint64_t unit;
        union {
            uint64_t epoch;
            int diff;
        } val;
    } alarm;
};

#define     a6i_timer_tval(t_)      ((t_).alarm.val.epoch)
