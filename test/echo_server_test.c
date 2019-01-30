#include    <stdio.h>
#include    <stdint.h>
#include    <stdlib.h>
#include    <string.h>
#include    <stdarg.h>

#include    <unistd.h>
#include    <sys/types.h>
#include    <errno.h>
#include    <fcntl.h>
#include    <ucontext.h>
#include    <pthread.h>
#include    <time.h>
#include    <sys/epoll.h>
#include    <sys/eventfd.h>
#include    <sys/uio.h>
#include    <sys/mman.h>
#include    <sys/time.h>
#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    <signal.h>

#include    "../src/core/swarm.h"

#define     N_TEST_REQS         20000
#define     N_TEST_CLIS         10
#define     N_TEST_SRVCORES     2

uint32_t success_count = 0;
uint32_t seq = 0;
pthread_barrier_t barrier_testinit;

// client-side

void *test_client(void *arg) {
    uint32_t n_reqs = N_TEST_REQS / N_TEST_CLIS;

    char buf_in[64], buf_out[64];

	struct sockaddr_in rserv;
	rserv.sin_family = AF_INET;
	rserv.sin_addr.s_addr = inet_addr("127.0.0.1");
	rserv.sin_port = htons(8080);
	bzero(&(rserv.sin_zero), 8);

    pthread_barrier_wait(&barrier_testinit);

    for (uint32_t i = 0; i < n_reqs; i++) {
        int conn_fd;
		if ((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("socket");
			continue;
		}
		if (connect(conn_fd, (struct sockaddr *) &rserv, sizeof(struct sockaddr)) == -1) {
			perror("connect");
            close(conn_fd);
			continue;
		}
        sprintf(buf_in, "hello server %u", __atomic_fetch_add(&seq, 1, __ATOMIC_ACQ_REL));
        if (send(conn_fd, buf_in, strlen(buf_in), 0) == -1) {
            perror("send");
            close(conn_fd);
            continue;
        }
        if (recv(conn_fd, buf_out, 64, 0) == -1) {
            perror("recv");
            close(conn_fd);
            continue;
        }
        if (strcmp(buf_in, buf_out) == 0)
            __atomic_add_fetch(&success_count, 1, __ATOMIC_ACQ_REL);
        close(conn_fd);
    }

    return NULL;
}

// server-side

void func(void *arg) {
    intptr_t conn_fd_p = arg;
    int conn_fd = (int) conn_fd_p;
    char buf[64];
    if (!a6_read_barrier_oneshot(conn_fd, 0)) {
        perror("a6_read_barrier_oneshot");
    }
    ssize_t rlen = recv(conn_fd, buf, 64, 0);
    if (rlen == -1) {
        perror("recv");
        close(conn_fd);
    } else {
        buf[rlen] = '\0';
        if (!a6_write_barrier_oneshot(conn_fd, 0)) {
            perror("a6_write_barrier_oneshot");
        }
        send(conn_fd, buf, rlen + 1, 0);
        close(conn_fd);
    }
}

void *test_echo_server(void *arg) {
    struct a6_swarm *swarm = a6_swarm_create(N_TEST_SRVCORES);
    a6_swarm_launch(swarm);

    int accept_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (accept_fd == 0) {
        perror("socket");
        abort();
    }

    struct sockaddr_in lserv;
    lserv.sin_family = AF_INET;
    lserv.sin_port = htons(8080);
    lserv.sin_addr.s_addr = INADDR_ANY;
    bzero(&(lserv.sin_zero), 8);

    if (bind(accept_fd, (struct sockaddr *) &lserv, sizeof(lserv)) == -1) {
        perror("bind");
        abort();
    }
    if (listen(accept_fd, 8) == -1) {
        perror("listen");
        abort();
    }

    pthread_barrier_wait(&barrier_testinit);

    uint32_t count = 0;
    while (count < N_TEST_REQS) {
        struct sockaddr_in raddr;
        socklen_t slen = sizeof(raddr);
        count++;
        int conn_fd = accept(accept_fd, (struct sockaddr *) &raddr, &slen);
        a6_swarm_run(swarm, func, (void *) conn_fd);
    }

    a6_swarm_destroy(swarm);

    return NULL;
}

// driver-side

int main(void) {
    pthread_barrier_init(&barrier_testinit, NULL, 1 + N_TEST_CLIS);
    pthread_t srv;
    pthread_t clis[N_TEST_CLIS];

    pthread_create(&srv, NULL, test_echo_server, NULL);
    for (int i = 0; i < N_TEST_CLIS; i++)
        pthread_create(&(clis[i]), NULL, test_client, NULL);

    pthread_join(srv, NULL);
    for (int i = 0; i < N_TEST_CLIS; i++)
        pthread_join(clis[i], NULL);

    uint32_t success_count_l = __atomic_load_n(&success_count, __ATOMIC_ACQUIRE);
    printf("success_count=%u(total=%u)\n", success_count_l, N_TEST_REQS);
    return (success_count_l == N_TEST_REQS) ? 0 : 1;
}
