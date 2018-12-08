#pragma once

#include    <common/stdc_common.h>
#include    <common/list.h>

struct a6_iomonitor;

struct a6_ioext_act {
    intrusive;
    void *arg;
    int (*hook)(struct a6_iomonitor *, void *arg);
};
