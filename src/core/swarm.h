#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>

#include    <core/mthread.h>

struct a6_swarm {
    struct {
        uint32_t idx;
    } slb;
    struct a6_mthread_pool *mthpool;
};
