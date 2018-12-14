#pragma once

#include    <core/iomonitor.h>

struct a6_waitk kdummy = { .type = A6_WAITK_DUMMY };

static
int a6_iomon_ltrd_fd_(struct a6_iomonitor *iomon, int fd, struct a6_waitk *k) {
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.ptr = k;
    return epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, fd, &epev);
}

#define a6i_ltrd_fd_(i_, f_) a6_iomon_ltrd_fd_((i_), (f_), &kdummy)
