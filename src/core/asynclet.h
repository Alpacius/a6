#pragma once

#include    <common/stdc_common.h>

struct a6_uthread;

#define     A6_ASYNCLET_PWORD       0
#define     A6_ASYNCLET_EXT         1

struct a6_asynclet {
    int chan;
    int valtype;
    size_t valsize;
};

struct a6_promise {
    struct a6_asynclet base;
    struct a6_uthread *uth;
    uintptr_t val_pword;
    char ext_payload[];
};

struct a6_future {
    struct a6_asynclet base;
    struct a6_promise *promise;
};
