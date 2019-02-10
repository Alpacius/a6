#include    <core/iomonitor.h>
#include    <core/uthread.h>

struct a6_evslots *a6i_curr_evslots(void);

struct a6_iomonitor *a6_iomonitor_create(int cap) {
    struct a6_iomonitor *iomon = malloc(sizeof(struct a6_iomonitor) + sizeof(struct epoll_event) * cap);
    if (unlikely(iomon == NULL))
        return NULL;
    if (unlikely((iomon->evtbl = a6_evslots_create()) == NULL))
        return free(iomon), NULL;
    if (unlikely(socketpair(AF_UNIX, SOCK_STREAM, 0, iomon->extevch.fdpair) == -1))
        return a6_evslots_destroy(iomon->evtbl), free(iomon), NULL;
    if (unlikely((iomon->epfd = epoll_create1(EPOLL_CLOEXEC)) == -1))
        return 
            close(a6_evadaptor_read_end(&(iomon->extevch))), 
            close(a6_evadaptor_write_end(&(iomon->extevch))),
            a6_evslots_destroy(iomon->evtbl),
            free(iomon),
            NULL;
    // TODO timer heap init
    for (int i = 0; i < N_IOEXT_CHAINS; i++)
        list_init(&(iomon->ioext_chains[i]));
    return (iomon->current_state.timeout = -1), (iomon->cap = cap), iomon;
}

void a6_iomonitor_destroy(struct a6_iomonitor *iomon) {
    close(iomon->epfd);
    close(a6_evadaptor_read_end(&(iomon->extevch))), close(a6_evadaptor_write_end(&(iomon->extevch)));
    a6_evslots_destroy(iomon->evtbl);
    free(iomon);
}

// 3 groups of implementations should be offered for different situations:
//
//     - Quick-n-dirty mode. No special pre/postprocessing or status marking enabled. 
//       Fds shall wake the corresponding io monitor up whenever triggered. 
//       All fd registeration actions shall be EPOLL_CTL_ADD. EEXIST errors are omitted however.
//
//     - Long-term mode. Library users should keep fd wrappers for further actions.
//       Fd wrappers keep polling registeration status, thus no EEXIST shall be triggered.
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

static inline
void a6i_iomon_add_k(struct a6_iomonitor *iomon, struct a6_waitk *k) {
    a6_ioev_add(iomon->evtbl, k);
}

// TODO macro instead of inlined function
static inline
void a6i_ioev_attach_k(struct a6_waitk *k) {
    struct a6_evslots *evtbl = a6i_curr_evslots();
    a6_ioev_add(evtbl, k);
}

int a6_prepare_event_quick(struct a6_iomonitor *iomon, struct a6_uthread *uth, int fd, uint32_t main_ev, struct a6_waitk *udata, uint32_t options, ...) {
    struct epoll_event ev;
    (ev.events = main_ev|build_ep_events_aux(options)), (ev.data.fd = fd);
    {
        va_list vargs;
        va_start(vargs, options);
        handle_timeout_event(iomon, options, vargs);
        va_end(vargs);
    }
    a6i_ioev_attach_k(udata);
    if (epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        int errsv = errno;
        return likely(errsv == EEXIST) ? 1 : 0;
    }
    return 1;
}

a6_fdwrap a6_prepare_event_keepalive(struct a6_iomonitor *iomon, struct a6_uthread *uth, a6_fdwrap fdw, uint32_t main_ev, struct a6_waitk *udata, uint32_t options, ...) {
    if (a6_fdwrap_exists(fdw))
        return fdw;
    struct epoll_event ev;
    (ev.events = main_ev|build_ep_events_aux(options)), (ev.data.fd = a6_fdwrap_fd(fdw));
    {
        va_list vargs;
        va_start(vargs, options);
        handle_timeout_event(iomon, options, vargs);
        va_end(vargs);
    }
    a6i_ioev_attach_k(udata);
    return (epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, a6_fdwrap_fd(fdw), &ev) == 0) ? a6_fdwrap_mark_reg(fdw) : a6_fdwrap_mark_err(fdw);
}

int a6_prepare_read_oneshot(struct a6_iomonitor *iomon, struct a6_uthread *uth, int fd, struct a6_waitk *udata, uint32_t options, ...) {
    struct epoll_event ev;
    (ev.events = ((EPOLLIN|build_ep_events_aux(options)))|EPOLLONESHOT), (ev.data.fd = fd);
    udata->evres = ev.events;
    {
        va_list vargs;
        va_start(vargs, options);
        handle_timeout_event(iomon, options, vargs);
        va_end(vargs);
    }
    a6i_ioev_attach_k(udata);
    if (epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        int errsv = errno;
        return (errsv == EEXIST) ? (epoll_ctl(iomon->epfd, EPOLL_CTL_MOD, fd, &ev) != -1) : 0;
    }
    return 1;
}

int a6_prepare_write_oneshot(struct a6_iomonitor *iomon, struct a6_uthread *uth, int fd, struct a6_waitk *udata, uint32_t options, ...) {
    struct epoll_event ev;
    (ev.events = ((EPOLLOUT|build_ep_events_aux(options)))|EPOLLONESHOT), (ev.data.fd = fd);
    udata->evres = ev.events;
    {
        va_list vargs;
        va_start(vargs, options);
        handle_timeout_event(iomon, options, vargs);
        va_end(vargs);
    }
    a6i_ioev_attach_k(udata);
    if (epoll_ctl(iomon->epfd, EPOLL_CTL_MOD, fd, &ev) == -1) {
        int errsv = errno;
        return (errsv == ENOENT) ? (epoll_ctl(iomon->epfd, EPOLL_CTL_ADD, fd, &ev) != -1) : 0;
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
    //iomon->current_state.timeout = -1;
    ioext_run(iomon, IDX_IOEXT_PREPOLL);
    int nfds = epoll_wait(iomon->epfd, iomon->epevents, iomon->cap, iomon->current_state.timeout);
    ioext_run(iomon, IDX_IOEXT_PRETIMED);
    if (unlikely(nfds == -1))
        return 0;
    // TODO timed events
    ioext_run(iomon, IDX_IOEXT_PREIO);
    for (int i = 0; i < nfds; i++) {
        // TODO cleanup & refactor such mess
        int fd_target = iomon->epevents[i].data.fd;
        if (fd_target != A6_FD_DUMMY) {
            struct link_index k_targets;
            a6_ioev_pick(iomon->evtbl, fd_target, &k_targets);
            list_foreach(&k_targets) {
                __auto_type k = intrusive_ref(struct a6_waitk);
                struct a6_ioevent ioev = { .type = A6_IOEV_EP, .fd = a6_waitk_p_fd(k), .udata = k };
                collect(&ioev, res_groups, n_res_groups);
            }
        } else {
            struct a6_waitk *k = a6_ioev_pick_(iomon->evtbl, fd_target);
            struct a6_ioevent ioev = { .type = A6_IOEV_CR, .fd = a6_waitk_p_fd(k), .udata = k };
            collect(&ioev, res_groups, n_res_groups);
        }
    }
    ioext_run(iomon, IDX_IOEXT_POSTED);
    return 1;
#undef ioext_run
}

void a6_attach_ioext_hook(struct a6_iomonitor *iomon, struct a6_ioext_act *act, uint32_t timing) {
    struct link_index *target = &(iomon->ioext_chains[timing]);
    list_add_tail(intrusion_from_ptr(act), target);
}
