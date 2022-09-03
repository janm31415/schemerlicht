#pragma once

#include "namespace.h"
#include "vm_api.h"
#include "vmcode.h"

#include <vector>

VM_BEGIN

VM_API void peephole_optimization(VM::vmcode& code);

VM_END
