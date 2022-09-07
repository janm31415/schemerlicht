#ifndef TOKEN_TESTS_H
#define TOKEN_TESTS_H

#include "schemerlicht/svector.h"

struct schemerlicht_context;

schemerlicht_vector script2tokens(schemerlicht_context* ctxt, const char* script);

void run_all_token_tests();

#endif // TOKEN_TESTS_H