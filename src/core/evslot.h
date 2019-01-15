#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <common/list.h>

#define     N_EVSLOTS_NORMAL        8209
#define     N_EVSLOTS_EXTENDED      102407

#ifndef     A6_LARGE_EVTBL
#define     N_EVSLOTS               N_EVSLOTS_NORMAL
#else
#define     N_EVSLOTS               N_EVSLOTS_EXTENDED
#endif

#ifndef     A6_SAFE_MODULE
#define     evtbl_idx(i_)           ((i_) % N_EVSLOTS)
#else
#define     evtbl_idx(i_) \
    ({ \
        __auto_type i__ = (i_); \
        i__ > 0 ? i__ % N_EVSLOTS : i__; \
    })
#endif

struct a6_evslots {
    struct link_index *slots;
    struct link_index slots_[];
};

static inline
struct a6_evslots *a6_evslots_create(void) {
    struct a6_evslots *tbl = (struct a6_evslots *) malloc(sizeof(struct a6_evslots) + sizeof(struct link_index) * (N_EVSLOTS + 1));
    tbl->slots = &(tbl->slots_[1]);
    for (int i = 0; i <= N_EVSLOTS; i++)
        list_init(&(tbl->slots_[i]));
    return tbl;
}

static inline
void a6_evslots_destroy(struct a6_evslots *tbl) {
    free(tbl);
}

static
struct link_index *a6_evslots_pick(struct a6_evslots *tbl, int key) {
    return tbl->slots + evtbl_idx(key);
}
