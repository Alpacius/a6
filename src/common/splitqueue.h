#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <semaphore.h>

#include    <common/tribuffer.h>

struct spqchan {
    pthread_mutex_t mutex;
    sem_t wchan;
    struct tribuffer chan;
};

struct splitqueue {
    uint64_t p;
    struct spqchan dchans[];
};

static inline
struct splitqueue *spqueue_create(uint64_t p) {
    struct splitqueue *spq = malloc(sizeof(struct splitqueue) + sizeof(struct spqchan) * p);
    if (unlikely(spq == NULL))
        return spq;
    if (unlikely(sem_init(&(spq->wchan), 0, 0) == -1))
        return free(spq), NULL;
    for (uint64_t i = 0; i < p; i++)
        tribuffer_init(&(spq->dchans[i].chan)), pthread_mutex_init(&(spq->dchans[i].mutex), NULL), sem_init(&(spq->dchans[i].wchan), 0, 0);
    return spq;
}

static inline
void spqueue_destroy(struct splitqueue *q) {
    sem_destroy(&(q->wchan));
    for (uint64_t i = 0; i < p; i++)
        pthread_mutex_destroy(&(q->dchans[i].mutex));
    free(q);
}

static inline
struct link_index *spqueue_consume(struct splitqueue *q, uint64_t tok) {
    uint64_t tok_real = tok % q->p;
    struct link_index *result = NULL;
    sem_wait(&(q->dchans[tok_real].wchan))
    {
        pthread_mutex_lock(&(q->dchans[tok_real].mutex));
        struct link_index *rdq = tribuffer_rdside(&(q->dchans[tok_real].chan));
        result = rdq->next;
        list_del(result);
        pthread_mutex_unlock(&(q->dchans[tok_real].mutex));
    }
    return result;
}

static inline
void spqueue_produce(struct splitqueue *q, struct link_index *product, uint64_t tok) {
    uint64_t tok_real = tok % q->p;
    {
        pthread_mutex_lock(&(q->dchans[tok_real].mutex));
        struct link_index *wrq = tribuffer_wrside(&(q->dchans[tok_real].chan));
        list_add_tail(wrq, product);
        pthread_mutex_unlock(&(q->dchans[tok_real].mutex));
    }
    sem_post(&(q->dchans[tok_real].wchan));
}
