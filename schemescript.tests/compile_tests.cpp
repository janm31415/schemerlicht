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
#include "schemescript/primitives_lib.h"
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

compiler_options g_ops;

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
    std::ostream* vm_output = nullptr;

    compile_fixture()
      {
      ops = g_ops;
      stream_out = false;
      ctxt = create_context(1024 * 1024, 1024, 1024, 1024);
      env = std::make_shared<environment<environment_entry>>(nullptr);

      vmcode code;
      
      try
        {
        compile_primitives_library(pm, rd, env, ctxt, code, ops);
        uint64_t size;
        first_pass_data d;
        uint8_t* f = (uint8_t*)vm_bytecode(size, d, code);
        reg.rcx = (uint64_t)(&ctxt);
        run_bytecode(f, size, reg, vm_output);
        compiled_bytecode.emplace_back(f, size);
        assign_primitive_addresses(pm, d, (uint64_t)f);
        }
      catch (std::logic_error e)
        {
        std::cout << e.what() << " while compiling primitives library\n\n";
        }      
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
      reg.rcx = (uint64_t)(&ctxt);
      try {
        run_bytecode(f, size, reg, externals_for_vm, vm_output);
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
    
  struct bools : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run("#t"));
      TEST_EQ("#f", run("#f"));
      TEST_EQ("#t", run("(#t)"));
      TEST_EQ("#f", run("(#f)"));
      }
    };

  struct test_for_nil : public compile_fixture {
    void test()
      {
      TEST_EQ("()", run("()"));
      TEST_EQ("()", run("(())"));
      TEST_EQ("()", run("((()))"));
      TEST_EQ("()", run("(((())))"));
      }
    };

  struct chars : public compile_fixture {
    void test()
      {
      TEST_EQ(R"(#\*)", run(R"(#\*)"));
      TEST_EQ(R"(#\a)", run(R"(#\97)"));

      TEST_EQ(R"(#\*)", run(R"(#\*)"));
      TEST_EQ(R"(#\!)", run(R"(#\!)"));
      TEST_EQ(R"(#\@)", run(R"(#\@)"));
      TEST_EQ(R"(#\#)", run(R"(#\#)"));
      TEST_EQ(R"(#\$)", run(R"(#\$)"));
      TEST_EQ(R"(#\%)", run(R"(#\%)"));
      TEST_EQ(R"(#\^)", run(R"(#\^)"));
      TEST_EQ(R"(#\&)", run(R"(#\&)"));
      TEST_EQ(R"(#\()", run(R"(#\()"));
      TEST_EQ(R"(#\))", run(R"(#\))"));
      TEST_EQ(R"(#\")", run(R"(#\")"));
      TEST_EQ(R"(#\+)", run(R"(#\+)"));
      TEST_EQ(R"(#\-)", run(R"(#\-)"));
      TEST_EQ(R"(#\/)", run(R"(#\/)"));
      TEST_EQ(R"(#\')", run(R"(#\')"));
      TEST_EQ(R"(#\;)", run(R"(#\;)"));
      TEST_EQ(R"(#\:)", run(R"(#\:)"));
      TEST_EQ(R"(#\<)", run(R"(#\<)"));
      TEST_EQ(R"(#\>)", run(R"(#\>)"));
      TEST_EQ(R"(#\.)", run(R"(#\.)"));
      TEST_EQ(R"(#\=)", run(R"(#\=)"));
      TEST_EQ(R"(#\?)", run(R"(#\?)"));

      TEST_EQ(R"(#\a)", run(R"(#\a)"));
      TEST_EQ(R"(#\b)", run(R"(#\b)"));
      TEST_EQ(R"(#\c)", run(R"(#\c)"));
      TEST_EQ(R"(#\d)", run(R"(#\d)"));
      TEST_EQ(R"(#\e)", run(R"(#\e)"));
      TEST_EQ(R"(#\f)", run(R"(#\f)"));
      TEST_EQ(R"(#\g)", run(R"(#\g)"));
      TEST_EQ(R"(#\h)", run(R"(#\h)"));
      TEST_EQ(R"(#\i)", run(R"(#\i)"));
      TEST_EQ(R"(#\j)", run(R"(#\j)"));
      TEST_EQ(R"(#\k)", run(R"(#\k)"));
      TEST_EQ(R"(#\l)", run(R"(#\l)"));
      TEST_EQ(R"(#\m)", run(R"(#\m)"));
      TEST_EQ(R"(#\n)", run(R"(#\n)"));
      TEST_EQ(R"(#\o)", run(R"(#\o)"));
      TEST_EQ(R"(#\p)", run(R"(#\p)"));
      TEST_EQ(R"(#\q)", run(R"(#\q)"));
      TEST_EQ(R"(#\r)", run(R"(#\r)"));
      TEST_EQ(R"(#\s)", run(R"(#\s)"));
      TEST_EQ(R"(#\t)", run(R"(#\t)"));
      TEST_EQ(R"(#\u)", run(R"(#\u)"));
      TEST_EQ(R"(#\v)", run(R"(#\v)"));
      TEST_EQ(R"(#\w)", run(R"(#\w)"));
      TEST_EQ(R"(#\x)", run(R"(#\x)"));
      TEST_EQ(R"(#\y)", run(R"(#\y)"));
      TEST_EQ(R"(#\z)", run(R"(#\z)"));

      TEST_EQ(R"(#\A)", run(R"(#\A)"));
      TEST_EQ(R"(#\B)", run(R"(#\B)"));
      TEST_EQ(R"(#\C)", run(R"(#\C)"));
      TEST_EQ(R"(#\D)", run(R"(#\D)"));
      TEST_EQ(R"(#\E)", run(R"(#\E)"));
      TEST_EQ(R"(#\F)", run(R"(#\F)"));
      TEST_EQ(R"(#\G)", run(R"(#\G)"));
      TEST_EQ(R"(#\H)", run(R"(#\H)"));
      TEST_EQ(R"(#\I)", run(R"(#\I)"));
      TEST_EQ(R"(#\J)", run(R"(#\J)"));
      TEST_EQ(R"(#\K)", run(R"(#\K)"));
      TEST_EQ(R"(#\L)", run(R"(#\L)"));
      TEST_EQ(R"(#\M)", run(R"(#\M)"));
      TEST_EQ(R"(#\N)", run(R"(#\N)"));
      TEST_EQ(R"(#\O)", run(R"(#\O)"));
      TEST_EQ(R"(#\P)", run(R"(#\P)"));
      TEST_EQ(R"(#\Q)", run(R"(#\Q)"));
      TEST_EQ(R"(#\R)", run(R"(#\R)"));
      TEST_EQ(R"(#\S)", run(R"(#\S)"));
      TEST_EQ(R"(#\T)", run(R"(#\T)"));
      TEST_EQ(R"(#\U)", run(R"(#\U)"));
      TEST_EQ(R"(#\V)", run(R"(#\V)"));
      TEST_EQ(R"(#\W)", run(R"(#\W)"));
      TEST_EQ(R"(#\X)", run(R"(#\X)"));
      TEST_EQ(R"(#\Y)", run(R"(#\Y)"));
      TEST_EQ(R"(#\Z)", run(R"(#\Z)"));

      TEST_EQ(R"(#\000)", run(R"(#\000)"));
      TEST_EQ(R"(#\001)", run(R"(#\001)"));
      TEST_EQ(R"(#\002)", run(R"(#\002)"));
      TEST_EQ(R"(#\003)", run(R"(#\003)"));
      TEST_EQ(R"(#\004)", run(R"(#\004)"));
      TEST_EQ(R"(#\005)", run(R"(#\005)"));
      TEST_EQ(R"(#\006)", run(R"(#\006)"));
      TEST_EQ(R"(#\007)", run(R"(#\007)"));
      TEST_EQ(R"(#\008)", run(R"(#\008)"));
      TEST_EQ(R"(#\009)", run(R"(#\009)"));
      TEST_EQ(R"(#\010)", run(R"(#\010)"));
      TEST_EQ(R"(#\011)", run(R"(#\011)"));
      TEST_EQ(R"(#\012)", run(R"(#\012)"));
      TEST_EQ(R"(#\013)", run(R"(#\013)"));
      TEST_EQ(R"(#\014)", run(R"(#\014)"));
      TEST_EQ(R"(#\015)", run(R"(#\015)"));
      TEST_EQ(R"(#\016)", run(R"(#\016)"));
      TEST_EQ(R"(#\017)", run(R"(#\017)"));
      TEST_EQ(R"(#\018)", run(R"(#\018)"));
      TEST_EQ(R"(#\019)", run(R"(#\019)"));

      TEST_EQ(R"(#\*)", run(R"((#\*))"));
      TEST_EQ(R"(#\a)", run(R"((#\97))"));

      TEST_EQ(R"(#\*)", run(R"((#\*))"));
      TEST_EQ(R"(#\!)", run(R"((#\!))"));
      TEST_EQ(R"(#\@)", run(R"((#\@))"));
      TEST_EQ(R"(#\#)", run(R"((#\#))"));
      TEST_EQ(R"(#\$)", run(R"((#\$))"));
      TEST_EQ(R"(#\%)", run(R"((#\%))"));
      TEST_EQ(R"(#\^)", run(R"((#\^))"));
      TEST_EQ(R"(#\&)", run(R"((#\&))"));
      TEST_EQ(R"(#\()", run(R"((#\())"));
      TEST_EQ(R"(#\))", run(R"((#\)))"));
      TEST_EQ(R"(#\")", run(R"((#\"))"));
      TEST_EQ(R"(#\+)", run(R"((#\+))"));
      TEST_EQ(R"(#\-)", run(R"((#\-))"));
      TEST_EQ(R"(#\/)", run(R"((#\/))"));
      TEST_EQ(R"(#\')", run(R"((#\'))"));
      TEST_EQ(R"(#\;)", run(R"((#\;))"));
      TEST_EQ(R"(#\:)", run(R"((#\:))"));
      TEST_EQ(R"(#\<)", run(R"((#\<))"));
      TEST_EQ(R"(#\>)", run(R"((#\>))"));
      TEST_EQ(R"(#\.)", run(R"((#\.))"));
      TEST_EQ(R"(#\=)", run(R"((#\=))"));
      TEST_EQ(R"(#\?)", run(R"((#\?))"));

      TEST_EQ(R"(#\a)", run(R"((#\a))"));
      TEST_EQ(R"(#\b)", run(R"((#\b))"));
      TEST_EQ(R"(#\c)", run(R"((#\c))"));
      TEST_EQ(R"(#\d)", run(R"((#\d))"));
      TEST_EQ(R"(#\e)", run(R"((#\e))"));
      TEST_EQ(R"(#\f)", run(R"((#\f))"));
      TEST_EQ(R"(#\g)", run(R"((#\g))"));
      TEST_EQ(R"(#\h)", run(R"((#\h))"));
      TEST_EQ(R"(#\i)", run(R"((#\i))"));
      TEST_EQ(R"(#\j)", run(R"((#\j))"));
      TEST_EQ(R"(#\k)", run(R"((#\k))"));
      TEST_EQ(R"(#\l)", run(R"((#\l))"));
      TEST_EQ(R"(#\m)", run(R"((#\m))"));
      TEST_EQ(R"(#\n)", run(R"((#\n))"));
      TEST_EQ(R"(#\o)", run(R"((#\o))"));
      TEST_EQ(R"(#\p)", run(R"((#\p))"));
      TEST_EQ(R"(#\q)", run(R"((#\q))"));
      TEST_EQ(R"(#\r)", run(R"((#\r))"));
      TEST_EQ(R"(#\s)", run(R"((#\s))"));
      TEST_EQ(R"(#\t)", run(R"((#\t))"));
      TEST_EQ(R"(#\u)", run(R"((#\u))"));
      TEST_EQ(R"(#\v)", run(R"((#\v))"));
      TEST_EQ(R"(#\w)", run(R"((#\w))"));
      TEST_EQ(R"(#\x)", run(R"((#\x))"));
      TEST_EQ(R"(#\y)", run(R"((#\y))"));
      TEST_EQ(R"(#\z)", run(R"((#\z))"));

      TEST_EQ(R"(#\A)", run(R"((#\A))"));
      TEST_EQ(R"(#\B)", run(R"((#\B))"));
      TEST_EQ(R"(#\C)", run(R"((#\C))"));
      TEST_EQ(R"(#\D)", run(R"((#\D))"));
      TEST_EQ(R"(#\E)", run(R"((#\E))"));
      TEST_EQ(R"(#\F)", run(R"((#\F))"));
      TEST_EQ(R"(#\G)", run(R"((#\G))"));
      TEST_EQ(R"(#\H)", run(R"((#\H))"));
      TEST_EQ(R"(#\I)", run(R"((#\I))"));
      TEST_EQ(R"(#\J)", run(R"((#\J))"));
      TEST_EQ(R"(#\K)", run(R"((#\K))"));
      TEST_EQ(R"(#\L)", run(R"((#\L))"));
      TEST_EQ(R"(#\M)", run(R"((#\M))"));
      TEST_EQ(R"(#\N)", run(R"((#\N))"));
      TEST_EQ(R"(#\O)", run(R"((#\O))"));
      TEST_EQ(R"(#\P)", run(R"((#\P))"));
      TEST_EQ(R"(#\Q)", run(R"((#\Q))"));
      TEST_EQ(R"(#\R)", run(R"((#\R))"));
      TEST_EQ(R"(#\S)", run(R"((#\S))"));
      TEST_EQ(R"(#\T)", run(R"((#\T))"));
      TEST_EQ(R"(#\U)", run(R"((#\U))"));
      TEST_EQ(R"(#\V)", run(R"((#\V))"));
      TEST_EQ(R"(#\W)", run(R"((#\W))"));
      TEST_EQ(R"(#\X)", run(R"((#\X))"));
      TEST_EQ(R"(#\Y)", run(R"((#\Y))"));
      TEST_EQ(R"(#\Z)", run(R"((#\Z))"));

      TEST_EQ(R"(#\000)", run(R"((#\000))"));
      TEST_EQ(R"(#\001)", run(R"((#\001))"));
      TEST_EQ(R"(#\002)", run(R"((#\002))"));
      TEST_EQ(R"(#\003)", run(R"((#\003))"));
      TEST_EQ(R"(#\004)", run(R"((#\004))"));
      TEST_EQ(R"(#\005)", run(R"((#\005))"));
      TEST_EQ(R"(#\006)", run(R"((#\006))"));
      TEST_EQ(R"(#\007)", run(R"((#\007))"));
      TEST_EQ(R"(#\008)", run(R"((#\008))"));
      TEST_EQ(R"(#\009)", run(R"((#\009))"));
      TEST_EQ(R"(#\010)", run(R"((#\010))"));
      TEST_EQ(R"(#\011)", run(R"((#\011))"));
      TEST_EQ(R"(#\012)", run(R"((#\012))"));
      TEST_EQ(R"(#\013)", run(R"((#\013))"));
      TEST_EQ(R"(#\014)", run(R"((#\014))"));
      TEST_EQ(R"(#\015)", run(R"((#\015))"));
      TEST_EQ(R"(#\016)", run(R"((#\016))"));
      TEST_EQ(R"(#\017)", run(R"((#\017))"));
      TEST_EQ(R"(#\018)", run(R"((#\018))"));
      TEST_EQ(R"(#\019)", run(R"((#\019))"));

      TEST_EQ(R"(#\000)", run(R"(#\nul)"));
      TEST_EQ(R"(#\000)", run(R"(#\null)"));
      TEST_EQ(R"(#\008)", run(R"(#\backspace)"));
      TEST_EQ(R"(#\009)", run(R"(#\tab)"));
      TEST_EQ(R"(#\010)", run(R"(#\newline)"));
      TEST_EQ(R"(#\010)", run(R"(#\linefeed)"));
      TEST_EQ(R"(#\011)", run(R"(#\vtab)"));
      TEST_EQ(R"(#\012)", run(R"(#\page)"));
      TEST_EQ(R"(#\013)", run(R"(#\return)"));
      TEST_EQ(R"(#\ )", run(R"(#\space)"));
      TEST_EQ(R"(#\127)", run(R"(#\rubout)"));
      }
    };

  struct doubles : public compile_fixture {
    void test()
      {
      TEST_EQ("3.14", run("3.14"));
      TEST_EQ("3.14", run("(3.14)"));
      TEST_EQ("3.14", run("((3.14))"));
      TEST_EQ("3.14159", run("3.14159265359"));
      TEST_EQ("3.1415926535900001", run("(3.14159265359)", true, 17));
      }
    };
    
  struct add1 : public compile_fixture {
    void test()
      {
      TEST_EQ("0", run("(add1)"));
      TEST_EQ("1", run("(add1 0)"));
      TEST_EQ("0", run("(add1 -1)"));
      TEST_EQ("6", run("(add1 5)"));
      TEST_EQ("-999", run("(add1 -1000)"));

      TEST_EQ("536870911", run("(add1 536870910)"));
      TEST_EQ("-536870911", run("(add1 -536870912)"));
      TEST_EQ("2", run("(add1 (add1 0))"));
      TEST_EQ("18", run("(add1 (add1 (add1 (add1 (add1 (add1 12))))))"));
      TEST_EQ("53687091001", run("(add1 53687091000)"));
      TEST_EQ("-53687091000", run("(add1 -53687091001)"));

      TEST_EQ("1.5", run("(add1 0.5)"));
      TEST_EQ("0.4", run("(add1 -0.6)"));
      }
    };

  struct sub1 : public compile_fixture {
    void test()
      {
      TEST_EQ("0", run("(sub1 1)"));
      TEST_EQ("-1", run("(sub1 0)"));
      TEST_EQ("-2", run("(sub1 -1)"));
      TEST_EQ("4", run("(sub1 5)"));
      TEST_EQ("-1001", run(R"((sub1 -1000))"));

      TEST_EQ("0.5", run("(sub1 1.5)"));
      TEST_EQ("-0.5", run("(sub1 0.5)"));
      TEST_EQ("-1.6", run("(sub1 -0.6)"));
      }
    };

  struct add_fixnums : public compile_fixture {
    void test()
      {
      TEST_EQ("3", run("(+ 1 2)"));
      TEST_EQ("6", run("(+ 1 2 3)"));
      TEST_EQ("10", run("(+ 1 2 3 4)"));
      TEST_EQ("15", run("(+ 1 2 3 4 5)"));
      TEST_EQ("21", run("(+ 1 2 3 4 5 6)"));
      TEST_EQ("28", run("(+ 1 2 3 4 5 6 7)"));
      TEST_EQ("36", run("(+ 1 2 3 4 5 6 7 8)"));
      TEST_EQ("45", run("(+ 1 2 3 4 5 6 7 8 9)"));
      TEST_EQ("55", run("(+ 1 2 3 4 5 6 7 8 9 10)"));
      TEST_EQ("66", run("(+ 1 2 3 4 5 6 7 8 9 10 11)"));
      }
    };

  struct add_flonums : public compile_fixture {
    void test()
      {
      TEST_EQ("3", run("(+ 1.0 2.0)"));
      TEST_EQ("6", run("(+ 1.0 2.0 3.0)"));
      TEST_EQ("10", run("(+ 1.0 2.0 3.0 4.0)"));
      TEST_EQ("15", run("(+ 1.0 2.0 3.0 4.0 5.0)"));
      TEST_EQ("21", run("(+ 1.0 2.0 3.0 4.0 5.0 6.0)"));
      TEST_EQ("28", run("(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0)"));
      TEST_EQ("36", run("(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0)"));
      TEST_EQ("45", run("(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0)"));
      TEST_EQ("55", run("(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0)"));
      TEST_EQ("66", run("(+ 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0 11.0)"));
      }
    };

  struct add_flonums_and_fixnums : public compile_fixture {
    void test()
      {
      TEST_EQ("3", run("(+ 1 2.0)"));
      TEST_EQ("3", run("(+ 1.0 2)"));
      }
    };

  struct sub : public compile_fixture {
    void test()
      {
      TEST_EQ("-1", run("(- 1 2)"));
      TEST_EQ("-4", run("(- 1 2 3)"));
      TEST_EQ("-3", run("(- 1 2 3 -1)"));

      TEST_EQ("0.1", run("(- 0.5 0.4)"));
      TEST_EQ("-2.77556e-17", run("(- 0.5 0.4 0.1)"));

      TEST_EQ("5.8", run("(- 7 0.5 0.4 0.1 0.2)"));
      }
    };

  struct mul : public compile_fixture {
    void test()
      {
      TEST_EQ("2", run("(* 1 2)"));
      TEST_EQ("6", run("(* 1 2 3)"));
      TEST_EQ("-6", run("(* 1 2 3 -1)"));

      TEST_EQ("1.25", run("(* 0.5 2.5)"));
      TEST_EQ("0.125", run("(* 0.5 2.5 0.1)"));
      }
    };

  struct divtest : public compile_fixture {
    void test()
      {
      TEST_EQ("2", run("(/ 4 2)"));
      TEST_EQ("2", run("(/ 8 2 2)"));
      TEST_EQ("-1", run("(/ 16 4 -4)"));
      TEST_EQ("-1", run("(/ 16 -4 4)"));

      TEST_EQ("1.25", run("(/ 2.5 2)"));
      TEST_EQ("-12.5", run("(/ 2.5 2 -0.1)"));
      }
    };

  struct add_incorrect_argument : public compile_fixture {
    void test()
      {
      if (!ops.safe_primitives)
        return;
      TEST_EQ("runtime error: add1: contract violation", run("(add1 #t)"));
      TEST_EQ("runtime error: sub1: contract violation", run("(sub1 ())"));

      TEST_EQ("runtime error: +: contract violation", run("(+ #t #t)"));
      TEST_EQ("runtime error: +: contract violation", run("(+ 3 #t)"));
      TEST_EQ("runtime error: +: contract violation", run("(+ #t 5)"));
      TEST_EQ("runtime error: +: contract violation", run("(+ 3.1 #t)"));
      TEST_EQ("runtime error: +: contract violation", run("(+ #t 5.1)"));
      TEST_EQ("runtime error: +: contract violation", run("(+ 1 2 3 4 5 6 7 8 3.1 #t)"));
      TEST_EQ("runtime error: +: contract violation", run("(+ 1 2 3 4 5 6 7 8 #t 5.1)"));

      TEST_EQ("runtime error: -: contract violation", run("(- #t #t)"));
      TEST_EQ("runtime error: -: contract violation", run("(- 3 #t)"));
      TEST_EQ("runtime error: -: contract violation", run("(- #t 5)"));
      TEST_EQ("runtime error: -: contract violation", run("(- 3.1 #t)"));
      TEST_EQ("runtime error: -: contract violation", run("(- #t 5.1)"));
      TEST_EQ("runtime error: -: contract violation", run("(- 1 2 3 4 5 6 7 8 3.1 #t)"));
      TEST_EQ("runtime error: -: contract violation", run("(- 1 2 3 4 5 6 7 8 #t 5.1)"));

      TEST_EQ("runtime error: *: contract violation", run("(* #t #t)"));
      TEST_EQ("runtime error: *: contract violation", run("(* 3 #t)"));
      TEST_EQ("runtime error: *: contract violation", run("(* #t 5)"));
      TEST_EQ("runtime error: *: contract violation", run("(* 3.1 #t)"));
      TEST_EQ("runtime error: *: contract violation", run("(* #t 5.1)"));
      TEST_EQ("runtime error: *: contract violation", run("(* 1 2 3 4 5 6 7 8 3.1 #t)"));
      TEST_EQ("runtime error: *: contract violation", run("(* 1 2 3 4 5 6 7 8 #t 5.1)"));

      TEST_EQ("runtime error: /: contract violation", run("(/ #t #t)"));
      TEST_EQ("runtime error: /: contract violation", run("(/ 3 #t)"));
      TEST_EQ("runtime error: /: contract violation", run("(/ #t 5)"));
      TEST_EQ("runtime error: /: contract violation", run("(/ 3.1 #t)"));
      TEST_EQ("runtime error: /: contract violation", run("(/ #t 5.1)"));
      TEST_EQ("runtime error: /: contract violation", run("(/ 1 2 3 4 5 6 7 8 3.1 #t)"));
      TEST_EQ("runtime error: /: contract violation", run("(/ 1 2 3 4 5 6 7 8 #t 5.1)"));
      }
    };

  struct combination_of_math_ops : public compile_fixture {
    void test()
      {
      TEST_EQ("27", run("(/ (* 3 (- (+ 23 9) 20.0) 1.5) 2)")); // 3*12*1.5 / 2 = 3*6*1.5 = 3*9 = 27
      }
    };

  struct equal : public compile_fixture {
    void test()
      {
      TEST_EQ("#f", run("(= 12 13)"));
      TEST_EQ("#t", run("(= 12 12)"));
      TEST_EQ("#f", run("(= 12.1 13.1)"));
      TEST_EQ("#t", run("(= 12.1 12.1)"));
      TEST_EQ("#f", run("(= 12 13.1)"));
      TEST_EQ("#t", run("(= 12 12.0)"));
      TEST_EQ("#f", run("(= 12.0 13)"));
      TEST_EQ("#t", run("(= 12.0 12)"));

      TEST_EQ("#t", run("(= 12 12)"));
      TEST_EQ("#f", run("(= 13 12)"));
      TEST_EQ("#f", run("(= 16 (+ 13 1)) "));
      TEST_EQ("#t", run("(= 16 (+ 13 3))"));
      TEST_EQ("#f", run("(= 16 (+ 13 13))"));
      TEST_EQ("#f", run("(= (+ 13 1) 16) "));
      TEST_EQ("#t", run("(= (+ 13 3) 16) "));
      TEST_EQ("#f", run("(= (+ 13 13) 16)"));

      TEST_EQ("#f", run("(= 12.0 13)"));
      TEST_EQ("#t", run("(= 12.0 12)"));
      TEST_EQ("#f", run("(= 13.0 12)"));
      TEST_EQ("#f", run("(= 16.0 (+ 13 1)) "));
      TEST_EQ("#t", run("(= 16.0 (+ 13 3))"));
      TEST_EQ("#f", run("(= 16.0 (+ 13 13))"));
      TEST_EQ("#f", run("(= (+ 13.0 1) 16) "));
      TEST_EQ("#t", run("(= (+ 13.0 3) 16.0) "));
      TEST_EQ("#f", run("(= (+ 13.0 13.0) 16.0)"));

      TEST_EQ("#t", run("(= 12 12 12)"));
      TEST_EQ("#t", run("(= 12 12 12 12)"));
      TEST_EQ("#t", run("(= 12 12 12 12 12)"));
      TEST_EQ("#t", run("(= 12 12 12 12 12 12)"));
      TEST_EQ("#t", run("(= 12 12 12 12 12 12 12)"));
      TEST_EQ("#t", run("(= 12 12 12 12 12 12 12 12)"));
      TEST_EQ("#t", run("(= 12 12 12 12 12 12 12 12 12)"));
      TEST_EQ("#t", run("(= 12 12 12 12 12 12 12 12 12 12)"));

      TEST_EQ("#f", run("(= 13 12 12)"));
      TEST_EQ("#f", run("(= 13 12 12 12)"));
      TEST_EQ("#f", run("(= 13 12 12 12 12)"));
      TEST_EQ("#f", run("(= 13 12 12 12 12 12)"));
      TEST_EQ("#f", run("(= 13 12 12 12 12 12 12)"));
      TEST_EQ("#f", run("(= 13 12 12 12 12 12 12 12)"));
      TEST_EQ("#f", run("(= 13 12 12 12 12 12 12 12 12)"));
      TEST_EQ("#f", run("(= 13 12 12 12 12 12 12 12 12 12)"));

      TEST_EQ("#f", run("(= 12 12 13)"));
      TEST_EQ("#f", run("(= 12 12 12 13)"));
      TEST_EQ("#f", run("(= 12 12 12 12 13)"));
      TEST_EQ("#f", run("(= 12 12 12 12 12 13)"));
      TEST_EQ("#f", run("(= 12 12 12 12 12 12 13)"));
      TEST_EQ("#f", run("(= 12 12 12 12 12 12 12 13)"));
      TEST_EQ("#f", run("(= 12 12 12 12 12 12 12 12 13)"));
      TEST_EQ("#f", run("(= 12 12 12 12 12 12 12 12 12 13)"));
      }
    };

  struct not_equal : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run("(!= 12 13)"));
      TEST_EQ("#f", run("(!= 12 12)"));
      TEST_EQ("#t", run("(!= 12.1 13.1)"));
      TEST_EQ("#f", run("(!= 12.1 12.1)"));
      TEST_EQ("#t", run("(!= 12 13.1)"));
      TEST_EQ("#f", run("(!= 12 12.0)"));
      TEST_EQ("#t", run("(!= 12.0 13)"));
      TEST_EQ("#f", run("(!= 12.0 12)"));

      TEST_EQ("#f", run("(!= 12 12)"));
      TEST_EQ("#t", run("(!= 13 12)"));
      TEST_EQ("#t", run("(!= 16 (+ 13 1)) "));
      TEST_EQ("#f", run("(!= 16 (+ 13 3))"));
      TEST_EQ("#t", run("(!= 16 (+ 13 13))"));
      TEST_EQ("#t", run("(!= (+ 13 1) 16) "));
      TEST_EQ("#f", run("(!= (+ 13 3) 16) "));
      TEST_EQ("#t", run("(!= (+ 13 13) 16)"));

      TEST_EQ("#t", run("(!= 12.0 13)"));
      TEST_EQ("#f", run("(!= 12.0 12)"));
      TEST_EQ("#t", run("(!= 13.0 12)"));
      TEST_EQ("#t", run("(!= 16.0 (+ 13 1)) "));
      TEST_EQ("#f", run("(!= 16.0 (+ 13 3))"));
      TEST_EQ("#t", run("(!= 16.0 (+ 13 13))"));
      TEST_EQ("#t", run("(!= (+ 13.0 1) 16) "));
      TEST_EQ("#f", run("(!= (+ 13.0 3) 16.0) "));
      TEST_EQ("#t", run("(!= (+ 13.0 13.0) 16.0)"));

      TEST_EQ("#t", run("(!= 12 13 14 15 16 17 18 19 20 21 22 23)"));
      TEST_EQ("#t", run("(!= 12 13 14 15 16 17 18 19 20 21 22 12)"));
      }
    };

  struct less : public compile_fixture {
    void test()
      {
      TEST_EQ("#f", run("(< 4 2)"));
      TEST_EQ("#t", run("(< 2 4)"));
      TEST_EQ("#f", run("(< 4 2 3)"));
      TEST_EQ("#t", run("(< 2 4 5)"));
      TEST_EQ("#f", run("(< 2 4 3)"));

      TEST_EQ("#f", run("(< 4.1 2)"));
      TEST_EQ("#t", run("(< 2.1 4)"));
      TEST_EQ("#f", run("(< 4.1 2 3)"));
      TEST_EQ("#t", run("(< 2.1 4 5)"));
      TEST_EQ("#f", run("(< 2.1 4 3)"));

      TEST_EQ("#t", run("(< 12 13)"));
      TEST_EQ("#f", run("(< 12 12)"));
      TEST_EQ("#f", run("(< 13 12)"));
      TEST_EQ("#f", run("(< 16 (+ 13 1)) "));
      TEST_EQ("#f", run("(< 16 (+ 13 3))"));
      TEST_EQ("#t", run("(< 16 (+ 13 13))"));
      TEST_EQ("#t", run("(< (+ 13 1) 16) "));
      TEST_EQ("#f", run("(< (+ 13 3) 16) "));
      TEST_EQ("#f", run("(< (+ 13 13) 16)"));

      TEST_EQ("#t", run("(< 12.0 13)"));
      TEST_EQ("#f", run("(< 12.0 12)"));
      TEST_EQ("#f", run("(< 13.0 12)"));
      TEST_EQ("#f", run("(< 16.0 (+ 13 1.0)) "));
      TEST_EQ("#f", run("(< 16.0 (+ 13.0 3.0))"));
      TEST_EQ("#t", run("(< 16.0 (+ 13.0 13.0))"));
      TEST_EQ("#t", run("(< (+ 13.0 1) 16.0) "));
      TEST_EQ("#f", run("(< (+ 13.0 3.000000001) 16.0)"));
      TEST_EQ("#f", run("(< (+ 13 13.0) 16)"));
      }
    };


  struct leq : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run("(<= 12 13)"));
      TEST_EQ("#t", run("(<= 12 12)"));
      TEST_EQ("#f", run("(<= 13 12)"));
      TEST_EQ("#f", run("(<= 16 (+ 13 1)) "));
      TEST_EQ("#t", run("(<= 16 (+ 13 3))"));
      TEST_EQ("#t", run("(<= 16 (+ 13 13))"));
      TEST_EQ("#t", run("(<= (+ 13 1) 16) "));
      TEST_EQ("#t", run("(<= (+ 13 3) 16) "));
      TEST_EQ("#f", run("(<= (+ 13 13) 16)"));

      TEST_EQ("#t", run("(<= 12.0 13.0)"));
      TEST_EQ("#t", run("(<= 12.0 12.0)"));
      TEST_EQ("#f", run("(<= 13.0 12)"));
      TEST_EQ("#f", run("(<= 16 (+ 13.0 1)) "));
      TEST_EQ("#t", run("(<= 16 (+ 13 3.0))"));
      TEST_EQ("#t", run("(<= 16.0 (+ 13.0 13.0))"));
      TEST_EQ("#t", run("(<= (+ 13.0 1) 16) "));
      TEST_EQ("#t", run("(<= (+ 13 3.0) 16.0) "));
      TEST_EQ("#f", run("(<= (+ 13.0 13) 16.0)"));
      }
    };

  struct greater : public compile_fixture {
    void test()
      {
      TEST_EQ("#f", run("(> 12 13)"));
      TEST_EQ("#f", run("(> 12 12)"));
      TEST_EQ("#t", run("(> 13 12)"));
      TEST_EQ("#t", run("(> 16 (+ 13 1)) "));
      TEST_EQ("#f", run("(> 16 (+ 13 3))"));
      TEST_EQ("#f", run("(> 16 (+ 13 13))"));
      TEST_EQ("#f", run("(> (+ 13 1) 16) "));
      TEST_EQ("#f", run("(> (+ 13 3) 16) "));
      TEST_EQ("#t", run("(> (+ 13 13) 16)"));

      TEST_EQ("#f", run("(> 12.0 13)"));
      TEST_EQ("#f", run("(> 12.0 12)"));
      TEST_EQ("#t", run("(> 13.0 12)"));
      TEST_EQ("#t", run("(> 16.0 (+ 13 1)) "));
      TEST_EQ("#f", run("(> 16.0 (+ 13 3))"));
      TEST_EQ("#f", run("(> 16.0 (+ 13 13))"));
      TEST_EQ("#f", run("(> (+ 13.0 1) 16) "));
      TEST_EQ("#f", run("(> (+ 13.0 3) 16) "));
      TEST_EQ("#t", run("(> (+ 13.0 13) 16)"));
      }
    };

  struct geq : public compile_fixture {
    void test()
      {
      TEST_EQ("#f", run("(>= 12 13)"));
      TEST_EQ("#t", run("(>= 12 12)"));
      TEST_EQ("#t", run("(>= 13 12)"));
      TEST_EQ("#t", run("(>= 16 (+ 13 1)) "));
      TEST_EQ("#t", run("(>= 16 (+ 13 3))"));
      TEST_EQ("#f", run("(>= 16 (+ 13 13))"));
      TEST_EQ("#f", run("(>= (+ 13 1) 16) "));
      TEST_EQ("#t", run("(>= (+ 13 3) 16) "));
      TEST_EQ("#t", run("(>= (+ 13 13) 16)"));

      TEST_EQ("#f", run("(>= 12.0 13)"));
      TEST_EQ("#t", run("(>= 12.0 12)"));
      TEST_EQ("#t", run("(>= 13.0 12)"));
      TEST_EQ("#t", run("(>= 16.0 (+ 13 1)) "));
      TEST_EQ("#t", run("(>= 16.0 (+ 13 3))"));
      TEST_EQ("#f", run("(>= 16.0 (+ 13 13))"));
      TEST_EQ("#f", run("(>= (+ 13.0 1) 16) "));
      TEST_EQ("#t", run("(>= (+ 13.0 3) 16) "));
      TEST_EQ("#t", run("(>= (+ 13.0 13) 16)"));
      }
    };

  struct compare_incorrect_argument : public compile_fixture {
    void test()
      {
      if (!ops.safe_primitives)
        return;
      TEST_EQ("runtime error: =: contract violation", run("(= 3 #t)"));
      TEST_EQ("runtime error: !=: contract violation", run("(!= 3 ())"));
      TEST_EQ("runtime error: <: contract violation", run("(< 3 #t)"));
      TEST_EQ("runtime error: <=: contract violation", run("(<= 3 ())"));
      TEST_EQ("runtime error: >: contract violation", run("(> 3 #t)"));
      TEST_EQ("runtime error: >=: contract violation", run("(>= 3 ())"));
      }
    };

  struct iftest : public compile_fixture {
    void test()
      {
      TEST_EQ("2", run("(if (< 2 3) (2) (3))"));
      TEST_EQ("3", run("(if (< 3 2) (2) (3))"));
      TEST_EQ("12", run("(if #t 12 13)"));
      TEST_EQ("13", run("(if #f 12 13)"));
      TEST_EQ("12", run("(if 0 12 13)"));
      TEST_EQ("43", run("(if () 43 ())"));
      TEST_EQ("13", run("(if #t (if 12 13 4) 17)"));
      TEST_EQ("4", run("(if #f 12 (if #f 13 4))"));
      TEST_EQ("2", run(R"((if #\X (if 1 2 3) (if 4 5 6)))"));
      TEST_EQ("#t", run("(if (not (boolean? #t)) 15 (boolean? #f))"));
      TEST_EQ("-23", run(R"((if (if (char? #\a) (boolean? #\b) (fixnum? #\c)) 119 -23))"));
      TEST_EQ("6", run(R"((if (if (if (not 1) (not 2) (not 3)) 4 5) 6 7))"));
      TEST_EQ("7", run(R"((if (not (if (if (not 1) (not 2) (not 3)) 4 5)) 6 7))"));
      TEST_EQ("#f", run(R"((not (if (not (if (if (not 1) (not 2) (not 3)) 4 5)) 6 7)) )"));
      TEST_EQ("14", run(R"((if (char? 12) 13 14) )"));
      TEST_EQ("13", run(R"((if (char? #\a) 13 14) )"));
      TEST_EQ("13", run(R"((add1 (if (sub1 1) (sub1 13) 14)))"));

      TEST_EQ("13", run("(if (= 12 13) 12 13) "));
      TEST_EQ("13", run("(if (= 12 12) 13 14) "));
      TEST_EQ("12", run("(if (< 12 13) 12 13) "));
      TEST_EQ("14", run("(if (< 12 12) 13 14) "));
      TEST_EQ("14", run("(if (< 13 12) 13 14) "));
      TEST_EQ("12", run("(if (<= 12 13) 12 13) "));
      TEST_EQ("12", run("(if (<= 12 12) 12 13) "));
      TEST_EQ("14", run("(if (<= 13 12) 13 14) "));
      TEST_EQ("13", run("(if (> 12 13) 12 13) "));
      TEST_EQ("13", run("(if (> 12 12) 12 13) "));
      TEST_EQ("13", run("(if (> 13 12) 13 14) "));
      TEST_EQ("13", run("(if (>= 12 13) 12 13) "));
      TEST_EQ("12", run("(if (>= 12 12) 12 13) "));
      TEST_EQ("13", run("(if (>= 13 12) 13 14) "));
      }
    };
    
  struct andtest : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run("(and #t)"));
      TEST_EQ("#f", run("(and #f)"));
      TEST_EQ("#t", run("(and #t #t)"));
      TEST_EQ("#f", run("(and #f #f)"));
      TEST_EQ("#f", run("(and #f #t)"));
      TEST_EQ("#f", run("(and #t #f)"));
      TEST_EQ("#t", run("(and #t #t #t)"));
      TEST_EQ("#f", run("(and #f #t #t)"));
      TEST_EQ("#f", run("(and #t #t #f)"));
      TEST_EQ("#f", run("(and #t #f #t)"));
      TEST_EQ("#f", run("(and #f #f #f)"));
      }
    };

  struct ortest : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run("(or #t)"));
      TEST_EQ("#f", run("(or #f)"));
      TEST_EQ("#t", run("(or #t #t)"));
      TEST_EQ("#f", run("(or #f #f)"));
      TEST_EQ("#t", run("(or #f #t)"));
      TEST_EQ("#t", run("(or #t #f)"));
      TEST_EQ("#t", run("(or #t #t #t)"));
      TEST_EQ("#t", run("(or #f #t #t)"));
      TEST_EQ("#t", run("(or #t #t #f)"));
      TEST_EQ("#t", run("(or #t #f #t)"));
      TEST_EQ("#f", run("(or #f #f #f)"));
      }
    };

  struct let : public compile_fixture {
    void test()
      {
      TEST_EQ("5", run("(let ([x 5]) x)"));
      TEST_EQ("5", run("(let ([x 5][y 6]) x)"));
      TEST_EQ("6", run("(let ([x 5][y 6]) y)"));

      TEST_EQ("3", run("(let ([x (+ 1 2)]) x)"));
      TEST_EQ("10", run("(let ([x (+ 1 2)]) (let([y(+ 3 4)])(+ x y))) "));
      TEST_EQ("4", run("(let ([x (+ 1 2)])  (let([y(+ 3 4)])(- y x)))"));
      TEST_EQ("4", run("(let ([x (+ 1 2)] [y(+ 3 4)])  (- y x))"));
      TEST_EQ("18", run("(let ([x (let ([y (+ 1 2)]) (* y y))]) (+ x x))"));
      TEST_EQ("7", run("(let ([x (+ 1 2)]) (let([x(+ 3 4)]) x))"));
      TEST_EQ("7", run("(let ([x (+ 1 2)]) (let([x(+ x 4)]) x)) "));
      TEST_EQ("3", run("(let ([t (let ([t (let ([t (let ([t (+ 1 2)]) t)]) t)]) t)]) t)"));
      TEST_EQ("192", run("(let ([x 12])  (let([x(+ x x)]) (let([x(+ x x)]) (let([x(+ x x)]) (+ x x)))))"));

      TEST_EQ("45", run("(let ([a 0] [b 1] [c 2] [d 3] [e 4] [f 5] [g 6] [h 7] [i 8] [j 9]) (+ a b c d e f g h i j) )"));
      if (ops.safe_primitives)
        TEST_EQ("runtime error: closure expected", run("(let ([x 5]) (x))"));
      }
    };


  struct let_star : public compile_fixture {
    void test()
      {
      TEST_EQ("5", run("(let* ([x 5]) x)"));
      TEST_EQ("3", run("(let* ([x (+ 1 2)]) x)"));
      TEST_EQ("10", run("(let* ([x (+ 1 2)] [y(+ 3 4)])(+ x y))"));
      TEST_EQ("4", run("(let* ([x (+ 1 2)] [y(+ 3 4)]) (- y x))"));
      TEST_EQ("18", run("(let* ([x (let* ([y (+ 1 2)]) (* y y))])(+ x x))"));
      TEST_EQ("7", run("(let* ([x (+ 1 2)] [x(+ 3 4)]) x)"));
      TEST_EQ("7", run("(let* ([x (+ 1 2)] [x(+ x 4)]) x)"));
      TEST_EQ("3", run("(let* ([t (let* ([t (let* ([t (let* ([t (+ 1 2)]) t)]) t)]) t)]) t)"));
      TEST_EQ("192", run("(let* ([x 12] [x(+ x x)] [x(+ x x)] [x(+ x x)])  (+ x x))"));
      }
    };
    
  struct arithmetic : public compile_fixture {
    void test()
      {
      TEST_EQ("45", run("(+ (+ (+ (+ (+ (+ (+ (+ 1 2) 3) 4) 5) 6) 7) 8) 9)"));
      TEST_EQ("45", run("(+ 1 (+ 2 (+ 3 (+ 4 (+ 5 (+ 6 (+ 7 (+ 8 9))))))))"));

      TEST_EQ("-43", run("(- (- (- (- (- (- (- (- 1 2) 3) 4) 5) 6) 7) 8) 9)"));
      TEST_EQ("5", run("(- 1 (- 2 (- 3 (- 4 (- 5 (- 6 (- 7 (- 8 9))))))))"));

      TEST_EQ("5040", run("(* (* (* (* (* 2 3) 4) 5) 6) 7)"));
      TEST_EQ("5040", run("(* 2 (* 3 (* 4 (* 5 (* 6 7)))))"));
      }
    };

  struct globals : public compile_fixture {
    void test()
      {
      TEST_EQ("3.14", run("(define x 3.14) 50 x"));
      TEST_EQ("3.14", run("x"));
      TEST_EQ("7", run("(define x 7) 51 x"));
      TEST_EQ("7", run("x"));
      }
    };
    

  struct vector : public compile_fixture {
    void test()
      {
      TEST_EQ("#(1 2)", run("(vector 1 2)"));
      TEST_EQ("#(1 2 3 4 5 6 7 8)", run("(vector 1 2 3 4 5 6 7 8)"));
      TEST_EQ("#(1 2 3 4 5 6 7 8 9)", run("(vector 1 2 3 4 5 6 7 8 9)"));
      TEST_EQ("#(1 2 3 4 5 6 7 8 9 10)", run("(vector 1 2 3 4 5 6 7 8 9 10)"));
      TEST_EQ("#(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)", run("(vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)"));
      TEST_EQ("2", run("(let ([x (vector 1 2)]) (vector-ref x 1))"));
      TEST_EQ("2", run("(let ([x (vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)]) (vector-ref x 1))"));
      TEST_EQ("10", run("(let ([x (vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)]) (vector-ref x 9))"));
      TEST_EQ("1", run("(let ([x (vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)]) (vector-ref x 0))"));
      TEST_EQ("15", run("(let ([x (vector 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15)]) (vector-ref x 14))"));

      if (ops.safe_primitives)
        {
        TEST_EQ("runtime error: vector-ref: contract violation", run("(let ([x (vector 1 2)] [y 3]) (vector-ref y 1))"));
        TEST_EQ("runtime error: vector-ref: contract violation", run("(let ([x (vector 1 2)] [y 3]) (vector-ref x #t))"));
        TEST_EQ("runtime error: vector-ref: out of bounds", run("(let ([x (vector 1 2)] [y 3]) (vector-ref x 2))"));
        TEST_EQ("runtime error: vector-ref: out of bounds", run("(let ([x (vector 1 2)] [y 3]) (vector-ref x -1))"));
        }
      TEST_EQ("100", run("(let ([x (vector 1 2)]) (begin (vector-set! x 1 100) (vector-ref x 1)))"));
      TEST_EQ("#(1 100)", run("(let ([x (vector 1 2)]) (begin (vector-set! x 1 100) x))"));
      TEST_EQ("#(3.14 2)", run("(let ([x (vector 1 2)]) (begin (vector-set! x 0 3.14) x))"));

      if (ops.safe_primitives)
        {
        TEST_EQ("runtime error: vector-set!: contract violation", run("(let ([x (vector 1 2)]) (begin (vector-set! x 0.0 3.14) x))"));
        TEST_EQ("runtime error: vector-set!: contract violation", run("(let ([x (vector 1 2)] [y 3]) (begin (vector-set! y 0.0 3.14) x))"));
        TEST_EQ("runtime error: vector-set!: out of bounds", run("(let ([x (vector 1 2)]) (begin (vector-set! x -1 3.14) x))"));
        TEST_EQ("runtime error: vector-set!: out of bounds", run("(let ([x (vector 1 2)]) (begin (vector-set! x 100 3.14) x))"));
        }

      TEST_EQ("#t", run("(vector? (vector 0)) "));
      TEST_EQ("#f", run("(vector? 1287) "));
      TEST_EQ("#f", run("(vector? ()) "));
      TEST_EQ("#f", run("(vector? #t) "));
      TEST_EQ("#f", run("(vector? #f) "));
      }
    };

  struct letrec : public compile_fixture {
    void test()
      {
      TEST_EQ("12", run("(letrec () 12)"));
      TEST_EQ("10", run("(letrec () (let ([x 5]) (+ x x)))"));
      TEST_EQ("7", run("(letrec ([f (lambda () 5)]) 7)"));
      TEST_EQ("12", run("(letrec () 12)"));
      TEST_EQ("10", run("(letrec () (let ([x 5]) (+ x x)))"));
      TEST_EQ("7", run("(letrec ([f (lambda () 5)]) 7)"));
      TEST_EQ("12", run("(letrec ([f (lambda () 5)]) (let ([x 12]) x))"));
      TEST_EQ("5", run("(letrec ([f (lambda () 5)]) (f))"));
      TEST_EQ("5", run("(letrec ([f (lambda () 5)]) (let ([x (f)]) x))"));
      TEST_EQ("11", run("(letrec ([f (lambda () 5)]) (+ (f) 6))"));
      TEST_EQ("11", run("(letrec ([f (lambda () 5)]) (+ 6 (f)))"));
      TEST_EQ("15", run("(letrec ([f (lambda () 5)]) (- 20 (f)))"));
      TEST_EQ("10", run("(letrec ([f (lambda () 5)]) (+ (f) (f)))"));
      TEST_EQ("12", run("(letrec ([f (lambda () (+ 5 7))])(f))"));
      TEST_EQ("25", run("(letrec ([f (lambda (x) (+ x 12))]) (f 13))"));
      TEST_EQ("12", run("(letrec ([f (lambda (x) (+ x 12))]) (f 0))"));
      TEST_EQ("24", run("(letrec ([f (lambda (x) (+ x 12))]) (f (f 0)))"));
      TEST_EQ("36", run("(letrec ([f (lambda (x) (+ x 12))]) (f (f (f 0))))"));
      TEST_EQ("41", run("(letrec ([f (lambda (x y) (+ x y))] [g (lambda(x) (+ x 12))])(f 16 (f (g 0) (+ 1 (g 0)))))"));
      TEST_EQ("24", run("(letrec ([f (lambda (x) (g x x))][g(lambda(x y) (+ x y))])(f 12))"));
      TEST_EQ("34", run("(letrec ([f (lambda (x) (+ x 12))]) (f (f 10)))"));
      TEST_EQ("36", run("(letrec ([f (lambda (x) (+ x 12))]) (f (f (f 0))))"));
      TEST_EQ("25", run("(let ([f (lambda () 12)][g(lambda() 13)])(+ (f)(g)))"));
      TEST_EQ("120", run("(letrec ([f (lambda (x) (if (zero? x) 1 (* x(f(sub1 x)))))]) (f 5))"));
      TEST_EQ("120", run("(letrec ([f (lambda (x acc) (if (zero? x) acc (f(sub1 x) (* acc x))))]) (f 5 1))"));
      TEST_EQ("200", run("(letrec ([f (lambda (x) (if (zero? x) 0 (+ 1 (f(sub1 x)))))]) (f 200))"));
      TEST_EQ("500", run("(letrec ([f (lambda (x) (if (zero? x) 0 (+ 1 (f(sub1 x)))))]) (f 500))"));
      }
    };

  struct lambdas : public compile_fixture {
    void test()
      {
      TEST_EQ("<lambda>", run("(lambda (x) (+ x x))"));
      TEST_EQ("3", run("((lambda(x) x) 3)"));
      TEST_EQ("10", run("((lambda(x y) (+ x y)) 3 7)"));
      TEST_EQ("8", run("(let ([x 5]) ((lambda (y) (+ 3 y)) x ))"));
      TEST_EQ("5", run("( (lambda () (+ 3 2)) () )"));
      TEST_EQ("<lambda>", run("(let ([f (lambda () 5)]) f)"));
      TEST_EQ("8", run("(let ([f (lambda (n) (+ n 5))]) (f 3))"));
      TEST_EQ("5", run("(let ([f (lambda () 5)]) (f))"));

      TEST_EQ("8", run("(let ([x 5]) ((lambda (y) (+ x y)) 3) )"));
      }
    };

  struct is_fixnum : public compile_fixture {
    void test()
      {
      TEST_EQ("#f", run(R"((fixnum? #\019))"));
      TEST_EQ("#f", run(R"((fixnum? #\a))"));
      TEST_EQ("#f", run(R"((fixnum? #t))"));
      TEST_EQ("#f", run(R"((fixnum? #f))"));
      TEST_EQ("#f", run(R"((fixnum? 0.3))"));
      TEST_EQ("#f", run(R"((fixnum? 0.0))"));
      TEST_EQ("#f", run(R"((fixnum? ()))"));
      TEST_EQ("#t", run(R"((fixnum? 0))"));
      TEST_EQ("#t", run(R"((fixnum? -1))"));
      TEST_EQ("#t", run(R"((fixnum? 1))"));
      TEST_EQ("#t", run(R"((fixnum? -1000))"));
      TEST_EQ("#t", run(R"((fixnum? 1000))"));
      TEST_EQ("#t", run(R"((fixnum? ((1000))))"));
      }
    };

  struct nottest : public compile_fixture {
    void test()
      {
      TEST_EQ("#f", run("(not #t)"));
      TEST_EQ("#t", run("(not #f)"));
      TEST_EQ("#t", run("(not (not #t))"));
      TEST_EQ("#f", run(R"((not #\a))"));
      TEST_EQ("#f", run("(not 0)"));
      TEST_EQ("#f", run("(not 15)"));
      TEST_EQ("#f", run("(not ())"));
      TEST_EQ("#t", run("(not (not 15))"));
      TEST_EQ("#f", run("(not (fixnum? 15))"));
      TEST_EQ("#t", run("(not (fixnum? #f))"));
      }
    };

  struct is_null : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run("(null? ())"));
      TEST_EQ("#f", run("(null? #f)"));
      TEST_EQ("#f", run("(null? #t)"));
      TEST_EQ("#f", run("(null? (null? ()))"));
      TEST_EQ("#f", run(R"((null? #\a))"));
      TEST_EQ("#f", run("(null? 0)"));
      TEST_EQ("#f", run("(null? -10)"));
      TEST_EQ("#f", run("(null? 10)"));
      }
    };


  struct is_flonum : public compile_fixture {
    void test()
      {
      TEST_EQ("#f", run(R"((flonum? #\019))"));
      TEST_EQ("#f", run(R"((flonum? #\a))"));
      TEST_EQ("#f", run(R"((flonum? #t))"));
      TEST_EQ("#f", run(R"((flonum? #f))"));
      TEST_EQ("#t", run(R"((flonum? 0.3))"));
      TEST_EQ("#t", run(R"((flonum? 0.0))"));
      TEST_EQ("#f", run(R"((flonum? ()))"));
      TEST_EQ("#t", run(R"((flonum? 0.0))"));
      TEST_EQ("#t", run(R"((flonum? -1.50))"));
      TEST_EQ("#t", run(R"((flonum? 1.02))"));
      TEST_EQ("#f", run(R"((flonum? -1000))"));
      TEST_EQ("#f", run(R"((flonum? 1000))"));
      TEST_EQ("#t", run(R"((flonum? ((1000.0))))"));
      }
    };
  struct is_zero : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run("(zero? 0)"));
      TEST_EQ("#f", run("(zero? 1)"));
      TEST_EQ("#f", run("(zero? -1)"));
      TEST_EQ("#f", run("(zero? 64)"));
      TEST_EQ("#f", run("(zero? 960)"));
      TEST_EQ("#f", run("(zero? 53687091158)"));

      TEST_EQ("#t", run("(zero? 0.0)"));
      TEST_EQ("#f", run("(zero? 0.0001)"));
      TEST_EQ("#f", run("(zero? 1.1)"));
      TEST_EQ("#f", run("(zero? -1.1)"));
      TEST_EQ("#f", run("(zero? 64.1)"));
      TEST_EQ("#f", run("(zero? 960.1)"));
      TEST_EQ("#f", run("(zero? 53687091158.1)"));

      if (ops.safe_primitives)        
        TEST_EQ("runtime error: zero?: contract violation", run("(zero? #t)"));
      }
    };

  struct is_boolean : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run("(boolean? #t)"));
      TEST_EQ("#t", run("(boolean? #f)"));
      TEST_EQ("#f", run("(boolean? 0)"));
      TEST_EQ("#f", run("(boolean? 1)"));
      TEST_EQ("#f", run("(boolean? -1)"));
      TEST_EQ("#f", run("(boolean? ())"));
      TEST_EQ("#f", run(R"((boolean? #\a))"));
      TEST_EQ("#t", run("(boolean? (boolean? 0))"));
      TEST_EQ("#t", run("(boolean? (fixnum? (boolean? 0)))"));
      }
    };

  struct is_char : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run(R"((char? #\a))"));
      TEST_EQ("#t", run(R"((char? #\Z))"));
      TEST_EQ("#t", run(R"((char? #\000))"));
      TEST_EQ("#f", run(R"((char? #t))"));
      TEST_EQ("#f", run(R"((char? #f))"));
      TEST_EQ("#f", run(R"((char? (char? #t)))"));
      TEST_EQ("#f", run(R"((char? 0))"));
      TEST_EQ("#f", run(R"((char? 10))"));
      TEST_EQ("#f", run(R"((char? 0.3))"));
      }
    };
  struct cons : public compile_fixture {
    void test()
      {
      TEST_EQ("#t", run("(pair? (cons 1 2))"));
      TEST_EQ("#f", run("(pair? 12)"));
      TEST_EQ("#f", run("(pair? #t)"));
      TEST_EQ("#f", run("(pair? #f)"));
      TEST_EQ("#f", run("(pair? ())"));

      TEST_EQ("#f", run("(fixnum? (cons 12 43))"));
      TEST_EQ("#f", run("(boolean? (cons 12 43))"));
      TEST_EQ("#f", run("(null? (cons 12 43))"));
      TEST_EQ("#f", run("(not (cons 12 43))"));

      TEST_EQ("32", run("(if (cons 12 43) 32 43)"));
      TEST_EQ("1", run("(car(cons 1 23))"));
      TEST_EQ("123", run("(cdr(cons 43 123))"));
      TEST_EQ("#t", run("(let([x(cons 1 2)] [y(cons 3 4)]) (pair? x))"));
      TEST_EQ("#t", run("(pair? (cons(cons 12 3) #f))"));
      TEST_EQ("#t", run("(pair? (cons(cons 12 3) (cons #t #f)))"));
      TEST_EQ("12", run("(car(car(cons(cons 12 3) (cons #t #f))))"));
      TEST_EQ("3", run("(cdr(car(cons(cons 12 3) (cons #t #f))))"));
      TEST_EQ("#t", run("(car(cdr(cons(cons 12 3) (cons #t #f))))"));
      TEST_EQ("#f", run("(cdr(cdr(cons(cons 12 3) (cons #t #f))))"));
      TEST_EQ("#t", run("(pair? (cons(* 1 1) 1))"));

      TEST_EQ("((1 . 4) 3 . 2)", run("(let ([t0 (cons 1 2)] [t1 (cons 3 4)]) (let([a0(car t0)][a1(car t1)][d0(cdr t0)][d1(cdr t1)])(let([t0(cons a0 d1)][t1(cons a1 d0)]) (cons t0 t1))))"));
      TEST_EQ("(1 . 2)", run("(let ([t (cons 1 2)])(let([t t])(let([t t])(let([t t]) t))))"));
      TEST_EQ("(1 . 2)", run("(let ([t (let ([t (let ([t (let ([t (cons 1 2)]) t)]) t)]) t)]) t)"));
      TEST_EQ("((((()) ()) (()) ()) ((()) ()) (()) ())", run("(let ([x ()])(let([x(cons x x)])(let([x(cons x x)])(let([x(cons x x)])(cons x x)))))"));
      TEST_EQ("((#t #t . #t) ((#f . #f) . #f))", run("(cons (let ([x #t]) (let ([y (cons x x)]) (cons x y))) (cons(let([x #f]) (let([y(cons x x)]) (cons y x))) ())) "));

      TEST_EQ("(1 2 3 4 5 6 7 8 9 10)", run("(letrec ([f (lambda (i lst) (if (= i 0) lst (f (sub1 i) (cons i lst))))])(f 10 ())) "));
      if (ops.safe_primitives)
        {
        TEST_EQ("runtime error: cons: contract violation", run("(cons 2)"));
        TEST_EQ("runtime error: cons: contract violation", run("(cons 2 3 4)"));

        TEST_EQ("runtime error: car: contract violation", run("(car 2)"));
        TEST_EQ("runtime error: cdr: contract violation", run("(cdr 2 3 4)"));
        }
      }
    };
  struct tailcall : public compile_fixture {
    void test()
      {
      TEST_EQ("#f", run("(letrec ([e (lambda (x) (if (zero? x) #t (o (sub1 x))))][o(lambda(x) (if (zero? x) #f(e(sub1 x))))])(e 1))"));
      TEST_EQ("0", run("(letrec ([countdown (lambda (n) (if (zero? n) n (countdown(sub1 n))))])(countdown 5050))"));
      TEST_EQ("50005000", run("(letrec ([sum (lambda (n ac)(if (zero? n) ac (sum(sub1 n) (+ n ac))))]) (sum 10000 0))"));
      TEST_EQ("#t", run("(letrec ([e (lambda (x) (if (zero? x) #t (o (sub1 x))))][o(lambda(x) (if (zero? x) #f(e(sub1 x))))])(e 500))"));
      }
    };

  struct begin : public compile_fixture {
    void test()
      {
      TEST_EQ("12", run("(begin 12)"));
      TEST_EQ("122", run("(begin 13 122)"));
      TEST_EQ("#t", run("(begin 123 2343 #t)"));
      TEST_EQ("(1 . 2)", run("(let ([t (begin 12 (cons 1 2))]) (begin t t))"));
      TEST_EQ("(1 . 2)", run("(let ([t (begin 13 (cons 1 2))])(begin (cons 1 t)t ))"));
      TEST_EQ("(1 . 2)", run("(let ([t (cons 1 2)])(if (pair? t)(begin t) 12))"));
      }
    };

  struct implicit_begin : public compile_fixture {
    void test()
      {
      TEST_EQ("(1 . 2)", run("(let ([t (begin 13 (cons 1 2))])(cons 1 t)t)"));
      }
    };

  struct closures : public compile_fixture {
    void test()
      {
      TEST_EQ("12", run("(let ([n 12])(let([f(lambda() n)])(f)))"));
      TEST_EQ("112", run("(let ([n 12])(let([f(lambda(m) (+ n m))])(f 100)))"));
      TEST_EQ("120", run("(let ([f (lambda (f n m)(if (zero? n)  m (f(sub1 n) (* n m))))]) (let([g(lambda(g n m) (f(lambda(n m) (g g n m)) n m))])  (g g 5 1)))"));
      TEST_EQ("120", run("(let ([f (lambda (f n) (if (zero? n) 1 (* n(f(sub1 n)))))]) (let([g(lambda(g n) (f(lambda(n) (g g n)) n))]) (g g 5)))"));
      }
    };

  struct set : public compile_fixture {
    void test()
      {
      TEST_EQ("13", run("(let ([x 12])(set! x 13) x)"));
      TEST_EQ("13", run("(let([x 12]) (set! x(add1 x)) x) "));
      TEST_EQ("12", run("(let([x 12]) (let([x #f]) (set! x 14)) x) "));
      TEST_EQ("12", run("(let([x 12]) (let([y(let([x #f]) (set! x 14))])  x)) "));
      TEST_EQ("10", run("(let([f #f])(let([g(lambda() f)]) (set! f 10) (g))) "));
      TEST_EQ("13", run("(let([f(lambda(x) (set! x (add1 x)) x)]) (f 12)) "));
      TEST_EQ("(10 . 11)", run("(let([x 10]) (let([f(lambda(x) (set! x(add1 x)) x)]) (cons x(f x)))) "));
      TEST_EQ("17", run("(let([t #f]) (let([locative (cons (lambda() t) (lambda(n) (set! t n)))]) ((cdr locative) 17) ((car locative))))"));
      TEST_EQ("17", run("(let([locative (let([t #f]) (cons (lambda() t)  (lambda(n) (set! t n))))]) ((cdr locative) 17) ((car locative))) "));
      TEST_EQ("(1 . 0)", run("(let([make-counter (lambda() (let([counter -1]) (lambda()  (set! counter(add1 counter)) counter)))]) (let([c0(make-counter)] [c1(make-counter)])(c0) (cons(c0) (c1)))) "));
      TEST_EQ("120", run("(let([fact #f]) (set! fact(lambda(n) (if (zero? n) 1 (* n(fact(sub1 n)))))) (fact 5)) "));
      TEST_EQ("120", run("(let([fact #f]) ((begin (set! fact(lambda(n) (if (zero? n)  1 (* n(fact(sub1 n)))))) fact) 5)) "));
      }
    };

  struct letrec2 : public compile_fixture {
    void test()
      {
      TEST_EQ("12", run("(letrec() 12) "));
      TEST_EQ("12", run("(letrec([f 12]) f) "));
      TEST_EQ("25", run("(letrec([f 12][g 13]) (+ f g)) "));
      TEST_EQ("120", run("(letrec([fact (lambda(n) (if (zero? n)  1  (* n(fact(sub1 n)))))]) (fact 5)) "));
      TEST_EQ("12", run("(letrec([f 12][g(lambda() f)]) (g)) "));
      TEST_EQ("130", run("(letrec([f 12][g(lambda(n) (set! f n))])(g 130) f) "));
      TEST_EQ("12", run("(letrec([f (lambda(g) (set! f g) (f))]) (f(lambda() 12))) "));
      TEST_EQ("100", run("(letrec([f (cons (lambda() f) (lambda(x) (set! f x)))]) (let([g(car f)]) ((cdr f) 100) (g))) "));
      TEST_EQ("1", run("(let([v (vector 1 2 3)])(vector-ref v 0))"));
      TEST_EQ("#(5 2 3)", run("(let([v (vector 1 2 3)])(vector-set! v 0 5) v)"));
      TEST_EQ("48", run("(letrec([f(letrec([g(lambda(x) (* x 2))]) (lambda(n) (g(* n 2))))]) (f 12)) "));
      TEST_EQ("120", run("(letrec([f(lambda(f n) (if (zero? n)  1 (* n(f f(sub1 n)))))]) (f f 5)) "));
      TEST_EQ("120", run("(let([f(lambda(f) (lambda(n) (if (zero? n)  1 (* n(f(sub1 n))))))]) (letrec([fix (lambda(f) (f(lambda(n) ((fix f) n))))])((fix f) 5))) "));
      }
    };


  struct inner_define : public compile_fixture {
    void test()
      {
      TEST_EQ("7", run("(let()(define x 3)(define y 4)(+ x y))"));
      //TEST_EQ("6", run("(let()(letrec ([x 3][y x]) (+ x y)))")); // this violates r5rs letrec definition
      //TEST_EQ("6", run("(letrec ([x 3][y x]) (+ x y))")); // this violates r5rs letrec definition
      //TEST_EQ("6", run("(let()(define x 3)(define y x)(+ x y))"));  // this violates r5rs letrec / define definition : it must be possible to evaluate each (expression) of every internal definition in a body without assigning or referring to the value of any variable being defined
      //TEST_EQ("8", run("(let()(define x 3)(set! x 4)(define y x)(+ x y))"));  // this violates r5rs letrec / define definition
      TEST_EQ("45", run("(let([x 5])(define foo(lambda(y) (bar x y))) (define bar(lambda(a b) (+(* a b) a))) (foo(+ x 3)))"));
      }
    };

  struct global_define : public compile_fixture {
    void test()
      {
      TEST_EQ("3", run("(define x 3) x"));
      TEST_EQ("4", run("(define x 3) (set! x 4) x"));
      TEST_EQ("4", run("(begin (define x 3) (set! x 4) x)"));
      }
    };
  struct list : public compile_fixture {
    void test()
      {
      TEST_EQ("()", run("(list)"));
      TEST_EQ("(3)", run("(list 3)"));
      TEST_EQ("(3 6)", run("(list 3 6)"));
      TEST_EQ("(1 2 3)", run("(list 1 2 3)"));
      TEST_EQ("(1 2 3 4)", run("(list 1 2 3 4)"));
      TEST_EQ("(1 2 3 4 5)", run("(list 1 2 3 4 5)"));
      TEST_EQ("(1 2 3 4 5 6)", run("(list 1 2 3 4 5 6)"));
      TEST_EQ("(1 2 3 4 5 6 7)", run("(list 1 2 3 4 5 6 7)"));
      TEST_EQ("(1 2 3 4 5 6 7 8)", run("(list 1 2 3 4 5 6 7 8)"));
      TEST_EQ("(1 2 3 4 5 6 7 8 9)", run("(list 1 2 3 4 5 6 7 8 9)"));
      TEST_EQ("(1 2 3 4 5 6 7 8 9 10)", run("(list 1 2 3 4 5 6 7 8 9 10)"));
      TEST_EQ("(1 2 3 4 5 6 7 8 9 10 11)", run("(list 1 2 3 4 5 6 7 8 9 10 11)"));
      TEST_EQ("(1 2 3 4 5 6 7 8 9 10 11 12)", run("(list 1 2 3 4 5 6 7 8 9 10 11 12)"));
      }
    };
/*
  struct scheme_tests : public compile_fixture {
    void test()
      {
      build_string_to_symbol();
      TEST_EQ("(testing 1 (2) -3.14e+159)", run("(quote (testing 1 (2.0) -3.14e159))"));
      TEST_EQ("4", run("(+ 2 2)"));
      TEST_EQ("210", run("(+ (* 2 100) (* 1 10))"));
      TEST_EQ("2", run("(if (> 6 5) (+ 1 1) (+ 2 2))"));
      TEST_EQ("4", run("(if (< 6 5) (+ 1 1) (+ 2 2))"));
      TEST_EQ("3", run("(define x 3)"));
      TEST_EQ("3", run("x"));
      TEST_EQ("6", run("(+ x x)"));
      TEST_EQ("3", run("(begin (define x 1) (set! x (+ x 1)) (+ x 1))"));
      TEST_EQ("10", run("((lambda (x) (+ x x)) 5)"));
      TEST_EQ("<lambda>", run("(define twice (lambda (x) (* 2 x)))"));
      TEST_EQ("10", run("(twice 5)"));
      TEST_EQ("<lambda>", run("(define cube (lambda (x) (* x x x)))"));
      TEST_EQ("125", run("(cube 5)"));
      TEST_EQ("<lambda>", run("(define compose (lambda (f g) (lambda (x) (f (g x)))))"));
      TEST_EQ("<lambda>", run("(define repeat (lambda (f) (compose f f)))"));
      TEST_EQ("20", run("((repeat twice) 5)"));
      TEST_EQ("80", run("((repeat (repeat twice)) 5)"));
      TEST_EQ("120", run("(let ([x 5]) (define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1)))))) (fact x))"));
      TEST_EQ("<lambda>", run("(define fact (lambda (n) (if (<= n 1) 1 (* n (fact (- n 1))))))"));
      TEST_EQ("6", run("(fact 3)"));
      TEST_EQ("479001600", run("(fact 12)"));
      TEST_EQ("<lambda>", run("(define make_list (lambda (n) (list n)))"));
      TEST_EQ("(10)", run("((compose make_list twice) 5)"));
      TEST_EQ("<lambda>", run("(define abs (lambda (n) (if (> n 0) n (- 0 n))))"));
      TEST_EQ("(3 0 3)", run("(list (abs -3) (abs 0) (abs 3))"));
      TEST_EQ("<lambda>", run("(define make_cons (lambda (m n) (cons m n)))"));
      TEST_EQ("(1 . 2)", run("(make_cons 1 2)"));

      TEST_EQ("<lambda>", run("(define combine (lambda (f) (lambda (x y) (if (null? x) (quote ()) (f (list (car x) (car y)) ((combine f) (cdr x) (cdr y)))))))"));
      TEST_EQ("<lambda>", run("(define zip (combine make_cons))"));
      TEST_EQ("((1 5) (2 6) (3 7) (4 8))", run("(zip (list 1 2 3 4) (list 5 6 7 8))"));
      }
    };
*/
  struct scheme_tests_part_b : public compile_fixture {
    void test()
      {
      run("(define append (lambda (l m)(if (null? l) m (cons(car l) (append(cdr l) m)))))");
      TEST_EQ("<lambda>", run("(define combine (lambda (f) (lambda (x y) (if (null? x) (quote ()) (f (list (car x) (car y)) ((combine f) (cdr x) (cdr y)))))))"));
      TEST_EQ("<lambda>", run("(define riff-shuffle (lambda (deck) (begin (define take (lambda (n seq) (if (<= n 0) (quote ()) (cons (car seq) (take (- n 1) (cdr seq)))))) (define drop (lambda (n seq) (if (<= n 0) seq (drop (- n 1) (cdr seq))))) (define mid (lambda (seq) (/ (length seq) 2))) ((combine append) (take (mid deck) deck) (drop (mid deck) deck)))))"));
      TEST_EQ("(1 2 3 4 5 6 7 8)", run("(cons 1 (cons 2 (cons 3 (cons 4 (cons 5 (cons 6 (cons 7 (cons 8 ()))))))))"));
      TEST_EQ("(1 5 2 6 3 7 4 8)", run("(riff-shuffle (list 1 2 3 4 5 6 7 8))"));
      TEST_EQ("(1 2 3 4 5 6 7 8)", run("(riff-shuffle (riff-shuffle (riff-shuffle (list 1 2 3 4 5 6 7 8))))"));
      }
    };

  struct scheme_tests_part_c : public compile_fixture {
    void test()
      {
      run("(define append (lambda (l m)(if (null? l) m (cons(car l) (append(cdr l) m)))))");
      TEST_EQ("<lambda>", run("(define combine (lambda (f) (lambda (x y) (if (null? x) (quote ()) (f (list (car x) (car y)) ((combine f) (cdr x) (cdr y)))))))"));
      TEST_EQ("<lambda>", run("(define riff-shuffle (lambda (deck) (begin (define take (lambda (n seq) (if (<= n 0) (quote ()) (cons (car seq) (take (- n 1) (cdr seq)))))) (define drop (lambda (n seq) (if (<= n 0) seq (drop (- n 1) (cdr seq))))) (define mid (lambda (seq) (/ (length seq) 2))) ((combine append) (take (mid deck) deck) (drop (mid deck) deck)))))"));
      TEST_EQ("<lambda>", run("(define compose (lambda (f g) (lambda (x) (f (g x)))))"));
      TEST_EQ("<lambda>", run("(define repeat (lambda (f) (compose f f)))"));
      TEST_EQ("(1 3 5 7 2 4 6 8)", run("((repeat riff-shuffle) (list 1 2 3 4 5 6 7 8))"));
      }
    };

  struct scheme_tests_2 : public compile_fixture {
    void test()
      {
      TEST_EQ("<lambda>", run("(define multiply-by (lambda (n) (lambda (y) (* y n))))"));
      TEST_EQ("<lambda>", run("(define doubler (multiply-by 2))"));
      TEST_EQ("<lambda>", run("(define tripler (multiply-by 3))"));
      TEST_EQ("8", run("(doubler 4)"));
      TEST_EQ("12", run("(tripler 4)"));
      TEST_EQ("9", run("(doubler 4.5)"));
      TEST_EQ("13.5", run("(tripler 4.5)"));
      }
    };

  struct scheme_tests_3 : public compile_fixture {
    void test()
      {
      TEST_EQ("<lambda>", run("(define count-down-from (lambda (n) (lambda () (set! n (- n 1)))))"));
      TEST_EQ("<lambda>", run("(define count-down-from-3  (count-down-from 3))"));
      TEST_EQ("<lambda>", run("(define count-down-from-4  (count-down-from 4))"));
      TEST_EQ("2", run("(count-down-from-3)"));
      TEST_EQ("3", run("(count-down-from-4)"));
      TEST_EQ("1", run("(count-down-from-3)"));
      TEST_EQ("0", run("(count-down-from-3)"));
      TEST_EQ("2", run("(count-down-from-4)"));
      TEST_EQ("1", run("(count-down-from-4)"));
      TEST_EQ("0", run("(count-down-from-4)"));
      TEST_EQ("-1", run("(count-down-from-4)"));
      TEST_EQ("-1", run("(count-down-from-3)"));
      }
    };

  struct scheme_tests_4 : public compile_fixture {
    void test()
      {
      TEST_EQ("0", run("(define set-hidden 0)"));
      TEST_EQ("0", run("(define get-hidden 0)"));
      TEST_EQ("<lambda>", run("((lambda () (begin (define hidden 0) (set! set-hidden (lambda (n) (set! hidden n))) (set! get-hidden (lambda () hidden)))))"));
      TEST_EQ("0", run("(get-hidden)"));
      TEST_EQ("1234", run("(set-hidden 1234)"));
      TEST_EQ("1234", run("(get-hidden)"));
      }
    };
  }
  
COMPILER_END

void run_all_compile_tests()
  {
  using namespace COMPILER;
  for (int i = 0; i < 3; ++i)
    {
    g_ops = compiler_options();
    switch (i)
      {
      case 0:
        break;
      case 1:
        g_ops.primitives_inlined = false;
        break;
      case 2:
        g_ops.safe_cons = false;
        g_ops.safe_flonums = false;
        g_ops.safe_promises = false;
        g_ops.safe_primitives = false;
        break;
      default:
        break;
      }
    fixnums().test();
    bools().test();
    test_for_nil().test();
    chars().test();
    doubles().test();
    add1().test();
    sub1().test();
    add_fixnums().test();
    add_flonums().test();
    add_flonums_and_fixnums().test();
    sub().test();
    mul().test();
    divtest().test();
    add_incorrect_argument().test();
    combination_of_math_ops().test();
    equal().test();
    not_equal().test();
    less().test();
    leq().test();
    greater().test();
    geq().test();
    compare_incorrect_argument().test();
    iftest().test();
    andtest().test();
    ortest().test();
    let().test();
    let_star().test();
    arithmetic().test();
    globals().test();
    
    vector().test();
    letrec().test();
    lambdas().test();
    is_fixnum().test();
    nottest().test();
    is_null().test();
    is_flonum().test();
    is_zero().test();
    is_boolean().test();
    is_char().test();
    cons().test();
    tailcall().test();
    begin().test();
    implicit_begin().test();
    closures().test();
    set().test();
    letrec2().test();
    inner_define().test();
    global_define().test();
    list().test();
    //scheme_tests().test();
    scheme_tests_part_b().test();
    scheme_tests_part_c().test();
    scheme_tests_2().test();
    scheme_tests_3().test();
    scheme_tests_4().test();
    }
  }
