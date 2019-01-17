#pragma once

#include    <core/waitk.h>
#include    <core/evslot.h>


static inline
void a6_ioev_pick(struct a6_evslots *tbl, int key, struct link_index *storage) {
    struct link_index *branch = a6_evslots_peek(tbl, key);
    list_init(storage);
    list_foreach_remove(branch) {
        struct a6_waitk *k = intrusive_ref(struct a6_waitk);
        int fd = a6_waitk_p_fd(k);
        if (key == fd) {
            list_del(intrusion_from_ptr(k));
            list_add_tail(intrusion_from_ptr(k), storage);
        }
    }
}

static inline
void a6_ioev_add(struct a6_evslots *tbl, struct a6_waitk *k) {
    a6_evslots_add(tbl, a6_waitk_p_fd(k), intrusion_from_ptr(k));
}
