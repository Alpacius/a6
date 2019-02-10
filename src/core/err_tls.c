#include    <core/err_tls.h>

static __thread int a6_errno_tls = 0;

int a6_get_errno(void) {
    return a6_errno_tls;
}

void a6i_set_errno(int errcode) {
    a6_errno_tls = errcode;
}
