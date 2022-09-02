#include "schemerlicht.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <fstream>

#include "vm/vmcode.h"
#include "vm/vm.h"
#include "asm_aux.h"
#include "context.h"
#include "load_lib.h"
#include "macro_data.h"
#include "cinput_data.h"
#include "parse.h"
#include "preprocess.h"
#include "primitives_lib.h"
#include "runtime.h"
#include "tokenize.h"
#include "compiler.h"
#include "types.h"
#include "dump.h"
#include "format.h"
#include "syscalls.h"

#include "concurrency.h"
#include "file_utils.h"

COMPILER_BEGIN

using namespace VM;

namespace
  {
  struct compiler_data
    {
    compiler_data() : initialized(false), trace(nullptr) {}

    bool initialized;
    compiler_options ops;
    context ctxt;
    std::list<context> ctxt_clones;
    repl_data rd;
    macro_data md;
    std::shared_ptr<environment<environment_entry>> env;
    std::vector<std::pair<void*, uint64_t>> compiled_bytecode;
    primitive_map pm;
    std::map<std::string, external_function> externals;
    std::vector<VM::external_function> externals_for_vm;
    std::ostream* trace;
    std::ostream* stderror;
    std::ostream* stdoutput;
    };

  void destroy_contexts(compiler_data& cd)
    {
    destroy_context(cd.ctxt);
    for (auto& ctxt : cd.ctxt_clones)
      destroy_context(ctxt);
    }

  struct compiler_data_memento
    {
    uint64_t* rsp_save; // state of RSP should be preserved, as it might contain RET information and so on
    uint64_t* stack_save; // stack_save contains the stack position at the beginning of the scheme call. After the scheme call the stack should be at this position again. Therefore this value needs to be preserved.
    uint64_t* error_label; // Each scheme call has its error label to which to jump in case of error. It should thus be preserved.
    };

  static combinable<std::vector< compiler_data_memento>> compiler_data_memento_vector;

  struct external_primitive
    {
    std::string name;
    std::string help_text;
    };

  static std::string help = R"(This is Skiwi. You are interacting with the REPL.
Enter scheme commands or one of the following:

,asm
,env
,exit
,expand
,external
,mem
,unresolved

)";
  static std::string welcome_message = "\nWelcome to Skiwi\nType ,? for help.\n";
  static std::string prompt = "schemerlicht> ";
  static compiler_data cd;
  static std::vector<external_primitive> external_primitives;

  template<typename ...Args>
  void log(Args && ...args)
    {
    if (cd.trace)
      ((*cd.trace) << ... << args);
    }

  template<typename ...Args>
  void out(Args && ...args)
    {
    if (cd.stdoutput)
      ((*cd.stdoutput) << ... << args);
    }

  template<typename ...Args>
  void err(Args && ...args)
    {
    if (cd.stderror)
      ((*cd.stderror) << ... << args);
    }

  void compile_primitives_library()
    {
    using namespace COMPILER;
    vmcode code;
    try
      {
      log(prompt.c_str(), "compiling primitives library...\n");
      compile_primitives_library(cd.pm, cd.rd, cd.env, cd.ctxt, code, cd.ops);
      first_pass_data d;
      uint64_t size;

      registers reg;
      uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
      reg.rcx = (uint64_t)(&cd.ctxt);
      run_bytecode(f, size, reg);
      cd.compiled_bytecode.emplace_back(f, size);
      assign_primitive_addresses(cd.pm, d, (uint64_t)f);
      }
    catch (std::logic_error e)
      {
      for (auto& f : cd.compiled_bytecode)
        free_bytecode((void*)f.first, f.second);
      destroy_contexts(cd);
      std::stringstream ss;
      ss << e.what() << " while compiling primitives library";
      throw std::logic_error(ss.str().c_str());
      }
    }

  void compile_string_to_symbol()
    {
    using namespace COMPILER;
    vmcode code;
    try
      {
      log(prompt.c_str(), "compiling string-to-symbol...\n");
      if (load_string_to_symbol(cd.env, cd.rd, cd.md, cd.ctxt, code, cd.pm, cd.ops))
        {
        first_pass_data d;
        uint64_t size;
        registers reg;
        uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
        reg.rcx = (uint64_t)(&cd.ctxt);
        run_bytecode(f, size, reg);
        cd.compiled_bytecode.emplace_back(f, size);
        }
      else
        err("Could not compile symbol-table.scm\n");
      }
    catch (std::logic_error e)
      {
      for (auto& f : cd.compiled_bytecode)
        free_bytecode((void*)f.first, f.second);
      destroy_contexts(cd);
      std::stringstream ss;
      ss << e.what() << " while compiling string-to-symbol";
      throw std::logic_error(ss.str().c_str());
      }
    }

  void compile_apply()
    {
    using namespace COMPILER;
    vmcode code;
    try
      {
      log(prompt.c_str(), "compiling apply...\n");
      if (load_apply(cd.env, cd.rd, cd.md, cd.ctxt, code, cd.pm, cd.ops))
        {
        first_pass_data d;
        uint64_t size;
        registers reg;
        uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
        reg.rcx = (uint64_t)(&cd.ctxt);
        run_bytecode(f, size, reg);
        cd.compiled_bytecode.emplace_back(f, size);
        }
      else
        err("Could not compile apply.scm\n");
      }
    catch (std::logic_error e)
      {
      for (auto& f : cd.compiled_bytecode)
        free_bytecode((void*)f.first, f.second);
      destroy_contexts(cd);
      std::stringstream ss;
      ss << e.what() << " while compiling apply";
      throw std::logic_error(ss.str().c_str());
      }
    }

  void compile_call_cc()
    {
    using namespace COMPILER;
    vmcode code;
    try
      {
      log(prompt.c_str(), "compiling call/cc library...\n");
      if (load_callcc(cd.env, cd.rd, cd.md, cd.ctxt, code, cd.pm, cd.ops))
        {
        first_pass_data d;
        uint64_t size;
        registers reg;
        uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
        reg.rcx = (uint64_t)(&cd.ctxt);
        run_bytecode(f, size, reg);
        cd.compiled_bytecode.emplace_back(f, size);
        }
      else
        err("Could not compile callcc.scm\n");
      }
    catch (std::logic_error e)
      {
      for (auto& f : cd.compiled_bytecode)
        free_bytecode((void*)f.first, f.second);
      destroy_contexts(cd);
      std::stringstream ss;
      ss << e.what() << " while compiling call/cc library";
      throw std::logic_error(ss.str().c_str());
      }
    }

  void compile_r5rs()
    {
    using namespace COMPILER;
    vmcode code;
    try
      {
      log(prompt.c_str(), "compiling r5rs library...\n");
      if (load_r5rs(cd.env, cd.rd, cd.md, cd.ctxt, code, cd.pm, cd.ops))
        {
        first_pass_data d;
        uint64_t size;
        registers reg;
        uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
        reg.rcx = (uint64_t)(&cd.ctxt);
        run_bytecode(f, size, reg);
        cd.compiled_bytecode.emplace_back(f, size);
        }
      else
        err("Could not compile r5rs.scm\n");
      }
    catch (std::logic_error e)
      {
      for (auto& f : cd.compiled_bytecode)
        free_bytecode((void*)f.first, f.second);
      destroy_contexts(cd);
      std::stringstream ss;
      ss << e.what() << " while compiling r5rs library";
      throw std::logic_error(ss.str().c_str());
      }
    }

  void compile_modules()
    {
    using namespace COMPILER;
    vmcode code;
    try
      {
      log(prompt.c_str(), "compiling modules...\n");
      if (load_modules(cd.env, cd.rd, cd.md, cd.ctxt, code, cd.pm, cd.ops))
        {
        first_pass_data d;
        uint64_t size;
        registers reg;
        uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
        reg.rcx = (uint64_t)(&cd.ctxt);
        run_bytecode(f, size, reg, cd.externals_for_vm);
        cd.compiled_bytecode.emplace_back(f, size);
        }
      else
        err("Could not compile modules.scm\n");
      }
    catch (std::logic_error e)
      {
      for (auto& f : cd.compiled_bytecode)
        free_bytecode((void*)f.first, f.second);
      destroy_contexts(cd);
      std::stringstream ss;
      ss << e.what() << " while compiling modules";
      throw std::logic_error(ss.str().c_str());
      }
    }

  void compile_lib(std::string path)
    {
    using namespace COMPILER;
    vmcode code;
    try
      {
      log(prompt.c_str(), "compiling ", path, " library\n");
      compiler_options ops = cd.ops;
      ops.do_alpha_conversion = false;
      if (load_lib(path, cd.env, cd.rd, cd.md, cd.ctxt, code, cd.pm, cd.ops))
        {
        first_pass_data d;
        uint64_t size;
        registers reg;
        uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
        reg.rcx = (uint64_t)(&cd.ctxt);
        run_bytecode(f, size, reg);
        cd.compiled_bytecode.emplace_back(f, size);
        }
      else
        err("Could not compile ", path, "\n");
      }
    catch (std::logic_error e)
      {
      for (auto& f : cd.compiled_bytecode)
        free_bytecode((void*)f.first, f.second);
      destroy_contexts(cd);
      std::stringstream ss;
      ss << e.what() << " while compiling " << path << " library";
      throw std::logic_error(ss.str().c_str());
      }
    }

  uint8_t* compile(uint64_t& size, const std::string& input, environment_map& env, repl_data& rd)
    {
    using namespace COMPILER;

    vmcode code;
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
      err(e.what(), "\n");
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
      err(e.what(), "\n");
      }
    catch (std::runtime_error e)
      {
      env = env_copy;
      rd = rd_copy;
      code.clear();
      err(e.what(), "\n");
      }
    return nullptr;
    }


  uint64_t compile_and_run(const std::string& input, environment_map& env, repl_data& rd)
    {
    using namespace COMPILER;
    uint64_t result = scheme_undefined;
    uint64_t size;
    auto f = compile(size, input, env, rd);
    if (f)
      {
      registers reg;
      reg.rcx = (uint64_t)(&cd.ctxt);

      run_bytecode(f, size, reg, cd.externals_for_vm);
      result = reg.rax;
      cd.compiled_bytecode.emplace_back(f, size);
      }
    return result;
    }


  bool is_cmd_command(std::string txt)
    {
    auto it = txt.find_first_not_of(' ');
    if (it == std::string::npos)
      return false;
    return (txt[it] == ',');
    }

  std::string get_cleaned_command(std::string txt)
    {
    assert(is_cmd_command(txt));
    auto it = txt.find_first_not_of(' ');
    std::string cleaned = txt.substr(it);
    it = cleaned.find_first_of(' ');
    if (it == std::string::npos)
      return cleaned;
    return cleaned.substr(0, it);
    }

  std::string get_cleaned_first_argument(std::string txt)
    {
    assert(is_cmd_command(txt));
    auto it = txt.find_first_not_of(' ');
    std::string cleaned = txt.substr(it);
    it = cleaned.find_first_of(' ');
    if (it == std::string::npos)
      return std::string();
    cleaned = cleaned.substr(it);
    it = cleaned.find_first_not_of(' ');
    if (it == std::string::npos)
      return std::string();
    cleaned = cleaned.substr(it);
    it = cleaned.find_first_of(' ');
    if (it == std::string::npos)
      return cleaned;
    return cleaned.substr(0, it);
    }

  std::string remove_command(std::string txt)
    {
    assert(is_cmd_command(txt));
    auto it = txt.find_first_not_of(' ');
    std::string cleaned = txt.substr(it);
    it = cleaned.find_first_of(' ');
    if (it == std::string::npos)
      return "";
    return cleaned.substr(it);
    }

  void show_memory(const context& ctxt)
    {
    size_t nr_globals = ctxt.globals_end - ctxt.globals;
    size_t nr_locals = ctxt.number_of_locals;
    size_t heap_size = ctxt.total_heap_size;
    uint32_t nr_of_globals_used = 0;
    uint64_t* ptr = ctxt.globals;
    while (ptr != ctxt.globals_end)
      {
      if (*ptr != unalloc_tag)
        ++nr_of_globals_used;
      ++ptr;
      }
    out("maximum number of global variables: ", nr_globals, "\n");
    out("number of global variables assigned: ", nr_of_globals_used, "\n");
    out("maximum number of local variables: ", nr_locals + 8, "\n");
    out("heap size: ", (double)heap_size / (0.125 * 1000.0 * 1000.0), "Mb\n");
    out("heap semispace size: ", (double)heap_size / (2.0 * 0.125 * 1000.0 * 1000.0), "Mb\n");

    size_t heap_size_used = ctxt.alloc - ctxt.from_space;
    out("heap size used: ", (double)heap_size_used / (0.125 * 1000.0 * 1000.0), "Mb\n");

    if (ctxt.alloc < ctxt.from_space_end)
      {
      size_t heap_size_available_until_overflow = ctxt.from_space_end - ctxt.alloc;
      out("heap available until overflow: ", (double)heap_size_available_until_overflow / (0.125 * 1000.0 * 1000.0), "Mb\n");
      }
    else
      {
      out("current status is heap overflow\n");
      }
    if (ctxt.alloc < ctxt.limit)
      {
      size_t heap_size_before_gc = ctxt.limit - ctxt.alloc;
      out("heap size allocable before garbage collection trigger: ", (double)heap_size_before_gc / (0.125 * 1000.0 * 1000.0), "Mb\n");
      }
    else
      {
      out("garbage collection will be triggered immediately\n");
      }
    }

  void show_environment(const std::shared_ptr<environment<environment_entry>>& env, const repl_data& rd, const context& ctxt)
    {
    using namespace COMPILER;
    for (const auto& item : *env)
      {
      out(item.first);
      if (item.second.st == environment_entry::st_global)
        {
        uint64_t rax = *(ctxt.globals + (item.second.pos >> 3));
        out(" : ");
        std::stringstream ss;
        scheme_runtime(rax, ss, env, rd, &ctxt);
        out(ss.str());
        }
      out("\n");
      }
    }

  external_function::argtype _convert(external_type t)
    {
    switch (t)
      {
      case schemerlicht_bool: return external_function::T_BOOL;
      case schemerlicht_char_pointer: return external_function::T_CHAR_POINTER;
      case schemerlicht_double: return external_function::T_DOUBLE;
      case schemerlicht_int64: return external_function::T_INT64;
      case schemerlicht_void: return external_function::T_VOID;
      case schemerlicht_scm: return external_function::T_SCM;
      }
    return external_function::T_VOID;
    }

  uint64_t schemerlicht_run_raw(const std::string& scheme_expression, environment_map& env, repl_data& rd)
    {
    return compile_and_run(scheme_expression, env, rd);
    }

  uint64_t schemerlicht_runf_raw(const std::string& scheme_file, environment_map& env, repl_data& rd)
    {
    using namespace COMPILER;
    if (!cd.initialized)
      throw std::runtime_error("Skiwi is not initialized");
    vmcode code;
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
      log(prompt.c_str(), "compiling ", filename, "...\n");
      res = schemerlicht_run_raw(file_in_chars, env, rd);
      }
    else
      {
      err("cannot open ", filename, "\n");
      }
#ifdef _WIN32
    SetCurrentDirectoryW(buf);
#else
    chdir(cwd);
#endif
    return res;
    }

  } // anonymous namespace

schemerlicht_parameters::schemerlicht_parameters()
  {
  heap_size = 2 * 1024 * 1024;
  globals_stack = 64 * 1024;
  local_stack = 256;
  scheme_stack = 4096;
  trace = &std::cout;
  stderror = &std::cout;
  stdoutput = &std::cout;
  }

void* scheme_with_schemerlicht(void* (*func)(void*), void* data, schemerlicht_parameters params)
  {
  using namespace COMPILER;

  if (cd.initialized)
    throw std::runtime_error("Skiwi is already initialized");

  std::string modulepath = get_folder(get_executable_path()) + std::string("scm/");

  putenv(std::string("SKIWI_MODULE_PATH"), modulepath);

  cd = compiler_data(); // this step is necessary to clear any potential content in cd if this is the second instantiation of schemerlicht

  cd.initialized = true;
  add_system_calls(cd.externals);
#ifdef _SKIWI_FOR_ARM
  cd.externals_for_vm = convert_externals_to_vm(cd.externals);
#endif
  cd.ctxt = create_context(params.heap_size, params.globals_stack, params.local_stack, params.scheme_stack);
  cd.env = std::make_shared<environment<environment_entry>>(nullptr);
  cd.trace = params.trace;
  cd.stderror = params.stderror;
  cd.stdoutput = params.stdoutput;

  compile_primitives_library();
  compile_string_to_symbol();
  compile_apply();
  compile_call_cc();

  compile_r5rs();
  //compile_modules();

  if (!func)
    return nullptr;
  return func(data);
  }

std::string schemerlicht_expand(const std::string& scheme_expression)
  {
  using namespace COMPILER;
  if (!cd.initialized)
    throw std::runtime_error("Skiwi is not initialized");
  auto env_copy = make_deep_copy(cd.env);
  auto rd_copy = make_deep_copy(cd.rd);
  Program prog;
  try
    {
    auto tokens = tokenize(scheme_expression);
    std::reverse(tokens.begin(), tokens.end());
    prog = make_program(tokens);
    }
  catch (std::logic_error e)
    {
    cd.env = env_copy;
    cd.rd = rd_copy;
    return e.what();
    }
  catch (std::runtime_error e)
    {
    cd.env = env_copy;
    cd.rd = rd_copy;
    return e.what();
    }
  cinput_data cinput;
  preprocess(cd.env, cd.rd, cd.md, cd.ctxt, cinput, prog, cd.pm, cd.ops);
  cd.env = env_copy;
  cd.rd = rd_copy;
  std::stringstream ss;
  dump(ss, prog);
  format_options format_ops;
  format_ops.indent_offset = 2;
  format_ops.min_width = 5;
  format_ops.max_width = 10;
  return format(ss.str(), format_ops);
  }

std::string schemerlicht_assembly(const std::string& scheme_expression)
  {
  using namespace COMPILER;
  if (!cd.initialized)
    throw std::runtime_error("Skiwi is not initialized");
  auto env_copy = make_deep_copy(cd.env);
  auto rd_copy = make_deep_copy(cd.rd);
  vmcode code;
  Program prog;
  try
    {
    auto tokens = tokenize(scheme_expression);
    std::reverse(tokens.begin(), tokens.end());
    prog = make_program(tokens);
    }
  catch (std::logic_error e)
    {
    cd.env = env_copy;
    cd.rd = rd_copy;
    return e.what();
    }
  catch (std::runtime_error e)
    {
    cd.env = env_copy;
    cd.rd = rd_copy;
    return e.what();
    }
  try
    {
    compile(cd.env, cd.rd, cd.md, cd.ctxt, code, prog, cd.pm, cd.externals, cd.ops);
    }
  catch (std::logic_error e)
    {
    cd.env = env_copy;
    cd.rd = rd_copy;
    return e.what();
    }
  catch (std::runtime_error e)
    {
    cd.env = env_copy;
    cd.rd = rd_copy;
    return e.what();
    }
  cd.env = env_copy;
  cd.rd = rd_copy;
  std::stringstream ss;
  code.stream(ss);
  return ss.str();
  }

schemerlicht_compiled_function_ptr schemerlicht_compile(const std::string& scheme_expression)
  {
  using namespace COMPILER;
  uint64_t size;
  auto f = compile(size, scheme_expression, cd.env, cd.rd);
  if (f)
    {
    cd.compiled_bytecode.emplace_back(f, size);
    }
  return (schemerlicht_compiled_function_ptr)f;
  }

void* schemerlicht_get_context()
  {
  return (void*)(&cd.ctxt);
  }

void* schemerlicht_clone_context(void* ctxt)
  {
  context* p_ctxt = (context*)ctxt;
  auto it = cd.ctxt_clones.insert(cd.ctxt_clones.end(), clone_context(*p_ctxt));
  return &(*it);
  }

void schemerlicht_destroy_clone_context(void* ctxt)
  {
  context* p_ctxt = (context*)ctxt;
  destroy_context(*p_ctxt);
  }

uint64_t schemerlicht_run_raw(const std::string& scheme_expression)
  {
  if (!cd.initialized)
    throw std::runtime_error("Skiwi is not initialized");
  return compile_and_run(scheme_expression, cd.env, cd.rd);
  }

std::string schemerlicht_raw_to_string(uint64_t scm_value, std::streamsize precision)
  {
  std::stringstream ss;
  ss << std::setprecision(precision);
  scheme_runtime(scm_value, ss, cd.env, cd.rd, &cd.ctxt);
  return ss.str();
  }

void schemerlicht_run(const std::string& scheme_expression)
  {
  uint64_t value = schemerlicht_run_raw(scheme_expression);
  std::string s = schemerlicht_raw_to_string(value);
  out(s, "\n");
  }

uint64_t schemerlicht_runf_raw(const std::string& scheme_file)
  {
  return schemerlicht_runf_raw(scheme_file, cd.env, cd.rd);
  }

void schemerlicht_runf(const std::string& scheme_file)
  {
  uint64_t value = schemerlicht_runf_raw(scheme_file);
  std::string s = schemerlicht_raw_to_string(value);
  out(s, "\n");
  }

void schemerlicht_show_help()
  {
  out(help);
  }

void schemerlicht_show_expand(const std::string& input)
  {
  std::string expanded = schemerlicht_expand(remove_command(input));
  out(expanded, "\n");
  }

void schemerlicht_show_assembly(const std::string& input)
  {
  std::string assembly = schemerlicht_assembly(remove_command(input));
  out(assembly, "\n");
  }

void schemerlicht_show_external_primitives(const std::string& arg)
  {
  for (const auto& p : external_primitives)
    {
    if (!arg.empty())
      {
      if (p.name.substr(0, arg.length()) != arg)
        continue;
      }
    out("NAME\n");
    out("\t", p.name, "\n");
    if (!p.help_text.empty())
      {
      std::string txt = p.help_text;
      auto it = txt.begin();
      auto it_end = txt.end();
      int count = 0;
      for (; it != it_end; ++it, ++count)
        {
        if (count > 50 && *it == ' ')
          {
          count = 0;
          *it = '\n';
          }
        }
      size_t pos = txt.find("\n");
      while (pos != std::string::npos)
        {
        txt.replace(pos, 1, "\n\t");
        pos = txt.find("\n", pos + 2);
        }
      out("DESCRIPTION\n");
      out("\t", txt, "\n\n");
      }
    }
  }

void schemerlicht_show_unresolved()
  {
  auto it = cd.env->begin();
  auto it_end = cd.env->end();

  for (; it != it_end; ++it)
    {
    if (it->second.st == environment_entry::st_global)
      {
      uint64_t* addr = cd.ctxt.globals + (it->second.pos >> 3);
      if (*addr == unresolved_tag)
        {
        std::string var_name = get_variable_name_before_alpha(it->first);
        out(var_name, "\n");
        }
      }
    }
  }

void schemerlicht_show_memory()
  {
  show_memory(cd.ctxt);
  }

void schemerlicht_show_environment()
  {
  show_environment(cd.env, cd.rd, cd.ctxt);
  }

void schemerlicht_repl(int argc, char** argv)
  {
  using namespace COMPILER;
  if (!cd.initialized)
    throw std::runtime_error("Skiwi is not initialized");

  for (int i = 1; i < argc; ++i)
    {
    std::string filename(argv[i]);
    schemerlicht_runf(filename);
    }

  out(welcome_message);
  std::string input;
  bool quit = false;
  while (!quit)
    {
    out(prompt.c_str());

    std::getline(std::cin, input);

    bool cmd = is_cmd_command(input);
    if (cmd)
      {
      std::string command = get_cleaned_command(input);
      if (command.size() < 2)
        continue;
      switch (command[1])
        {
        case '?':
        {
        if (command == ",?")
          {
          schemerlicht_show_help();
          }
        break;
        }
        case 'a':
        {
        if (command == ",asm")
          {
          schemerlicht_show_assembly(input);
          }
        break;
        }
        case 'e':
        {
        if (command == ",env")
          {
          schemerlicht_show_environment();
          }
        else if (command == ",exit")
          {
          quit = true;
          }
        else if (command == ",expand")
          {
          schemerlicht_show_expand(input);
          }
        else if (command == ",external")
          {
          std::string arg = get_cleaned_first_argument(input);
          schemerlicht_show_external_primitives(arg);
          }
        break;
        }
        case 'm':
        {
        if (command == ",mem")
          {
          schemerlicht_show_memory();
          }
        break;
        }
        case 'u':
        {
        if (command == ",unresolved")
          {
          schemerlicht_show_unresolved();
          }
        break;
        }
        }
      }
    else if (!input.empty())
      {
      schemerlicht_run(input);
      }
    }
  }

std::string schemerlicht_last_global_variable_used()
  {
  using namespace COMPILER;
  if (!cd.initialized)
    throw std::runtime_error("Skiwi is not initialized");
  std::stringstream ss;
  print_last_global_variable_used(ss, cd.env, cd.rd, &cd.ctxt);
  return ss.str();
  }

void schemerlicht_quit()
  {
  using namespace COMPILER;
  if (!cd.initialized)
    throw std::runtime_error("Skiwi is not initialized");
  destroy_macro_data(cd.md);
  for (auto& f : cd.compiled_bytecode)
    free_bytecode((void*)f.first, f.second);
  destroy_contexts(cd);
  cd.initialized = false;
  }

void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, const std::vector<external_type>& arguments, const char* help_text)
  {
  using namespace COMPILER;
  if (!cd.initialized)
    throw std::runtime_error("Skiwi is not initialized");
  external_function ef;
  ef.name = name;
  ef.address = (uint64_t)func_ptr;
  ef.return_type = _convert(return_type);
  for (auto arg : arguments)
    ef.arguments.push_back(_convert(arg));
  cd.externals[ef.name] = ef;
#ifdef _SKIWI_FOR_ARM
  cd.externals_for_vm.push_back(convert_external_to_vm(ef));
#endif
  std::stringstream ss;
  ss << "(define (" << ef.name;
  int max_pars = (int)arguments.size();
  if (max_pars > 26)
    max_pars = 26;
  for (int i = 0; i < max_pars; ++i)
    {
    ss << " " << (char)(i + 97);
    }
  ss << ") (foreign-call " << ef.name;
  for (int i = 0; i < max_pars; ++i)
    {
    ss << " " << (char)(i + 97);
    }
  ss << "))";
  compile_and_run(ss.str(), cd.env, cd.rd);

  external_primitive rf;
  rf.name = ef.name;
  if (help_text)
    rf.help_text = std::string(help_text);
  external_primitives.push_back(rf);
  }

void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, const char* help_text)
  {
  std::vector<external_type> args;
  register_external_primitive(name, func_ptr, return_type, args, help_text);
  }

void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, external_type arg1, const char* help_text)
  {
  std::vector<external_type> args{ {arg1} };
  register_external_primitive(name, func_ptr, return_type, args, help_text);
  }

void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, external_type arg1, external_type arg2, const char* help_text)
  {
  std::vector<external_type> args{ {arg1, arg2} };
  register_external_primitive(name, func_ptr, return_type, args, help_text);
  }

void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, external_type arg1, external_type arg2, external_type arg3, const char* help_text)
  {
  std::vector<external_type> args{ {arg1, arg2, arg3} };
  register_external_primitive(name, func_ptr, return_type, args, help_text);
  }

void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, external_type arg1, external_type arg2, external_type arg3, external_type arg4, const char* help_text)
  {
  std::vector<external_type> args{ {arg1, arg2, arg3, arg4} };
  register_external_primitive(name, func_ptr, return_type, args, help_text);
  }

scm_type::scm_type() : scm_value(scheme_undefined)
  {
  }

scm_type::scm_type(uint64_t rax) : scm_value(rax)
  {

  }

scm_type::~scm_type()
  {

  }

bool scm_type::is_fixnum() const
  {
  return ((scm_value & fixnum_mask) == fixnum_tag);
  }

bool scm_type::is_bool_true() const
  {
  return (scm_value == bool_t);
  }
bool scm_type::is_bool_false() const
  {
  return (scm_value == bool_f);
  }
bool scm_type::is_nil() const
  {
  return (scm_value == nil);
  }
bool scm_type::is_char() const
  {
  return ((scm_value & char_mask) == char_tag);
  }
bool scm_type::is_undefined() const
  {
  return (scm_value == scheme_undefined);
  }
bool scm_type::is_scheme_quiet_undefined() const
  {
  return (scm_value == scheme_quiet_undefined);
  }
bool scm_type::is_eof() const
  {
  return (scm_value == eof_tag);
  }
bool scm_type::is_procedure() const
  {
  return ((scm_value & procedure_mask) == procedure_tag);
  }
bool scm_type::is_error() const
  {
  return ((scm_value & error_mask) == error_tag);
  }
bool scm_type::is_block() const
  {
  return ((scm_value & block_mask) == block_tag);
  }
bool scm_type::is_flonum() const
  {
  if (!is_block())
    return false;
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  return block_header_is_flonum(header);
  }
bool scm_type::is_closure() const
  {
  if (!is_block())
    return false;
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  return block_header_is_closure(header);
  }
bool scm_type::is_pair() const
  {
  if (!is_block())
    return false;
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  return block_header_is_pair(header);
  }
bool scm_type::is_vector() const
  {
  if (!is_block())
    return false;
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  return block_header_is_vector(header);
  }
bool scm_type::is_string() const
  {
  if (!is_block())
    return false;
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  return block_header_is_string(header);
  }
bool scm_type::is_symbol() const
  {
  if (!is_block())
    return false;
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  return block_header_is_symbol(header);
  }
bool scm_type::is_port() const
  {
  if (!is_block())
    return false;
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  return block_header_is_port(header);
  }
bool scm_type::is_promise() const
  {
  if (!is_block())
    return false;
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  return block_header_is_promise(header);
  }

int64_t scm_type::get_fixnum() const
  {
  if (!is_fixnum())
    throw std::runtime_error("schemerlicht error: not a fixnum");
  int64_t value = (int64_t)(((int64_t)scm_value) >> (int64_t)fixnum_shift);
  return value;
  }


double* scm_type::flonum_address() const
  {
  if (!is_flonum())
    throw std::runtime_error("schemerlicht error: not a flonum");
  uint64_t* addr = get_address_from_block(scm_value);
  double* d = reinterpret_cast<double*>((uint64_t*)(addr + 1));
  return d;
  }

double scm_type::get_flonum() const
  {
  return *flonum_address();
  }

double scm_type::get_number() const
  {
  if (is_fixnum())
    return (double)get_fixnum();
  return get_flonum();
  }

std::string scm_type::get_string() const
  {
  if (!is_string())
    throw std::runtime_error("schemerlicht error: not a string");
  std::string s;
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  uint64_t vsize = get_block_size(header);
  for (uint64_t i = 0; i < vsize; ++i)
    {
    uint64_t item = *((uint64_t*)(addr + i + 1));
    for (uint64_t j = 0; j < 8; ++j)
      {
      unsigned char ch = (unsigned char)((item >> (j * 8)) & 255);
      if (ch == 0)
        return s;
      s.push_back(ch);
      }
    }
  return s;
  }

std::vector<scm_type> scm_type::get_vector() const
  {
  if (!is_vector())
    throw std::runtime_error("schemerlicht error: not a vector");
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t header = *addr;
  uint64_t vsize = get_block_size(header);
  std::vector<scm_type> ret;
  ret.reserve(vsize);
  for (uint64_t i = 0; i < vsize; ++i)
    {
    uint64_t item = *((uint64_t*)(addr + i + 1));
    ret.emplace_back(item);
    }
  return ret;
  }

std::pair<scm_type, scm_type> scm_type::get_pair() const
  {
  if (!is_pair())
    throw std::runtime_error("schemerlicht error: not a pair");
  uint64_t* addr = get_address_from_block(scm_value);
  uint64_t first = *((uint64_t*)(addr + 1));
  uint64_t second = *((uint64_t*)(addr + 2));
  std::pair<scm_type, scm_type> out;
  out.first = scm_type(first);
  out.second = scm_type(second);
  return out;
  }

std::vector<scm_type> scm_type::get_list() const
  {
  std::vector<scm_type> out;
  if (is_nil())
    return out;
  if (!is_pair())
    throw std::runtime_error("schemerlicht error: not a list");
  uint64_t* addr = get_address_from_block(scm_value);
  scm_type first(*((uint64_t*)(addr + 1)));
  scm_type second(*((uint64_t*)(addr + 2)));
  out.push_back(first);
  while (!second.is_nil())
    {
    if (!second.is_pair())
      throw std::runtime_error("schemerlicht error: not a list");
    addr = get_address_from_block(second.value());
    first = scm_type(*((uint64_t*)(addr + 1)));
    second = scm_type(*((uint64_t*)(addr + 2)));
    out.push_back(first);
    }
  return out;
  }

std::string scm_type::get_closure_name() const
  {
  assert(is_closure());
  uint64_t* p_glob = cd.ctxt.globals;
  for (; p_glob < cd.ctxt.globals_end; ++p_glob)
    {
    if (*p_glob == scm_value)
      break;
    }
  if (p_glob >= cd.ctxt.globals_end)
    return std::string("");
  uint64_t pos = (p_glob - cd.ctxt.globals) << 3;
  std::pair<std::string, environment_entry> out;
  if (cd.env->find_if(out, [&](const std::pair<std::string, environment_entry>& v)
    {
    if (v.second.st != environment_entry::st_global)
      return false;
    return v.second.pos == pos;
    }))
    {
    return get_variable_name_before_alpha(out.first);
    }
    return std::string("");
  }

scm_type make_fixnum(int64_t value)
  {
  return (uint64_t)int2fixnum(value);
  }

scm_type make_flonum(double value)
  {
  uint64_t* alloc = cd.ctxt.alloc;
  int64_t header = make_block_header(1, T_FLONUM);
  uint64_t scm_value = (uint64_t)alloc;
  scm_value |= block_tag;
  *alloc = header;
  ++alloc;
  *alloc = *(reinterpret_cast<uint64_t*>(&value));
  ++alloc;
  cd.ctxt.alloc = alloc;
  return scm_value;
  }

scm_type make_nil()
  {
  return nil;
  }

scm_type make_true()
  {
  return bool_t;
  }

scm_type make_false()
  {
  return bool_f;
  }

scm_type make_undefined()
  {
  return scheme_undefined;
  }

scm_type make_scheme_quiet_undefined()
  {
  return scheme_quiet_undefined;
  }

scm_type make_pair(scm_type first, scm_type second)
  {
  uint64_t* alloc = cd.ctxt.alloc;
  int64_t header = make_block_header(2, T_PAIR);
  uint64_t scm_value = (uint64_t)alloc;
  scm_value |= block_tag;
  *alloc = header;
  ++alloc;
  *alloc = first.value();
  ++alloc;
  *alloc = second.value();
  ++alloc;
  cd.ctxt.alloc = alloc;
  return scm_value;
  }

scm_type make_list(const std::vector<scm_type>& lst)
  {
  if (lst.empty())
    return make_nil();
  auto it = lst.rbegin();
  auto it_end = lst.rend();
  scm_type out = make_pair(*it, make_nil());
  ++it;
  while (it != it_end)
    {
    out = make_pair(*it, out);
    ++it;
    }
  return out;
  }

scm_type make_string(const std::string& s)
  {
  uint64_t* alloc = cd.ctxt.alloc;
  uint64_t string_size = s.size() / 8 + 1;
  int64_t header = make_block_header(string_size, T_STRING);
  uint64_t scm_value = (uint64_t)alloc;
  scm_value |= block_tag;
  *alloc = header;
  ++alloc;
  for (uint64_t blob_index = 0; blob_index < string_size; ++blob_index)
    {
    uint64_t current_blob = 0;
    uint64_t char_index = blob_index * 8;
    uint64_t char_index_end = std::min<uint64_t>(s.size(), char_index + 8);
    for (uint64_t i = char_index; i < char_index_end; ++i)
      {
      uint64_t b = i - char_index;
      uint8_t ch = (uint8_t)s[i];
      current_blob |= (uint64_t)ch << (8 * b);
      }
    *alloc = current_blob;
    ++alloc;
    }
  cd.ctxt.alloc = alloc;
  return scm_value;
  }

scm_type make_vector(const std::vector<scm_type>& vec)
  {
  uint64_t* alloc = cd.ctxt.alloc;
  int64_t header = make_block_header(vec.size(), T_VECTOR);
  uint64_t scm_value = (uint64_t)alloc;
  scm_value |= block_tag;
  *alloc = header;
  ++alloc;
  for (const auto& value : vec)
    {
    *alloc = value.value();
    ++alloc;
    }
  cd.ctxt.alloc = alloc;
  return scm_value;
  }

void set_prompt(const std::string& prompt_text)
  {
  prompt = prompt_text;
  }

void set_welcome_message(const std::string& welcome_message_text)
  {
  welcome_message = welcome_message_text;
  }

void set_help_text(const std::string& help_text)
  {
  help = help_text;
  }

void save_compiler_data()
  {
  compiler_data_memento cdm;
  cdm.rsp_save = cd.ctxt.rsp_save; // state of RSP should be preserved, as it might contain RET information and so on
  cdm.stack_save = cd.ctxt.stack_save; // stack_save contains the stack position at the beginning of the scheme call. After the scheme call the stack should be at this position again. Therefore this value needs to be preserved.
  cdm.error_label = cd.ctxt.error_label; // Each scheme call has its error label to which to jump in case of error. It should thus be preserved.

  compiler_data_memento_vector.local().push_back(cdm);
  }

void restore_compiler_data()
  {
  compiler_data_memento cdm = compiler_data_memento_vector.local().back();
  compiler_data_memento_vector.local().pop_back();
  cd.ctxt.rsp_save = cdm.rsp_save;
  cd.ctxt.stack_save = cdm.stack_save;
  cd.ctxt.error_label = cdm.error_label;
  }

COMPILER_END
