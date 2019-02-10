#pragma once

#include    <core/errcode.h>

int a6_get_errno(void);

#define     a6_errno            a6_get_errno()
