#pragma once

#include    <stdint.h>

struct a6_swarm;

struct a6_swarm *a6_swarm_create(uint32_t size);
void a6_swarm_destroy(struct a6_swarm *swarm);
int a6_swarm_run(struct a6_swarm *swarm, void (*func)(void *), void *arg);
