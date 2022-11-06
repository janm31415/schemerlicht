#include "r5rs.h"
#include "parser.h"
#include "context.h"
#include "token.h"
#include "compiler.h"
#include "vm.h"
#include "simplifyconv.h"
#include "begconv.h"
#include "globdef.h"
#include "defconv.h"
#include "cps.h"
#include "lambdatolet.h"
#include "assignablevarconv.h"
#include "freevaranalysis.h"
#include "closure.h"
#include "dump.h"
#include "quotecollect.h"
#include "quoteconv.h"

#include <string.h>

static schemerlicht_vector script2tokens(schemerlicht_context* ctxt, const char* script)
  {
  schemerlicht_stream str;
  schemerlicht_stream_init(ctxt, &str, 10);
  schemerlicht_memsize len = cast(schemerlicht_memsize, strlen(script));
  schemerlicht_stream_write(ctxt, &str, script, len, 0);
  schemerlicht_stream_rewind(&str);
  schemerlicht_vector tokens = tokenize(ctxt, &str);
  schemerlicht_stream_close(ctxt, &str);
  return tokens;
  }

schemerlicht_function* schemerlicht_compile_r5rs(schemerlicht_context* ctxt)
  {
  char* script ="(define caar(lambda(x) (car(car x))))"
  "(define cadr(lambda(x) (car(cdr x))))"
  "(define cdar(lambda(x) (cdr(car x))))"
  "(define cddr(lambda(x) (cdr(cdr x))))"
  "(define caaar(lambda(x) (car(car(car x)))))"
  "(define caadr(lambda(x) (car(car(cdr x)))))"
  "(define cadar(lambda(x) (car(cdr(car x)))))"
  "(define caddr(lambda(x) (car(cdr(cdr x)))))"
  "(define cdaar(lambda(x) (cdr(car(car x)))))"
  "(define cdadr(lambda(x) (cdr(car(cdr x)))))"
  "(define cddar(lambda(x) (cdr(cdr(car x)))))"
  "(define cdddr(lambda(x) (cdr(cdr(cdr x)))))"
  "(define caaaar(lambda(x) (car(car(car(car x))))))"
  "(define caaadr(lambda(x) (car(car(car(cdr x))))))"
  "(define caadar(lambda(x) (car(car(cdr(car x))))))"
  "(define caaddr(lambda(x) (car(car(cdr(cdr x))))))"
  "(define cadaar(lambda(x) (car(cdr(car(car x))))))"
  "(define cadadr(lambda(x) (car(cdr(car(cdr x))))))"
  "(define caddar(lambda(x) (car(cdr(cdr(car x))))))"
  "(define cadddr(lambda(x) (car(cdr(cdr(cdr x))))))"
  "(define cdaaar(lambda(x) (cdr(car(car(car x))))))"
  "(define cdaadr(lambda(x) (cdr(car(car(cdr x))))))"
  "(define cdadar(lambda(x) (cdr(car(cdr(car x))))))"
  "(define cdaddr(lambda(x) (cdr(car(cdr(cdr x))))))"
  "(define cddaar(lambda(x) (cdr(cdr(car(car x))))))"
  "(define cddadr(lambda(x) (cdr(cdr(car(cdr x))))))"
  "(define cdddar(lambda(x) (cdr(cdr(cdr(car x))))))"
  "(define cddddr(lambda(x) (cdr(cdr(cdr(cdr x))))))"
  "(define map(lambda(proc lst1 . lsts)"
  "   (if (null? lsts)"
  "      (let loop([lst lst1])"
  "        (if (null? lst)"
  "          '()"
  "          (cons (proc (car lst)) (loop(cdr lst)))"
  "          ))"
  "      (let loop ([lsts(cons lst1 lsts)])"
  "        (let ([hds(let loop2([lsts lsts])"
  "          (if (null? lsts)"
  "            '()"
  "            (let([x(car lsts)])"
  "              (and (not (null? x))"
  "                (let([r(loop2(cdr lsts))])"
  "                  (and r(cons(car x) r))"
  "                  )))))])"
  "          (if hds"
  "          (cons(apply proc hds)"
  "            (loop(let loop3([lsts lsts])"
  "              (if (null? lsts)"
  "                '()"
  "                (cons(cdar lsts) (loop3(cdr lsts)))"
  "                ))))"
  "            '()"
  "            ))))))";


  schemerlicht_vector tokens = script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);  
  //schemerlicht_quasiquote_conversion(ctxt, &prog);
  schemerlicht_define_conversion(ctxt, &prog);
  schemerlicht_single_begin_conversion(ctxt, &prog);
  schemerlicht_simplify_to_core_forms(ctxt, &prog);
  //schemerlicht_alpha_conversion(ctxt, &prog);
  schemerlicht_vector quotes = schemerlicht_quote_collection(ctxt, &prog);
  schemerlicht_quote_conversion(ctxt, &prog, &quotes);
  schemerlicht_quote_collection_destroy(ctxt, &quotes);
  schemerlicht_global_define_environment_allocation(ctxt, &prog);
  schemerlicht_continuation_passing_style(ctxt, &prog);
  schemerlicht_lambda_to_let_conversion(ctxt, &prog);
  schemerlicht_assignable_variable_conversion(ctxt, &prog);
  schemerlicht_free_variable_analysis(ctxt, &prog);
  schemerlicht_closure_conversion(ctxt, &prog);

  schemerlicht_function* r5rs = schemerlicht_compile_expression(ctxt, schemerlicht_vector_at(&prog.expressions, 0, schemerlicht_expression));
  schemerlicht_run(ctxt, r5rs);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  return r5rs;
  }