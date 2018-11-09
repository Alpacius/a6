#pragma once

#include    <common/linux_common.h>
#include    <core/evadaptor.h>

#ifdef      a6_evadaptor_impl
#error      "Duplicated a6_evadaptor_impl found."
#endif      // a6_evadaptor_impl

#define     a6_evadaptor_impl

#define     a6_evadaptor_afunix         a6_evadaptor

#define     a6_evadaptor_init           a6_evadaptor_afunix_init
#define     a6_evadaptor_create         a6_evadaptor_afunix_create
#define     a6_evadaptor_ruin           a6_evadaptor_afunix_ruin
#define     a6_evadaptor_destroy        a6_evadaptor_afunix_ruin

static inline
int a6_evadaptor_afunix_init(struct a6_evadaptor_afunix *a) {
    return socketpair(AF_UNIX, SOCK_STREAM, 0, a->fdpair);
}

static inline
struct a6_evadaptor_afunix a6_evadaptor_afunix_create(void) {
    struct a6_evadaptor_afunix a;
    if (unlikely(socketpair(AF_UNIX, SOCK_STREAM, 0, a.fdpair) == -1)) {
        a.fdpair[0] = a.fdpair[1] = -1;
    }
    return a;
}

static inline
int a6_evadaptor_afunix_ruin(struct a6_evadaptor_afunix *a) {
    return ((close(a->fdpair[0]) == 0) && (a->fdpair[0] = -1)) && 
           ((close(a->fdpair[1]) == 0) && (a->fdpair[1] = -1));
}
