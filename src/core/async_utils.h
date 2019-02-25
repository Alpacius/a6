#pragma once

#include    <stddef.h>
#include    <stdint.h>

#define     A6_ASYNC_RV_PWORD           0
#define     A6_ASYNC_RV_U64             1
#define     A6_ASYNC_RV_U32             2
#define     A6_ASYNC_RV_I64             3
#define     A6_ASYNC_RV_I32             4
#define     A6_ASYNC_RV_INT             5
#define     A6_ASYNC_RV_UDS             6

struct a6_async_retval {
    union {
        uintptr_t val_pword;
        void *val_ptr;
        uint64_t val_u64;
        uint32_t val_u32;
        int64_t val_i64;
        int32_t val_i32;
        int val_int;
    };
};
