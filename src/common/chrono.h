#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>

static inline
uint64_t a6i_epoch_ms_now(void) {
    struct timespec timeval;
    clock_gettime(CLOCK_REALTIME_COARSE, &timeval);
    return ((uint64_t) timeval.tv_sec * 1000) + ((uint64_t) timeval.tv_nsec / (1000 * 1000));
}

static inline
uint64_t a6i_epoch_ms_diff(int64_t diff) {
    return a6i_epoch_ms_now() + diff;
}

static inline
int a6i_epoch_ms_diff_s(int64_t diff, uint64_t *res_p) {
    return __builtin_add_overflow(a6i_epoch_ms_now(), diff, res_p);
}
