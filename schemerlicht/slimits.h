#ifndef SCHEMERLICHT_LIMITS_H
#define SCHEMERLICHT_LIMITS_H

#include "schemerlicht.h"

#ifndef UNUSED
#define UNUSED(x) ((void)(x)) /* to avoid warnings */
#endif

#ifndef cast
#define cast(t, exp) ((t)(exp))
#endif

#ifndef schemerlicht_assert
#define schemerlicht_assert(c)  /* empty */
#endif

#ifndef SCHEMERLICHT_ALIGNMENT
typedef union { schemerlicht_fixnum fx; schemerlicht_flonum fl; void* ptr; } schemerlicht_alignment;
#else
typedef SCHEMERLICHT_ALIGNMENT schemerlicht_alignment;
#endif

#ifndef schemerlicht_memsize
#define schemerlicht_memsize uint32_t
#endif

#endif //SCHEMERLICHT_LIMITS_H