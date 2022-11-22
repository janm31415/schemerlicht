#ifndef SCHEMERLICHT_GC_H
#define SCHEMERLICHT_GC_H

#include "schemerlicht.h"
#include "func.h"

void schemerlicht_collect_garbage(schemerlicht_context* ctxt);
int schemerlicht_need_to_perform_gc(schemerlicht_context* ctxt);

void schemerlicht_check_garbage_collection(schemerlicht_context* ctxt);

#endif //SCHEMERLICHT_GC_H