#pragma once

#include "namespace.h"
#include "vm_api.h"
#include "vmcode.h"

#include <vector>

VM_BEGIN

VM_API void peephole_optimization(VM::vmcode& code, const std::vector<vmcode::operand>& temporary_operands);

VM_END
