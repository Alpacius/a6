#include    <core/iomonitor.h>
#include    <core/uthread.h>

struct a6_iomonitor *a6_iomonitor_create(int cap) {
    struct a6_iomonitor *iomon = malloc(sizeof(struct a6_iomonitor) + sizeof(struct epoll_event) * cap);
    if (unlikely(iomon == NULL))
        return NULL;
    if (unlikely(socketpair(AF_UNIX, SOCK_STREAM, 0, iomon->extevch.fdpair) == -1))
        return free(iomon), NULL;
    if (unlikely((iomon->epfd = epoll_create1(EPOLL_CLOEXEC)) == -1))
        return 
            close(a6_evadaptor_read_end(&(iomon->extevch))), 
            close(a6_evadaptor_write_end(&(iomon->extevch))),
            free(iomon),
            NULL;
    // TODO timer heap init
    for (int i = 0; i < N_IOEXT_CHAINS; i++)
        list_init(&(iomon->ioext_chains[i]));
    return (iomon->cap = cap), iomon;
}

void a6_iomonitor_destroy(struct a6_iomonitor *iomon) {
    close(iomon->epfd);
    close(a6_evadaptor_read_end(&(iomon->extevch))), close(a6_evadaptor_write_end(&(iomon->extevch)));
    free(iomon);
}

// 3 groups of implementations should be offered for different situations:
//
//     - Quick-n-dirty mode. No special pre/postprocessing or status marking enabled. 
//       Fds shall wake the corresponding io monitor up whenever triggered. 
//       All fd registeration actions shall be EPOLL_CTL_ADD. EEXIST errors are omitted however.
//
//     - Long-term mode. Library users should keep fd wrappers for further actions.
//       Fd wrappers keep polling registeration status, thus not EEXIST shall be triggered.
//       However, further option changes are omitted when adding fd to the epoll set.
//
//     - One-shot mode. EPOLLONSHOT attribute shall be attached to epoll event arguments.
//       This mode is suitable for simple request-respond communication, 
//       aiming at minimization of redundant syscall error occurence and least payload pressure on the monitor.
//
// Read & write ops may be implemented separately.

static inline
uint32_t build_ep_events_aux(uint32_t options) {
    // TODO implementation
    return 0;
}

static inline
void handle_timeout_event(struct a6_iomonitor *iomon, uint32_t options, va_list vargs) {
    // TODO implementation
}

int a6_prepare_event_quick(struct a6_iomonitor *iomon, struct a6_uthread *uth, int fd, uint32_t main_ev, void *udata, uint32_t options, ...) {
    struct epoll_event ev;
    (ev.data.ptr = uth), (ev.events |= main_ev|build_ep_events_aux(options)), (ev.data.ptr = udata);
    {
        va_list vargs;
        va_start(vargs, options);
        handle_timeout_event(iomon, options, vargs);
        va_end(vargs);
    }
    if (epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        int errsv = errno;
        return likely(errsv == EEXIST) ? 1 : 0;
    }
    return 1;
}

a6_fdwrap a6_prepare_event_keepalive(struct a6_iomonitor *iomon, struct a6_uthread *uth, a6_fdwrap fdw, uint32_t main_ev, void *udata, uint32_t options, ...) {
    if (a6_fdwrap_exists(fdw))
        return fdw;
    struct epoll_event ev;
    (ev.data.ptr = uth), (ev.events |= main_ev|build_ep_events_aux(options)), (ev.data.ptr = udata);
    {
        va_list vargs;
        va_start(vargs, options);
        handle_timeout_event(iomon, options, vargs);
        va_end(vargs);
    }
    return (epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, a6_fdwrap_fd(fdw), &ev) == 0) ? a6_fdwrap_mark_reg(fdw) : a6_fdwrap_mark_err(fdw);
}

int a6_prepare_read_oneshot(struct a6_iomonitor *iomon, struct a6_uthread *uth, int fd, void *udata, uint32_t options, ...) {
    struct epoll_event ev;
    (ev.data.ptr = uth), (ev.events |= ((EPOLLIN|build_ep_events_aux(options)) & ~EPOLLOUT)|EPOLLONESHOT), (ev.data.ptr = udata);
    {
        va_list vargs;
        va_start(vargs, options);
        handle_timeout_event(iomon, options, vargs);
        va_end(vargs);
    }
    if (epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        int errsv = errno;
        return (errsv == EEXIST) ? epoll_ctl(iomon->epfd, EPOLL_CTL_MOD, fd, &ev) : 0;
    }
    return 1;
}

int a6_prepare_write_oneshot(struct a6_iomonitor *iomon, struct a6_uthread *uth, int fd, void *udata, uint32_t options, ...) {
    struct epoll_event ev;
    (ev.data.ptr = uth), (ev.events |= ((EPOLLOUT|build_ep_events_aux(options)) & ~EPOLLIN)|EPOLLONESHOT), (ev.data.ptr = udata);
    {
        va_list vargs;
        va_start(vargs, options);
        handle_timeout_event(iomon, options, vargs);
        va_end(vargs);
    }
    if (epoll_ctl(iomon->epfd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        int errsv = errno;
        return (errsv == ENOENT) ? epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, fd, &ev) : 0;
    }
    return 1;
}

int a6_iomonitor_poll(
        struct a6_iomonitor *iomon, 
        struct link_index **res_groups, 
        uint32_t n_res_groups, 
        void (*collect)(struct a6_ioevent *, struct link_index **, uint32_t),
        uint32_t options) {
#define ioext_run(m_, i_) \
    do { \
        struct a6_iomonitor *m__ = (m_); \
        int i__ = (i_); \
        struct link_index *c__ = &(m__->ioext_chains[i__]); \
        list_foreach(c__) { \
            struct a6_ioext_act *a = intrusive_ref(struct a6_ioext_act); \
            a->hook(m__, a->arg); \
        } \
    } while (0)
    // TODO check timer & timeout
    iomon->current_state.timeout = -1;
    ioext_run(iomon, IDX_IOEXT_PREPOLL);
    int nfds = epoll_wait(iomon->epfd, iomon->epevents, iomon->cap, iomon->current_state.timeout);
    if (unlikely(nfds == -1))
        return 0;
    ioext_run(iomon, IDX_IOEXT_PRETIMED);
    // TODO timed events
    ioext_run(iomon, IDX_IOEXT_PREIO);
    for (int i = 0; i < nfds; i++) {
        struct a6_waitk *k = iomon->epevents[i].data.ptr;
        struct a6_ioevent ioev = { .type = A6_IOEV_EP, .fd = a6_waitk_p_fd(k), .udata = k };
        collect(&ioev, res_groups, n_res_groups);
    }
    ioext_run(iomon, IDX_IOEXT_POSTED);
    return 1;
#undef ioext_run
}
