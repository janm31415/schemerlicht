#include "compile_data.h"
#include "context.h"

COMPILER_BEGIN

compile_data create_compile_data(uint64_t heap_size, uint64_t globals_stack, uint32_t local_stack, context* p_ctxt)
  {
  compile_data cd;
  std::vector<VM::vmcode::operand> usable_registers;
  
  usable_registers.push_back(VM::vmcode::RCX);
  usable_registers.push_back(VM::vmcode::RDX);
  usable_registers.push_back(VM::vmcode::RSI);
  usable_registers.push_back(VM::vmcode::RDI);
  usable_registers.push_back(VM::vmcode::R8);
  usable_registers.push_back(VM::vmcode::R9);
  usable_registers.push_back(VM::vmcode::R12);
  usable_registers.push_back(VM::vmcode::R14);
  
  cd.ra = std::make_unique<reg_alloc>(usable_registers, local_stack);
  cd.local_stack_size = local_stack;
  cd.globals_stack = globals_stack;
  cd.heap_size = heap_size;
  cd.first_ra_map_time_point_to_elapse = (uint64_t)-1;

  cd.p_ctxt = p_ctxt;
  return cd;
  }

COMPILER_END
