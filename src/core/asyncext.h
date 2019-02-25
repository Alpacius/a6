#pragma once

#include    <stddef.h>
#include    <stdint.h>

//#include    <a6/promise.h>
#include    <core/promise.h>

int a6_promise_init(struct a6_promise *p);
void a6_promise_ruin(struct a6_promise *p);

int a6_future_wait(struct a6_future *f);
void a6_future_destroy(struct a6_future *f);

int a6_promise_put_pword_(struct a6_promise *p, uintptr_t pword);
int a6_promise_put_u64_(struct a6_promise *p, uint64_t u64);
int a6_promise_put_u32_(struct a6_promise *p, uint32_t u32);
int a6_promise_put_i64_(struct a6_promise *p, int64_t i64);
int a6_promise_put_i32_(struct a6_promise *p, int32_t i32);
int a6_promise_put_int_(struct a6_promise *p, int iv);
int a6_promise_put_uds_(struct a6_promise *p, void *udsv, size_t size);
