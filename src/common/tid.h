#pragma once

#ifndef     _GNU_SOURCE
#define     _GNU_SOURCE
#endif      // _GNU_SOURCE

#include    <stdint.h>
#include    <unistd.h>
#include    <sys/syscall.h>
#include    <common/miscutils.h>

long do_gettid(void);
uint64_t tseq_next(void);
