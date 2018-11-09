#pragma once

#ifndef     A6_COMMON_MCNTX
#ifdef      __x86_64__
#include    <core/mcontext_x64.h>
#else
#error      "No builtin x86-64 mcontext implementation found."
#endif
#else
#error      "Stdc/posix mcontext not supported."
#endif
