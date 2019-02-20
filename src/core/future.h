#pragma once

#include    <common/stdc_common.h>
#include    <core/asynclet.h>

struct a6_promise;

struct a6_future {
    struct a6_asynclet base;
    struct a6_uthread *uth;
    struct a6_promise *promise;
    union {
        uintptr_t val_pword;
        void *val_ptr;
        uint64_t val_u64;
        uint32_t val_u32;
        int64_t val_i64;
        int32_t val_i32;
        int val_int;
    };
    char ext_payload[];
};
