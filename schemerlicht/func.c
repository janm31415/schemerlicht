#include "func.h"
#include "vector.h"
#include "object.h"
#include "context.h"
#include "vm.h"

schemerlicht_function* schemerlicht_function_new(schemerlicht_context* ctxt)
  {
  schemerlicht_function* fun = schemerlicht_new(ctxt, schemerlicht_function);
  fun->constants_map = schemerlicht_map_new(ctxt, 0, 8);
  schemerlicht_vector_init(ctxt, &fun->constants, schemerlicht_object);
  schemerlicht_vector_init(ctxt, &fun->code, schemerlicht_instruction);
  schemerlicht_vector_init(ctxt, &fun->lambdas, schemerlicht_function*);  
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
    if (schemerlicht_object_get_type(it) != schemerlicht_object_type_symbol)
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


void schemerlicht_show_last_function(schemerlicht_context* ctxt, schemerlicht_string* s) {
  if (ctxt->lambdas.vector_size == 0)
    return;
  schemerlicht_function** fun = schemerlicht_vector_back(&(ctxt->lambdas), schemerlicht_function*);
  schemerlicht_string str = schemerlicht_fun_to_string(ctxt, *fun);
  schemerlicht_string_append(ctxt, s, &str);
  schemerlicht_string_destroy(ctxt, &str);
}