#include "func.h"
#include "vector.h"
#include "object.h"

schemerlicht_function* schemerlicht_function_new(schemerlicht_context* ctxt)
  {
  schemerlicht_function* fun = schemerlicht_new(ctxt, schemerlicht_function);
  fun->constants_map = schemerlicht_map_new(ctxt, 0, 8);
  schemerlicht_vector_init(ctxt, &fun->constants, schemerlicht_object);
  schemerlicht_vector_init(ctxt, &fun->code, schemerlicht_instruction);
  schemerlicht_vector_init(ctxt, &fun->lambdas, schemerlicht_function*);
  fun->freereg = 0;
  fun->number_of_constants = 0;
#ifdef SCHEMERLICHT_DEBUG_LAMBDA_DEFINITION
  schemerlicht_string_init(ctxt, &fun->function_definition, "");
#endif
  return fun;
  }

void schemerlicht_function_free(schemerlicht_context* ctxt, schemerlicht_function* f)
  {
  schemerlicht_function** lit = schemerlicht_vector_begin(&f->lambdas, schemerlicht_function*);
  schemerlicht_function** lit_end = schemerlicht_vector_end(&f->lambdas, schemerlicht_function*);
  for (; lit != lit_end; ++lit)
    {
    schemerlicht_function_free(ctxt, *lit);
    }
  schemerlicht_vector_destroy(ctxt, &f->lambdas);
  schemerlicht_object* it = schemerlicht_vector_begin(&f->constants, schemerlicht_object);
  schemerlicht_object* it_end = schemerlicht_vector_end(&f->constants, schemerlicht_object);
  for (; it != it_end; ++it)
    {
    schemerlicht_object_destroy(ctxt, it);
    }
  schemerlicht_map_free(ctxt, f->constants_map);
  schemerlicht_vector_destroy(ctxt, &f->constants);
  schemerlicht_vector_destroy(ctxt, &f->code);
#ifdef SCHEMERLICHT_DEBUG_LAMBDA_DEFINITION
  schemerlicht_string_destroy(ctxt, &f->function_definition);
#endif
  schemerlicht_delete(ctxt, f);
  }