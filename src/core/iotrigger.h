#pragma once

#include    <core/iomonitor.h>
#include    <core/uthread_lifespan.h>
#include    <core/uthread_infest.h>

#define     a6_simple_read(uth_, iomon_, fd_, options_, ...) \
    ({ \
        struct a6_waitk k__; \
        k__.type = A6_WAITK_PLAIN; \
        k__.fd.i = (fd_); \
        k__.uth = (uth_); \
        int r__ = a6_prepare_read_quick((iomon_), k__.uth, k__.fd.i, &k__, (options_), ##__VA_ARGS__); \
        a6_uth_blocking; \
        uthread_yield; \
        r__; \
    })

#define     a6_simple_write(uth_, iomon_, fd_, options_, ...) \
    ({ \
        struct a6_waitk k__; \
        k__.type = A6_WAITK_PLAIN; \
        k__.fd.i = (fd_); \
        k__.uth = (uth_); \
        int r__ = a6_prepare_write_quick((iomon_), k__.uth, k__.fd.i, &k__, (options_), ##__VA_ARGS__); \
        a6_uth_blocking; \
        uthread_yield; \
        r__; \
    })

#define     a6_oneshot_read(uth_, iomon_, fd_, options_, ...) \
    ({ \
        struct a6_waitk k__; \
        k__.type = A6_WAITK_PLAIN; \
        k__.fd.i = (fd_); \
        k__.uth = (uth_); \
        int r__ = a6_prepare_read_oneshot((iomon_), k__.uth, k__.fd.i, &k__, (options_), ##__VA_ARGS__); \
        a6_uth_blocking; \
        uthread_yield; \
        r__; \
    })

#define     a6_oneshot_write(uth_, iomon_, fd_, options_, ...) \
    ({ \
        struct a6_waitk k__; \
        k__.type = A6_WAITK_PLAIN; \
        k__.fd.i = (fd_); \
        k__.uth = (uth_); \
        int r__ = a6_prepare_write_oneshot((iomon_), k__.uth, k__.fd.i, &k__, (options_), ##__VA_ARGS__); \
        a6_uth_blocking; \
        uthread_yield; \
        r__; \
    })

#define     a6_lterm_read(uth_, iomon_, fdw_, options_, ...) \
    ({ \
        struct a6_waitk k__; \
        k__.type = A6_WAITK_LTERM; \
        k__.fd.w = (fdw__); \
        k__.uth = (uth_); \
        a6_fdwrap w__ = a6_prepare_read_keepalive((iomon_), k__.uth, k__fd.w., &k__, (options_), ##__VA_ARGS__); \
        a6_uth_blocking; \
        uthread_yield; \
        w__; \
    })

#define     a6_lterm_write(uth_, iomon_, fdw_, options_, ...) \
    ({ \
        struct a6_waitk k__; \
        k__.type = A6_WAITK_LTERM; \
        k__.fd.w = (fdw__); \
        a6_fdwrap w__ = a6_prepare_write_keepalive((iomon_), k__.uth, k__fd.w., &k__, (options_), ##__VA_ARGS__); \
        a6_uth_blocking; \
        uthread_yield; \
        w__; \
    })
