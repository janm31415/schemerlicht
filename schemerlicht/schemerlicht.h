#ifndef SCHEMERLICHT_H
#define SCHEMERLICHT_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

typedef struct schemerlicht_context schemerlicht_context;

#ifndef SCHEMERLICHT_FLONUM
typedef double schemerlicht_flonum;
#else
typedef SCHEMERLICHT_FLONUM schemerlicht_flonum;
#endif

#ifndef SCHEMERLICHT_FIXNUM
typedef int64_t schemerlicht_fixnum;
#else
typedef SCHEMERLICHT_FIXNUM schemerlicht_fixnum;
#endif

typedef unsigned char schemerlicht_byte;

#ifndef SCHEMERLICHT_API
#define SCHEMERLICHT_API extern
#endif

#define SCHEMERLICHT_DEBUG_LAMBDA_DEFINITION

#endif //SCHEMERLICHT_H