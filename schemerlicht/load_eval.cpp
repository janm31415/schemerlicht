#include "load_eval.h"
#include "compile_data.h"
#include "types.h"
#include "repl_data.h"
#include "context.h"
#include "file_utils.h"
#include "compiler.h"

#include "vm/vmcode.h"
#include "vm/vm.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <fstream>

#include "syscalls.h"

COMPILER_BEGIN
/*
namespace
  {

  uint8_t* _compile(uint64_t& size, const std::string& input, environment<environment_entry>* env, repl_data* rd)
    {
    VM::vmcode code;
    Program prog;
    auto env_copy = make_deep_copy(env);
    auto rd_copy = make_deep_copy(rd);
    try
      {
      auto tokens = tokenize(input);
      std::reverse(tokens.begin(), tokens.end());
      prog = make_program(tokens);
      }
    catch (std::logic_error e)
      {
      env = env_copy;
      rd = rd_copy;
      code.clear();
      //err(e.what(), "\n");
      return nullptr;
      }
    try
      {
      compile(env, rd, cd.md, cd.ctxt, code, prog, cd.pm, cd.externals, cd.ops);
      first_pass_data d;
      uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
      return f;
      }
    catch (std::logic_error e)
      {
      env = env_copy;
      rd = rd_copy;
      code.clear();
      //err(e.what(), "\n");
      }
    catch (std::runtime_error e)
      {
      env = env_copy;
      rd = rd_copy;
      code.clear();
      //err(e.what(), "\n");
      }
    return nullptr;
    }

  VM::external_function::argtype _convert(COMPILER::external_function::argtype arg)
    {
    switch (arg)
      {
      case COMPILER::external_function::T_BOOL: return VM::external_function::T_BOOL;
      case COMPILER::external_function::T_CHAR_POINTER: return VM::external_function::T_CHAR_POINTER;
      case COMPILER::external_function::T_DOUBLE: return VM::external_function::T_DOUBLE;
      case COMPILER::external_function::T_INT64: return VM::external_function::T_INT64;
      case COMPILER::external_function::T_VOID: return VM::external_function::T_VOID;
      case COMPILER::external_function::T_SCM: return VM::external_function::T_INT64;
      default: return VM::external_function::T_VOID;
      }
    }

  std::vector<VM::external_function> _convert_externals_to_vm(const std::map<std::string, COMPILER::external_function>& externals)
    {
    std::vector<VM::external_function> externals_for_vm;
    for (const auto& e : externals)
      {
      VM::external_function ef;
      ef.name = e.second.name;
      ef.address = e.second.address;
      ef.return_type = _convert(e.second.return_type);
      for (auto arg : e.second.arguments)
        {
        ef.arguments.push_back(_convert(arg));
        }
      externals_for_vm.push_back(ef);
      }
    }

  uint64_t _run(const std::string& script, context* ctxt, environment<environment_entry>* env, repl_data* rd)
    {
    std::map<std::string, COMPILER::external_function> externals;
    add_system_calls(externals);
    std::vector<VM::external_function> externals_for_vm = _convert_externals_to_vm(externals);

    uint64_t return_value = scheme_undefined;
    uint64_t size;
    auto f = _compile(size, script, env, rd);
    if (f)
      {
      VM::registers reg;
      reg.rcx = (uint64_t)(ctxt);

      run_bytecode(f, size, reg, externals_for_vm);
      return_value = reg.rax;
      cd.compiled_bytecode.emplace_back(f, size);
      }
    return return_value;
    }
  
  uint64_t _run_file(const char* scheme_file, context* ctxt, environment<environment_entry>* env, repl_data* rd)
    {
    std::string filename = get_filename(scheme_file);
    std::string folder = get_folder(scheme_file);
#ifdef _WIN32
    wchar_t buf[4096];
    GetCurrentDirectoryW(4096, buf);
    std::wstring wfolder = convert_string_to_wstring(folder);
    std::wstring wfilename = convert_string_to_wstring(filename);
    SetCurrentDirectoryW(wfolder.c_str());

    auto input_file = std::ifstream{ wfilename };
#else
    char cwd[4096];
    getcwd(cwd, sizeof(cwd));
    chdir(folder.c_str());
    auto input_file = std::ifstream{ filename };
#endif
    std::string file_in_chars;
    uint64_t res = bool_f;
    if (input_file.is_open())
      {
      std::stringstream ss;
      ss << input_file.rdbuf();
      file_in_chars = ss.str();
      input_file.close();
      res = _run(file_in_chars, ctxt, env, rd);
      }
    else
      {
      // error message?
      }
#ifdef _WIN32
    SetCurrentDirectoryW(buf);
#else
    chdir(cwd);
#endif
    return res;
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
  
  uint64_t res = _run_file(filename, ctxt, env, rd);
  
  
   //We run over the dangling variables and update the variables that were resolved by loading 'filename'.
   
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
  */

uint64_t c_prim_load(const char* script)
  {
  uint64_t return_value = scheme_undefined;

  return return_value;
  }

uint64_t c_prim_eval(const char* script)
  {
  uint64_t return_value = scheme_undefined;
  
  return return_value;
  }

COMPILER_END
