#ifndef SCHEMERLICHT_STRINGVEC_H
#define SCHEMERLICHT_STRINGVEC_H

#include "schemerlicht.h"
#include "vector.h"
#include "string.h"

SCHEMERLICHT_API void schemerlicht_string_vector_sort(schemerlicht_vector* v);
SCHEMERLICHT_API schemerlicht_string* schemerlicht_lower_bound(schemerlicht_string* first, schemerlicht_string* last, schemerlicht_string* value);
SCHEMERLICHT_API int schemerlicht_string_vector_binary_search(schemerlicht_vector* v, schemerlicht_string* s);
SCHEMERLICHT_API void schemerlicht_string_vector_insert_sorted(schemerlicht_context* ctxt, schemerlicht_vector* v, schemerlicht_string* s);
SCHEMERLICHT_API schemerlicht_string* schemerlicht_string_vector_unique(schemerlicht_vector* v);

#endif //SCHEMERLICHT_STRINGVEC_H