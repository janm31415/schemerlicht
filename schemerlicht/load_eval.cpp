#include "load_eval.h"
#include "compile_data.h"
#include "types.h"
#include "repl_data.h"
#include "context.h"

COMPILER_BEGIN

namespace
  {
  
  uint64_t run_file(const char* filename,environment<environment_entry>* env, repl_data* rd)
    {
    uint64_t return_value = scheme_undefined;
    return return_value;
    }
  
  }

uint64_t c_prim_load(const char* filename, uint64_t context_address, uint64_t repl_data_address, uint64_t env_address)
  {
  context* ctxt = reinterpret_cast<context*>(context_address);
  repl_data* rd = reinterpret_cast<repl_data*>(repl_data_address);
  
  environment<environment_entry>* env = reinterpret_cast<environment<environment_entry>*>(env_address);
  
  
  uint64_t* rsp_save = ctxt->rsp_save; // state of RSP should be preserved, as it might contain RET information and so on
  uint64_t* stack_save = ctxt->stack_save; // stack_save contains the stack position at the beginning of the scheme call. After the scheme call the stack should be at this position again. Therefore this value needs to be preserved.
  uint64_t* error_label = ctxt->error_label; // Each scheme call has its error label to which to jump in case of error. It should thus be preserved.
  
  auto it = env->begin();
  auto it_end = env->end();

  struct variables_data
    {
    std::string name;
    alpha_conversion_data acd;
    environment_entry e;
    };

  std::vector<variables_data> unresolved_variables;
  std::vector<variables_data> reserved_variables;

  for (; it != it_end; ++it)
    {
    if (it->second.st == environment_entry::st_global)
      {
      uint64_t* addr = ctxt->globals + (it->second.pos >> 3);
      if (*addr == unresolved_tag)
        {
        std::string var_name = get_variable_name_before_alpha(it->first);
        variables_data uvd;
        uvd.name = var_name;
        bool res = rd->alpha_conversion_env->find(uvd.acd, var_name);
        if (!res)
          throw std::runtime_error("compiler error in c_prim_load");
        res = env->find(uvd.e, uvd.acd.name);
        if (!res)
          throw std::runtime_error("compiler error in c_prim_load");
        unresolved_variables.push_back(uvd);
        rd->alpha_conversion_env->remove(var_name);
        }
      else if (*addr == reserved_tag)
        {
        std::string var_name = get_variable_name_before_alpha(it->first);
        variables_data uvd;
        uvd.name = var_name;
        bool res = rd->alpha_conversion_env->find(uvd.acd, var_name);
        if (!res)
          throw std::runtime_error("compiler error in c_prim_load");
        res = env->find(uvd.e, uvd.acd.name);
        if (!res)
          throw std::runtime_error("compiler error in c_prim_load");
        reserved_variables.push_back(uvd);
        rd->alpha_conversion_env->remove(var_name);
        }
      else if (*addr == unalloc_tag)
        {
        throw std::runtime_error("compiler error in c_prim_load");
        }
      }
    }

  for (const auto& pr : unresolved_variables)
    env->remove(pr.acd.name);
  for (const auto& pr : reserved_variables)
    env->remove(pr.acd.name);
  
  uint64_t res = run_file(filename, env, rd);
  
  /*
   We run over the dangling variables and update the variables that were resolved by loading 'filename'.
   */
  for (auto unresolved : unresolved_variables)
    {
    environment_entry new_e;
    alpha_conversion_data new_acd;

    if (rd->alpha_conversion_env->find(new_acd, unresolved.name))
      {
      // the variable can be resolved and updated with a new value
      if (!env->find(new_e, new_acd.name))
        throw std::runtime_error("compiler error in c_prim_load");
      uint64_t* unresolved_addr_in_parent = ctxt->globals + (unresolved.e.pos >> 3);
      uint64_t* addr_in_child = ctxt->globals + (new_e.pos >> 3);
      *unresolved_addr_in_parent = *addr_in_child;
      }
    else
      {
      // We could not resolve the variable. However, we push it back to the environment, as we removed it earlier on before calling the actual load.
      rd->alpha_conversion_env->push(unresolved.name, unresolved.acd);
      env->push(unresolved.acd.name, unresolved.e);
      }
    }

  for (auto reserved : reserved_variables)
    {
    rd->alpha_conversion_env->push(reserved.name, reserved.acd);
    env->push(reserved.acd.name, reserved.e);
    }
    
  ctxt->rsp_save = rsp_save;
  ctxt->stack_save = stack_save;
  ctxt->error_label = error_label;
  return res;
  
  }
  
uint64_t c_prim_eval(const char* script, uint64_t context_address, uint64_t repl_data_address, uint64_t env_address)
  {
  uint64_t return_value = scheme_undefined;
  
  return return_value;
  }

COMPILER_END
