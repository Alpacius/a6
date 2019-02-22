#pragma once

#include    <core/iomonitor.h>

static
int a6_iomon_ltrd_fd_(struct a6_iomonitor *iomon, int fd, struct a6_waitk *k) {
    k->type = A6_WAITK_DUMMY;
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = A6_FD_DUMMY;
    a6_ioev_add(iomon->evtbl, k);
    return epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, fd, &epev);
}
