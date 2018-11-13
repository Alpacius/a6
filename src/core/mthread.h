#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <semaphore.h>
#include    <common/list.h>
#include    <core/scheduler.h>

struct a6_mthread_pool;

struct a6_mthread {
    intrusive;
    pthread_t ptid;
    int type, status;
    struct a6_scheduler *sched;
    struct a6_mthread_pool *pool;
    struct {
        uint32_t seq;
        sem_t wchan;
    } aux;
};

#define     A6_MTH_PRELAUNCH        0
#define     A6_MTH_RUNNING          1
#define     A6_MTH_BLOCKING         2

#define     A6_MTH_CARRIER          0
#define     A6_MTH_FOLLOWER         1

struct a6_packed_word {
    uint32_t tok;
    struct a6_mthread *mth;
};

struct a6_prom_queue {
    uint32_t size;
    sem_t sem_rq_l, sem_rq_f;
    struct a6_packed_word *q;
};

struct a6_mthread_pool {
    uint32_t cap, size;
    struct link_index mthlist;
    struct a6_prom_queue priq;
};
