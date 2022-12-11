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

#ifndef schemerlicht_memsize
#define schemerlicht_memsize uint32_t
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


SCHEMERLICHT_API schemerlicht_context* schemerlicht_open(schemerlicht_memsize heap_size);
SCHEMERLICHT_API void schemerlicht_close(schemerlicht_context* ctxt);
SCHEMERLICHT_API schemerlicht_context* schemerlicht_context_init(schemerlicht_context* ctxt, schemerlicht_memsize heap_size);
SCHEMERLICHT_API void schemerlicht_context_destroy(schemerlicht_context* ctxt);

SCHEMERLICHT_API void schemerlicht_build_base(schemerlicht_context* ctxt);

typedef struct schemerlicht_object schemerlicht_object;
typedef struct schemerlicht_string schemerlicht_string;

SCHEMERLICHT_API schemerlicht_object* schemerlicht_execute(schemerlicht_context* ctxt, const char* script);
SCHEMERLICHT_API schemerlicht_object* schemerlicht_execute_file(schemerlicht_context* ctxt, const char* filename);

SCHEMERLICHT_API void schemerlicht_get_error_string(schemerlicht_context* ctxt, schemerlicht_string* s);
SCHEMERLICHT_API void schemerlicht_print_any_error(schemerlicht_context* ctxt);

SCHEMERLICHT_API void schemerlicht_show_stack(schemerlicht_context* ctxt, schemerlicht_string* s, int stack_start, int stack_end);
SCHEMERLICHT_API void schemerlicht_show_environment(schemerlicht_context* ctxt, schemerlicht_string* s);
SCHEMERLICHT_API void schemerlicht_show_memory(schemerlicht_context* ctxt, schemerlicht_string* s);
SCHEMERLICHT_API void schemerlicht_show_object(schemerlicht_context* ctxt, schemerlicht_object* obj, schemerlicht_string* s);
#endif //SCHEMERLICHT_H