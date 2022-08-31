#include "primitives_lib.h"
#include "primitives.h"
#include "globals.h"
#include "asm_aux.h"
#include "context.h"
#include "context_defs.h"
#include "types.h"
#include "compile_error.h"
#include "compile_data.h"
#include "compiler.h"

#include <sstream>

COMPILER_BEGIN

void compile_primitives_library(primitive_map& pm, repl_data& rd, environment_map& env, context& ctxt, VM::vmcode& code, const compiler_options& options)
  {
  //std::shared_ptr<environment<alpha_conversion_data>> new_alpha = std::make_shared<environment<alpha_conversion_data>>(rd.alpha_conversion_env);

  compile_data data = create_compile_data(ctxt.total_heap_size, ctxt.globals_end - ctxt.globals, (uint32_t)ctxt.number_of_locals, &ctxt);
  label = 0;
  function_map fm = generate_function_map();
  code.add(VM::vmcode::MOV, CONTEXT, CALLING_CONVENTION_INT_PAR_1);


  for (auto it = fm.begin(); it != fm.end(); ++it)
    {
    environment_entry e;
    e.pos = (uint64_t)rd.global_index * 8;
    ++rd.global_index;
    if (rd.global_index > data.globals_stack) // too many globals declared
      throw_error(too_many_globals);
    e.st = environment_entry::st_global;
    env->push_outer(it->first, e);
    //new_alpha->push_outer(it->first, it->first);

    std::string label_name = "L_" + it->first;
    code.add(VM::vmcode::MOV, VM::vmcode::RAX, VM::vmcode::LABELADDRESS, label_name);
    code.add(VM::vmcode::OR, VM::vmcode::RAX, VM::vmcode::NUMBER, procedure_tag);
    code.add(VM::vmcode::MOV, VM::vmcode::R11, GLOBALS);
    code.add(VM::vmcode::MOV, VM::vmcode::MEM_R11, e.pos, VM::vmcode::RAX);

    // We save the primitive constant twice. The second save has ### in front of its name, and is used during inlining for checking whether the primitive was redefined or not.
    e.pos = (uint64_t)rd.global_index * 8;
    ++rd.global_index;
    if (rd.global_index > data.globals_stack) // too many globals declared
      throw_error(too_many_globals);
    e.st = environment_entry::st_global;
    std::stringstream str;
    str << "###" << it->first;
    env->push_outer(str.str(), e);
    code.add(VM::vmcode::MOV, VM::vmcode::MEM_R11, e.pos, VM::vmcode::RAX);
    }

  /*Return to the caller*/
  code.add(VM::vmcode::RET);

  code.push();
  for (auto it = fm.begin(); it != fm.end(); ++it)
    {
    primitive_entry pe;
    pe.label_name = "L_" + it->first;
    pe.address = 0;
    pm.insert(std::pair<std::string, primitive_entry>(it->first, pe));
    code.add(VM::vmcode::LABEL_ALIGNED, pe.label_name);
    it->second(code, options);
    }
  compile_bitwise_and_2(code, options);
  compile_bitwise_or_2(code, options);
  compile_bitwise_xor_2(code, options);
  compile_add_2(code, options);
  compile_divide_2(code, options);
  compile_multiply_2(code, options);
  compile_subtract_2(code, options);
  compile_equal_2(code, options);
  compile_not_equal_2(code, options);
  compile_less_2(code, options);
  compile_leq_2(code, options);
  compile_geq_2(code, options);
  compile_greater_2(code, options);
  compile_max_2(code, options);
  compile_min_2(code, options);
  compile_fold_binary(code, options);
  compile_pairwise_compare(code, options);
  compile_mark(code, options);
  compile_recursively_equal(code, options, "L_recursively_equal");
  compile_structurally_equal(code, options, "L_structurally_equal");
  compile_member_cmp_eqv(code, options);
  compile_member_cmp_eq(code, options);
  compile_member_cmp_equal(code, options);
  compile_assoc_cmp_eqv(code, options);
  compile_assoc_cmp_eq(code, options);
  compile_assoc_cmp_equal(code, options);
  compile_apply_fake_cps_identity(code, options);
  code.pop();
  }

void assign_primitive_addresses(primitive_map& pm, const VM::first_pass_data& d, uint64_t address_start)
  {
  for (auto& pe : pm)
    {
    auto it = d.label_to_address.find(pe.second.label_name);
    if (it == d.label_to_address.end())
      throw std::runtime_error("Error during primitives library generation");
    pe.second.address = address_start + it->second;
    }
  }

COMPILER_END