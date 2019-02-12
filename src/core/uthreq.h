#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <common/list.h>

#define     DEFAULT_N_STKPAGES      64

#define     A6_REQ_TYPE_UTH         0
#define     A6_REQ_TYPE_ASYNC       1

// type 0 - uthread creation
struct a6_uth_req {
    intrusive;
    void (*func)(void *);
    void *arg;
    void (*dispose)(struct a6_uth_req *);
};

// type 1 - external asynchronized response
struct a6_async_req {
    intrusive;
    void *arg;
    // TODO further implementation
};

struct a6_req_packet {
    int type;
    union {
        struct a6_uth_req r_uth;
        struct a6_async_req r_async;
    } payload;
};
