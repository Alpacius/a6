#pragma once

#include    <common/stdc_common.h>
#include    <common/miscutils.h>
#include    <common/list.h>

// Thanks to lh_mouse.

struct tribuffer {
    uint32_t pliot;
    struct link_index bufs[3];
};

static inline
struct tribuffer *tribuffer_init(struct tribuffer *tb) {
    return 
        list_init(&(tb->bufs[0])), 
        list_init(&(tb->bufs[1])), 
        list_init(&(tb->bufs[2])), 
        (tb->pliot = 1),
        tb;
}

static inline
struct link_index *tribuffer_rdside(struct tribuffer *tb) {
    uint32_t rd, wr;
    uint32_t old = __atomic_load_n(&(tb->pliot), __ATOMIC_ACQUIRE);
    do {
        rd = old / 4;
        wr = old % 4;
        rd = (0 + 1 + 2) - rd;
    } while (!__atomic_compare_exchange_n(&(tb->pliot), &old, rd * 4 + wr, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED));
    return &(tb->bufs[rd]);
}

static inline
struct link_index *tribuffer_wrside(struct tribuffer *tb) {
    uint32_t rd, wr;
    uint32_t old = __atomic_load_n(&(tb->pliot), __ATOMIC_ACQUIRE);
    do {
        rd = old / 4;
        wr = old % 4;
        wr = (0 + 1 + 2) - wr;
    } while (!__atomic_compare_exchange_n(&(tb->pliot), &old, rd * 4 + wr, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED));
    return &(tb->bufs[wr]);
}
