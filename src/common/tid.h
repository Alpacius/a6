#pragma once

#define     _GNU_SOURCE

#include    <unistd.h>
#include    <sys/syscall.h>
#include    <common/miscutils.h>

long do_gettid(void);
