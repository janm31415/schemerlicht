#include "schemerlicht_tests.h"

#include "schemerlicht/schemerlicht.h"

#include "test_assert.h"

#include <iostream>

COMPILER_BEGIN

namespace
  {
  struct compile_fixture_schemerlicht
    {
    compile_fixture_schemerlicht()
      {
      using namespace COMPILER;
      schemerlicht_parameters params;
      params.trace = nullptr;
      params.stderror = &std::cout;
      params.stdoutput = nullptr;
      scheme_with_schemerlicht(nullptr, nullptr, params);
      }

    ~compile_fixture_schemerlicht()
      {
      COMPILER::schemerlicht_quit();
      }

    std::string run(const std::string& script)
      {
      return COMPILER::schemerlicht_raw_to_string(COMPILER::schemerlicht_run_raw(script));
      }

    void build_values()
      {
      COMPILER::schemerlicht_run_raw("(import 'values)");
      }

    void build_dynamic_wind()
      {
      COMPILER::schemerlicht_run_raw("(import 'dynamic-wind)");
      }

    void build_srfi6()
      {
      COMPILER::schemerlicht_run_raw("(import 'srfi-6)");
      }

    void build_srfi1()
      {
      COMPILER::schemerlicht_run_raw("(import 'srfi-1)");
      }

    void build_srfi28()
      {
      COMPILER::schemerlicht_run_raw("(import 'srfi-28)");
      }

    void build_mbe()
      {
      COMPILER::schemerlicht_run_raw("(import 'mbe)");
      }

    void build_csv()
      {
      COMPILER::schemerlicht_run_raw("(import 'csv)");
      }

    void build_eval()
      {
      COMPILER::schemerlicht_run_raw("(import 'eval)");
      }
    };

  struct is_procedure : public compile_fixture_schemerlicht {
    void test()
      {
      build_values();

      TEST_EQ("#t", run("(procedure? (lambda (x) x)) "));
      TEST_EQ("#t", run("(let ([f (lambda (x) x)]) (procedure? f))"));
      TEST_EQ("#f", run(R"((procedure? (make-vector 0)))"));
      TEST_EQ("#f", run(R"((procedure? (make-string 0)))"));
      TEST_EQ("#f", run(R"((procedure? (cons 1 2)))"));
      TEST_EQ("#f", run(R"((procedure? #\S))"));
      TEST_EQ("#f", run(R"((procedure? ()))"));
      TEST_EQ("#f", run(R"((procedure? #t))"));
      TEST_EQ("#f", run(R"((procedure? #f))"));
      TEST_EQ("#f", run(R"((string? (lambda (x) x)))"));
      TEST_EQ("#f", run(R"((vector? (lambda (x) x)))"));
      TEST_EQ("#f", run(R"((boolean? (lambda (x) x)))"));
      TEST_EQ("#f", run(R"((null? (lambda (x) x)))"));
      TEST_EQ("#f", run(R"((not (lambda (x) x)))"));
      TEST_EQ("#t", run("(procedure? +)"));
      TEST_EQ("#t", run("(procedure? car)"));
      TEST_EQ("#t", run("(procedure? apply)"));
      TEST_EQ("#t", run("(procedure? (lambda (x) (* x x)))"));
      TEST_EQ("#f", run("(procedure? '(lambda (x) (* x x)))"));
      //TEST_EQ("#t", run("(call-with-current-continuation procedure?)"));
      }
    };

  struct long_apply_2_test : public compile_fixture_schemerlicht {
    void test()
      {
      build_srfi1();
      TEST_EQ("210", run("(%apply + (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20))"));
      TEST_EQ("210", run("(apply + (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20))"));
      TEST_EQ("<lambda>", run("(define (rms nums) (sqrt (/ (apply + (map * nums nums))(length nums))))"));
      TEST_EQ("11.9791", run("(rms (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20))"));
      TEST_EQ("(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)", run("(define dm (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20))"));
      TEST_EQ("(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)", run("dm"));
      TEST_EQ("11.9791", run("(rms dm)"));
      TEST_EQ("(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)", run("dm"));
      TEST_EQ("(0 1 2 3 4)", run("(iota 5)"));
      TEST_EQ("2000", run("(define dm (iota 2000)) (length dm)"));
      TEST_EQ("1999000", run("(apply + dm)"));
      TEST_EQ("2000", run("(length dm)"));
      }
    };

  struct callcc_test : public compile_fixture_schemerlicht {
    void test()
      {
      build_values();
      build_dynamic_wind();

      TEST_EQ("1", run("(call/cc (lambda(throw) (+ 5 (* 10 (throw 1)))))"));
      TEST_EQ("15", run("(call/cc (lambda(throw) (+ 5 (* 10 1))))"));
      TEST_EQ("35", run("(call/cc(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 (escape 3))))))))"));
      TEST_EQ("3", run("(call/cc(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 (throw 3))))))))"));
      TEST_EQ("1005", run("(call/cc(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 1)))))))"));

      TEST_EQ("1", run("(call-with-current-continuation (lambda(throw) (+ 5 (* 10 (throw 1)))))"));
      TEST_EQ("15", run("(call-with-current-continuation (lambda(throw) (+ 5 (* 10 1))))"));
      TEST_EQ("35", run("(call-with-current-continuation(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 (escape 3))))))))"));
      TEST_EQ("3", run("(call-with-current-continuation(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 (throw 3))))))))"));
      TEST_EQ("1005", run("(call-with-current-continuation(lambda(throw) (+ 5 (* 10 (call/cc(lambda(escape) (* 100 1)))))))"));

      TEST_EQ("12", run("(call/cc (lambda(k) 12))"));
      TEST_EQ("12", run("(call/cc (lambda(k)  (k 12)))"));
      TEST_EQ("12", run("(call/cc (lambda(k) (fx+ 1 (k 12))))"));

      TEST_EQ("25", run("(fx+ (call/cc(lambda(k) (k 12)))(call/cc(lambda(k) 13)))"));
      TEST_EQ("#f", run("(fxzero? 1)"));
      TEST_EQ("#f", run("(fxzero? -1)"));
      TEST_EQ("#t", run("(fxzero? 0)"));
      TEST_EQ("4", run("(fxsub1 5)"));
      TEST_EQ("-1", run("(fxsub1 0)"));
      TEST_EQ("1", run("(fxadd1 0)"));
      TEST_EQ("0", run("(fxadd1 -1)"));
      TEST_EQ("5", run("(fxadd1 4)"));
      TEST_EQ("1", run("(letrec([fact (lambda(n k) (cond [(fxzero? n) (k 1)][else (fx* n (fact (fxsub1 n) k))]))]) (call/cc (lambda(k)(fact 5 k)))) "));
      TEST_EQ("1", run("(call/cc   (lambda(k)  (letrec([fact    (lambda(n)    (cond  [(fxzero? n) (k 1)][else (fx* n(fact(fxsub1 n)))]))])  (fact 5))))"));
      TEST_EQ("120", run("(let([k #f])  (letrec([fact (lambda(n)  (cond [(fxzero? n)  (call/cc (lambda(nk)    (set! k nk)   (k 1)))][else (fx* n(fact(fxsub1 n)))]))]) (let([v(fact 5)])  v)))"));
      TEST_EQ("(120 . 14400)", run("(let([k #f])(letrec([fact (lambda(n) (cond [(fxzero? n) (call/cc  (lambda(nk)  (set! k nk) (k 1)))] [else (fx* n(fact(fxsub1 n)))]))]) (let([v(fact 5)])(let([nk k])  (set! k(lambda(x) (cons v x))) (nk v))))) "));
      }
    };

  std::string first_line(const std::string& long_string)
    {
    auto pos = long_string.find_first_of('\n');
    if (pos == std::string::npos)
      return long_string;
    else
      return long_string.substr(0, pos - 1);
    }

  struct calcc_extended : public compile_fixture_schemerlicht {
    void test()
      {
      build_values();
      build_dynamic_wind();
      TEST_EQ("1", run("(values 1)"));
      TEST_EQ("((multiple . values) 1 2)", run("(values 1 2)"));
      TEST_EQ("((multiple . values) 1 2 3)", run("(values 1 2 3)"));

      TEST_EQ("3", run("(call-with-values (lambda () (values 1 2)) +)"));
      TEST_EQ("runtime error: <lambda>: invalid number of arguments:", first_line(run("(call-with-values (lambda () 1) (lambda (x y) (+ x y)))")));

      TEST_EQ("5", run("(call-with-values (lambda () (values 4 5)) (lambda (a b) b))"));
      TEST_EQ("-1", run("(call-with-values * -)"));

      TEST_EQ("(2 1)", run("(define lst '()) (define before (lambda () (set! lst (cons 1 lst)))) (define thunk (lambda () (set! lst (cons 2 lst))))  (define after (lambda () (set! lst (cons 3 lst)))) (dynamic-wind before thunk after)"));
      TEST_EQ("(3 2 1)", run("(define lst '()) (define before (lambda () (set! lst (cons 1 lst)))) (define thunk (lambda () (set! lst (cons 2 lst))))  (define after (lambda () (set! lst (cons 3 lst)))) (dynamic-wind before thunk after) lst"));

      auto script =
        R"(
(let ((path '())
      (c #f))
  (let ((add (lambda (s)
              (set! path (cons s path)))))
    (dynamic-wind
      (lambda () (add 'connect))
      (lambda ()
        (add (call-with-current-continuation
              (lambda (c0)
                (set! c c0)
                'talk1))))
      (lambda () (add 'disconnect)))
    (if (< (length path) 4)
        (c 'talk2)
        (reverse path))))
)";

      TEST_EQ("(connect talk1 disconnect connect talk2 disconnect)", run(script));
      }
    };

  struct basic_string_ports : public compile_fixture_schemerlicht {
    void test()
      {
      build_srfi6();

      TEST_EQ("<port>: \"input-string\"", run("(define p (open-input-string \"21 34\"))"));
      TEST_EQ("#t", run("(input-port? p)"));
      TEST_EQ("21", run("(read p)"));
      TEST_EQ("34", run("(read p)"));
      TEST_EQ("#t", run("(eof-object? (peek-char p))"));

      TEST_EQ("<port>: \"input-string\"", run("(define p2 (open-input-string \"(a . (b . (c . ()))) 34\"))"));
      TEST_EQ("#t", run("(input-port? p2)"));
      TEST_EQ("(a b c)", run("(read p2)"));
      TEST_EQ("34", run("(read p2)"));
      TEST_EQ("#t", run("(eof-object? (peek-char p2))"));

      std::string script = R"(
(let ([q (open-output-string)]
      [x '(a b c)])
          (write (car x) q)
          (write (cdr x) q)
          (get-output-string q))
)";
      TEST_EQ("\"a(b c)\"", run(script));

      std::string script2 = R"(
(let ([os (open-output-string)])
          (write "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" os)          
          (get-output-string os))
)";
      TEST_EQ("\"\"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz\"\"", run(script2));
      }
    };

  struct format_tests : public compile_fixture_schemerlicht {
    void test()
      {
      build_srfi6();
      build_srfi28();

      TEST_EQ("\"Hello, World!\"", run("(format \"Hello, ~a\" \"World!\")"));
      TEST_EQ("\"Error, list is too short: (one \"two\" 3)\n\"", run("(format \"Error, list is too short: ~s~%\" '(one \"two\" 3))"));
      }
    };
  }

COMPILER_END


void run_all_schemerlicht_tests()
  {
  using namespace COMPILER;
  is_procedure().test();
  //long_apply_2_test().test();
  //callcc_test().test();
  //calcc_extended().test();
  //basic_string_ports().test();
  //format_tests().test();
  }