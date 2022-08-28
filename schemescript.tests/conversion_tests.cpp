#include "conversion_tests.h"
#include "test_assert.h"

#include <iostream>
#include <string>

#include "schemescript/simplify_to_core.h"
#include "schemescript/dump.h"

COMPILER_BEGIN

namespace
  {
  std::string to_string(Program& prog)
    {
    std::stringstream str;
    dump(str, prog);
    return str.str();
    }
    
  void simplify_to_core_conversion_and()
    {
    auto tokens = tokenize("(and)");
    std::reverse(tokens.begin(), tokens.end());
    auto prog = make_program(tokens);
    simplify_to_core_forms(prog);
    TEST_ASSERT(std::holds_alternative<True>(std::get<Literal>(prog.expressions.front())));

    tokens = tokenize("(and 3)");
    std::reverse(tokens.begin(), tokens.end());
    prog = make_program(tokens);
    simplify_to_core_forms(prog);
    TEST_ASSERT(std::holds_alternative<Fixnum>(std::get<Literal>(prog.expressions.front())));
    TEST_ASSERT(std::get<Fixnum>(std::get<Literal>(prog.expressions.front())).value == 3);

    tokens = tokenize("(and 3 4)");
    std::reverse(tokens.begin(), tokens.end());
    prog = make_program(tokens);
    simplify_to_core_forms(prog);
    TEST_ASSERT(std::holds_alternative<If>(prog.expressions.front()));
    TEST_ASSERT(std::get<Fixnum>(std::get<Literal>(std::get<If>(prog.expressions.front()).arguments[0])).value == 3);
    }

  void simplify_to_core_conversion_or()
    {
    auto tokens = tokenize("(or)");
    std::reverse(tokens.begin(), tokens.end());
    auto prog = make_program(tokens);
    simplify_to_core_forms(prog);
    TEST_ASSERT(std::holds_alternative<False>(std::get<Literal>(prog.expressions.front())));

    tokens = tokenize("(or 3)");
    std::reverse(tokens.begin(), tokens.end());
    prog = make_program(tokens);
    simplify_to_core_forms(prog);
    TEST_ASSERT(std::holds_alternative<Fixnum>(std::get<Literal>(prog.expressions.front())));
    TEST_ASSERT(std::get<Fixnum>(std::get<Literal>(prog.expressions.front())).value == 3);

    tokens = tokenize("(or 3 4)");
    std::reverse(tokens.begin(), tokens.end());
    prog = make_program(tokens);
    simplify_to_core_forms(prog);
    TEST_ASSERT(std::holds_alternative<Let>(prog.expressions.front()));
    TEST_ASSERT(to_string(prog) == "( let ( [ #%x 3 ] ) ( begin ( if #%x #%x 4 ) ) ) ");    
    }

  void simplify_to_core_conversion_letrec()
    {
    auto tokens = tokenize("(letrec ([f (lambda () 5)]) (- 20 (f)))");
    std::reverse(tokens.begin(), tokens.end());
    auto prog = make_program(tokens);
    simplify_to_core_forms(prog);
    TEST_ASSERT(to_string(prog) == "( let ( [ f #undefined ] ) ( begin ( let ( [ #%t0 ( lambda ( ) ( begin 5 ) ) ] ) ( begin ( set! f #%t0 ) ) ) ( - 20 ( f ) ) ) ) ");

    tokens = tokenize("(letrec ([f (lambda (x y) (+ x y))] [g(lambda(x) (+ x 12))])(f 16 (f(g 0) (+ 1 (g 0)))))");
    std::reverse(tokens.begin(), tokens.end());
    prog = make_program(tokens);
    simplify_to_core_forms(prog);
    TEST_ASSERT(to_string(prog) == "( let ( [ f #undefined ] [ g #undefined ] ) ( begin ( let ( [ #%t0 ( lambda ( x y ) ( begin ( + x y ) ) ) ] [ #%t1 ( lambda ( x ) ( begin ( + x 12 ) ) ) ] ) ( begin ( set! f #%t0 ) ( set! g #%t1 ) ) ) ( f 16 ( f ( g 0 ) ( + 1 ( g 0 ) ) ) ) ) ) ");
    }
  }

COMPILER_END

void run_all_conversion_tests()
  {
  using namespace COMPILER;
  simplify_to_core_conversion_and();
  simplify_to_core_conversion_or();
  simplify_to_core_conversion_letrec();
  }
