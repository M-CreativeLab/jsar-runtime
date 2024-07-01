#pragma once

#include <unistd.h>

#ifndef TR_UNLIKELY
#if defined(__GNUC__) || defined(__clang__)
#define TR_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define TR_UNLIKELY(x) (x)
#endif
#endif
