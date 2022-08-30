#include "compile_tests.h"
#include "test_assert.h"

#include "vm/vm.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <map>
#include <stdint.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>

#include "schemescript/alpha_conversion.h"
#include "schemescript/compiler_options.h"
#include "schemescript/compile_data.h"
#include "schemescript/compiler.h"
#include "schemescript/context.h"
#include "schemescript/define_conversion.h"
#include "schemescript/dump.h"
#include "schemescript/format.h"
#include "schemescript/globals.h"
#include "schemescript/macro_data.h"
#include "schemescript/parse.h"
#include "schemescript/preprocess.h"
#include "schemescript/primitives.h"
#include "schemescript/repl_data.h"
#include "schemescript/runtime.h"
#include "schemescript/simplify_to_core.h"
#include "schemescript/syscalls.h"
#include "schemescript/tokenize.h"
#include "schemescript/types.h"
#include "schemescript/visitor.h"

#include <chrono>

COMPILER_BEGIN

using namespace VM;

namespace
  {
  struct compile_fixture
    {
    compiler_options ops;
    context ctxt;
    repl_data rd;
    macro_data md;
    std::shared_ptr<environment<environment_entry>> env;
    bool stream_out;
    std::vector<std::pair<void*, uint64_t>> compiled_bytecode;
    primitive_map pm;
    registers reg;
    std::map<std::string, COMPILER::external_function> externals;
    std::vector<VM::external_function> externals_for_vm;

    compile_fixture()
      {
      stream_out = false;
      ctxt = create_context(1024 * 1024, 1024, 1024, 1024);
      env = std::make_shared<environment<environment_entry>>(nullptr);

      vmcode code;
      /*
      try
        {
        compile_primitives_library(pm, rd, env, ctxt, code, ops);
        uint64_t size;
        first_pass_data d;
        uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
#ifdef _WIN32
        reg.rcx = (uint64_t)(&ctxt);
#else
        reg.rdi = (uint64_t)(&ctxt);
#endif
        run_bytecode(f, size, reg);
        compiled_bytecode.emplace_back(f, size);
        assign_primitive_addresses(pm, d, (uint64_t)f);
        }
      catch (std::logic_error e)
        {
        std::cout << e.what() << " while compiling primitives library\n\n";
        }
      */
      }

    virtual ~compile_fixture()
      {
      TEST_ASSERT(ctxt.stack == ctxt.stack_top);
      for (auto& f : compiled_bytecode)
        free_bytecode((void*)f.first, f.second);
      destroy_macro_data(md);
      destroy_context(ctxt);
      }

    vmcode get_vmcode(const std::string& script)
      {
      vmcode code;
      auto tokens = tokenize(script);
      std::reverse(tokens.begin(), tokens.end());
      Program prog;
      try
        {
        prog = make_program(tokens);
        }
      catch (std::logic_error e)
        {
        code.clear();
        throw e;
        }
      try
        {
        compile(env, rd, md, ctxt, code, prog, pm, externals, ops);
        }
      catch (std::logic_error e)
        {
        code.clear();
        throw e;
        }
      return code;
      }
      
    std::string run(const std::string& script, bool no_stack_info = true, int precision = 6)
      {
      vmcode code;
      try
        {
        code = get_vmcode(script);
        }
      catch (std::logic_error e)
        {
        return e.what();
        }
      if (stream_out)
        code.stream(std::cout);
      uint64_t size;
      first_pass_data d;
      uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
      std::stringstream str;
      str << std::setprecision(precision);
#ifdef _WIN32
      reg.rcx = (uint64_t)(&ctxt);
#else
      reg.rdi = (uint64_t)(&ctxt);
#endif
      try {
        run_bytecode(f, size, reg, externals_for_vm);
        }
      catch (std::logic_error e)
        {
        std::cout << e.what() << "\n";
        }
      uint64_t res = reg.rax;
      if (no_stack_info)
        scheme_runtime(res, str, env, rd, nullptr);
      else
        scheme_runtime(res, str, env, rd, &ctxt);
      compiled_bytecode.emplace_back(f, size);
      return str.str();
      }
    };

  struct fixnums : public compile_fixture
    {
    void test()
      {
      TEST_EQ("45", run("(45)"));
      TEST_EQ("-1", run("(-1)"));
      TEST_EQ("100", run("100"));
      TEST_EQ("4611686018427387903", run("4611686018427387903"));
      TEST_EQ("-4611686018427387904", run("-4611686018427387904"));
      TEST_EQ("-1", run("18446744073709551615"));
      TEST_EQ("-1", run("-1"));
      TEST_EQ("-123", run("-123"));
      TEST_EQ("-1", run("(18446744073709551615)"));
      TEST_EQ("-1", run("(-1)"));
      TEST_EQ("-123", run("(-123)"));
      }
    };
    
  }
  
COMPILER_END

void run_all_compile_tests()
  {
  using namespace COMPILER;
  fixnums().test();
  }
