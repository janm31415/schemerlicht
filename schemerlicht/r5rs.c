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
  char* script ="(define caar(lambda(x) (car(car x))))\n"
  "(define cadr(lambda(x) (car(cdr x))))\n"
  "(define cdar(lambda(x) (cdr(car x))))\n"
  "(define cddr(lambda(x) (cdr(cdr x))))\n"
  "(define caaar(lambda(x) (car(car(car x)))))\n"
  "(define caadr(lambda(x) (car(car(cdr x)))))\n"
  "(define cadar(lambda(x) (car(cdr(car x)))))\n"
  "(define caddr(lambda(x) (car(cdr(cdr x)))))\n"
  "(define cdaar(lambda(x) (cdr(car(car x)))))\n"
  "(define cdadr(lambda(x) (cdr(car(cdr x)))))\n"
  "(define cddar(lambda(x) (cdr(cdr(car x)))))\n"
  "(define cdddr(lambda(x) (cdr(cdr(cdr x)))))\n"
  "(define caaaar(lambda(x) (car(car(car(car x))))))\n"
  "(define caaadr(lambda(x) (car(car(car(cdr x))))))\n"
  "(define caadar(lambda(x) (car(car(cdr(car x))))))\n"
  "(define caaddr(lambda(x) (car(car(cdr(cdr x))))))\n"
  "(define cadaar(lambda(x) (car(cdr(car(car x))))))\n"
  "(define cadadr(lambda(x) (car(cdr(car(cdr x))))))\n"
  "(define caddar(lambda(x) (car(cdr(cdr(car x))))))\n"
  "(define cadddr(lambda(x) (car(cdr(cdr(cdr x))))))\n"
  "(define cdaaar(lambda(x) (cdr(car(car(car x))))))\n"
  "(define cdaadr(lambda(x) (cdr(car(car(cdr x))))))\n"
  "(define cdadar(lambda(x) (cdr(car(cdr(car x))))))\n"
  "(define cdaddr(lambda(x) (cdr(car(cdr(cdr x))))))\n"
  "(define cddaar(lambda(x) (cdr(cdr(car(car x))))))\n"
  "(define cddadr(lambda(x) (cdr(cdr(car(cdr x))))))\n"
  "(define cdddar(lambda(x) (cdr(cdr(cdr(car x))))))\n"
  "(define cddddr(lambda(x) (cdr(cdr(cdr(cdr x))))))\n"
  "(define map(lambda(proc lst1 . lsts)\n"
  "   (if (null? lsts)\n"
  "      (let loop([lst lst1])\n"
  "        (if (null? lst)\n"
  "          '()\n"
  "          (cons (proc (car lst)) (loop(cdr lst)))\n"
  "          ))\n"
  "      (let loop ([lsts(cons lst1 lsts)])\n"
  "        (let ([hds(let loop2([lsts lsts])\n"
  "          (if (null? lsts)\n"
  "            '()\n"
  "            (let([x(car lsts)])\n"
  "              (and (not (null? x))\n"
  "                (let([r(loop2(cdr lsts))])\n"
  "                  (and r(cons(car x) r))\n"
  "                  )))))])\n"
  "          (if hds"
  "          (cons(apply proc hds)\n"
  "            (loop(let loop3([lsts lsts])\n"
  "              (if (null? lsts)\n"
  "                '()\n"
  "                (cons(cdar lsts) (loop3(cdr lsts)))\n"
  "                ))))\n"
  "            '()\n"
  "            ))))))\n"
  " (define for-each(lambda(proc lst1 . lsts)\n"
  "   (if (null? lsts)\n"
  "     (let loop([lst lst1])\n"
  "       (unless(null? lst)\n"
  "         (proc(car lst))\n"
  "         (loop(cdr lst))))\n"
  "     (let loop([lsts(cons lst1 lsts)])\n"
  "       (let([hds(let loop2([lsts lsts])\n"
  "         (if (null? lsts)\n"
  "           '()\n"
  "           (let([x(car lsts)])\n"
  "             (and (not (null? x))\n"
  "               (let([r(loop2(cdr lsts))])\n"
  "                 (and r(cons(car x) r)))))))])\n"
  "         (when hds(apply proc hds)\n"
  "           (loop(let loop3([lsts lsts])\n"
  "             (if (null? lsts)\n"
  "               '()\n"
  "               (cons(cdar lsts) (loop3(cdr lsts)))\n"
  "               )))))))))\n";



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