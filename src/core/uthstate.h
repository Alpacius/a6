#pragma once

#include    <common/stdc_common.h>
#include    <common/list.h>

struct a6_uthstate {
    intrusive;
    int state;
};

#define     A6_UTH_ENTRY            0
#define     A6_UTH_BLOCKING         1
