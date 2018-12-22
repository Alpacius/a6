#include    <stdio.h>
#include    <common/stdc_common.h>
#include    <common/linux_common.h>

#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    <errno.h>
#include    <unistd.h>
#include    <dirent.h>

#include    <core/iomonitor.h>

int accept_fd = -1;
struct a6_iomonitor *iomon = NULL;

struct a6_waitk *test_new_waitk(int fd) {
    struct a6_waitk *k = malloc(sizeof(struct a6_waitk));
    return (k->type = A6_WAITK_PLAIN), (k->fd.i = fd), k;
}

void test_delete_waitk(struct a6_waitk *k) {
    free(k);
}

void do_collect(struct a6_ioevent *ioev, struct link_index **groups, uint32_t n_groups) {
    if (ioev->fd == accept_fd) {
        struct sockaddr rcli;
        socklen_t rclilen = sizeof(struct sockaddr);
        int conn_fd = accept(ioev->fd, &rcli, &rclilen);
        printf("incoming fd=%d\n", conn_fd);
        a6_prepare_read_oneshot(iomon, NULL, conn_fd, test_new_waitk(conn_fd), 0);
    } else {
        char buf[32];
        ssize_t packlen = recv(ioev->fd, buf, 32, 0);
        buf[packlen] = 0;
        printf("incoming packet on fd=%d, packet=%s\n", ioev->fd, buf);
        close(ioev->fd);
        test_delete_waitk((struct a6_waitk *) ioev->udata);
    }
}

int main(void) {
    iomon = a6_iomonitor_create(32);

    accept_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (accept_fd == -1) {
        perror("socket");
        abort();
    }

    int optval = 1;
    setsockopt(accept_fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &optval, sizeof(socklen_t));

    struct sockaddr_in lserv;
    lserv.sin_family = AF_INET;
    lserv.sin_port = htons(60000);
    lserv.sin_addr.s_addr = INADDR_ANY;
    bzero(&(lserv.sin_zero), 8);

    if (bind(accept_fd, (const struct sockaddr *) &lserv, sizeof(lserv)) == -1) {
        perror("bind");
        abort();
    }

    if (listen(accept_fd, SOMAXCONN) == -1) {
        perror("listen");
        abort();
    }

    a6_fdwrap accept_fdwrap = a6_fdwrap_fd(accept_fd);
    struct a6_waitk ak;
    ak.type = A6_WAITK_LTERM;
    ak.fd.w = accept_fdwrap;
    accept_fdwrap = a6_prepare_read_keepalive(iomon, NULL, accept_fdwrap, &ak, 0);
    printf("%x\n", accept_fdwrap);

    for (;;)
        if (!a6_iomonitor_poll(iomon, NULL, 0, do_collect, 0))
            perror("a6_iomonitor_poll");

    a6_iomonitor_destroy(iomon);
    close(accept_fd);
    return 0;
}
