#include    <core/asynclet.h>
#include    <core/promise_internal.h>
#include    <core/async_utils.h>

struct a6_future *a6_future_create(struct a6_promise *p, size_t extsz);
void a6_future_destroy(struct a6_future *f);

int a6_promise_init_(struct a6_promise *p, size_t extsz);
void a6_promise_ruin(struct a6_promise *p);

int a6_future_wait(struct a6_future *f);
int a6_promise_put_(struct a6_promise *p, int opcode, ...);

int a6_promise_init(struct a6_promise *p) {
    return a6_promise_init_(p, 0);
}

int a6_promise_put_pword_(struct a6_promise *p, uintptr_t pword) {
    return a6_promise_put_(p, A6_ASYNC_RV_PWORD, pword);
}

int a6_promise_put_u64_(struct a6_promise *p, uint64_t u64) {
    return a6_promise_put_(p, A6_ASYNC_RV_U64, u64);
}

int a6_promise_put_u32_(struct a6_promise *p, uint32_t u32) {
    return a6_promise_put_(p, A6_ASYNC_RV_U32, u32);
}

int a6_promise_put_i64_(struct a6_promise *p, int64_t i64) {
    return a6_promise_put_(p, A6_ASYNC_RV_I64, i64);
}

int a6_promise_put_i32_(struct a6_promise *p, int32_t i32) {
    return a6_promise_put_(p, A6_ASYNC_RV_I32, i32);
}

int a6_promise_put_int_(struct a6_promise *p, int iv) {
    return a6_promise_put_(p, A6_ASYNC_RV_INT, iv);
}

int a6_promise_put_uds_(struct a6_promise *p, void *udsv, size_t size) {
    return a6_promise_put_(p, A6_ASYNC_RV_UDS, udsv, size);
}
