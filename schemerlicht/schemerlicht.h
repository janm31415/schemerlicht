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

//#define SCHEMERLICHT_DEBUG_LAMBDA_DEFINITION
//#define SCHEMERLICHT_MAKE_NEW_STACK_DURING_LOAD
#define SCHEMERLICHT_USE_INLINES
#define SCHEMERLICHT_DIRECT_CALL_THREADING 1
#define SCHEMERLICHT_SWITCH_FUN_DISPATCHER 2
#define SCHEMERLICHT_SWITCH_DISPATCHER 3
#define SCHEMERLICHT_DISPATCH_TYPE SCHEMERLICHT_SWITCH_DISPATCHER
//#define SCHEMERLICHT_CHECK_HEAP_OVERFLOW
//#define SCHEMERLICHT_DEBUG

#endif //SCHEMERLICHT_H