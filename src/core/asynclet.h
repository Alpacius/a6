#pragma once

#include    <stddef.h>
#include    <stdint.h>

struct a6_uthread;

#define     A6_ASYNCLET_PWORD       0
#define     A6_ASYNCLET_EXT         1

#define     A6_ASYNC_CHAN_INIT     -1
#define     A6_ASYNC_CHAN_Q_PH1    -2
#define     A6_ASYNC_CHAN_Q_PH2    -3

struct a6_asynclet {
    int chan;
    int valtype;
    size_t valsize;
};

