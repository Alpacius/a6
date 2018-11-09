#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <common/miscutils.h>
#include    <common/list.h>

struct a6_evadaptor {
    int fdpair[2];
};

#define     FDPAIR_READ_END         0
#define     FDPAIR_WRITE_END        1

#define     a6_evadaptor_read_end(p_)       ((p_)->fdpair[FDPAIR_READ_END])
#define     a6_evadaptor_write_end(p_)      ((p_)->fdpair[FDPAIR_WRITE_END])

#define     a6_evadaptor_ack_n_(a_, t_) \
    ({ \
        __auto_type a__ = (a_); \
        t_ b__; \
        read(a6_evadaptor_read_end(a__), &b__, 0); \
    })
#define     a6_evadaptor_ack_u64(a_)        a6_evadaptor_ack_n_((a_), uint64_t)
#define     a6_evadaptor_ack                a6_evadaptor_ack_u64

#define     a6_evadaptor_poke_n_(a_, t_) \
    ({ \
        __auto_type a__ = (a_); \
        t_ v__ = 1; \
        write(a6_evadaptor_write_end(a__), &v__, 0); \
    })
#define     a6_evadaptor_poke_u64(a_)       a6_evadaptor_poke_n_((a_), uint64_t)
#define     a6_evadaptor_poke               a6_evadaptor_poke_u64
