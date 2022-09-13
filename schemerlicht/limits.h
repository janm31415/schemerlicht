#ifndef SCHEMERLICHT_LIMITS_H
#define SCHEMERLICHT_LIMITS_H

#include "schemerlicht.h"
#include <cassert>

#ifndef UNUSED
#define UNUSED(x) ((void)(x)) /* to avoid warnings */
#endif

#ifndef cast
#define cast(t, exp) ((t)(exp))
#endif

#ifndef schemerlicht_assert
#define schemerlicht_assert(c) assert(c)
#endif

#ifndef SCHEMERLICHT_ALIGNMENT
typedef union { schemerlicht_fixnum fx; schemerlicht_flonum fl; void* ptr; } schemerlicht_alignment;
#else
typedef SCHEMERLICHT_ALIGNMENT schemerlicht_alignment;
#endif

#ifndef schemerlicht_memsize
#define schemerlicht_memsize uint32_t
#endif

#ifndef schemerlicht_mem_bits
#define schemerlicht_mem_bits 32
#endif

#ifndef schemerlicht_mem_invalid_size
#define schemerlicht_mem_invalid_size ((schemerlicht_memsize)(-1))
#endif

#define swap(a, b, type) \
  { type tmp = (a); \
  (a) = (b); \
  (b) = tmp; }

#endif //SCHEMERLICHT_LIMITS_H