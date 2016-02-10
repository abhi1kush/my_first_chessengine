#include "header.h"
#ifndef POLYKEYS_H
#define POLYKEYS_H

#ifdef _MSC_VER
#  define U64_POLY(u) (u##ui64)
#else
#  define U64_POLY(u) (u##ULL)
#endif

extern const u64 random64poly[781];

#endif
