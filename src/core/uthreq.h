#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <common/list.h>

#define     DEFAULT_N_STKPAGES      64

struct a6_uth_req {
    intrusive;
    void (*func)(void *);
    void *arg;
    void (*dispose)(struct a6_uth_req *);
};
