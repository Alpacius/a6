#pragma once

#include    <common/stdc_common.h>
#include    <common/linux_common.h>
#include    <common/list.h>

struct a6_mthread;
struct a6_mthread_pool;

void a6_mthread_pool_destroy(struct a6_mthread_pool *pool);
struct a6_mthread_pool *a6_mthread_pool_create(uint32_t size);
int a6_mthread_pool_launch(struct a6_mthread_pool *pool);
