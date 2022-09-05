#ifndef SCHEMERLICHT_TOKEN_H
#define SCHEMERLICHT_TOKEN_H

#include "schemerlicht.h"

schemerlicht_flonum to_flonum(const char* value);
int is_number(int* is_real, int* is_scientific, const char* value);

#endif //SCHEMERLICHT_TOKEN_H