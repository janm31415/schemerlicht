#include "modules.h"
#include "parser.h"
#include "context.h"
#include "token.h"
#include "compiler.h"
#include "vm.h"
#include "preprocess.h"
#include "syscalls.h"
#include "string.h"
#include "dump.h"


void schemerlicht_compile_modules(schemerlicht_context* ctxt, const char* module_path)
  {
  if (module_path != ctxt->module_path.string_ptr)
    {
    schemerlicht_string_clear(&ctxt->module_path);
    schemerlicht_string_append_cstr(ctxt, &ctxt->module_path, module_path);
    }
  if (ctxt->module_path.string_length == 0)
    return;
  const char* current_module_path = schemerlicht_getenv("SCHEMERLICHT_MODULE_PATH");
  if (current_module_path)
    {
    schemerlicht_string_clear(&ctxt->module_path);
    schemerlicht_string_append_cstr(ctxt, &ctxt->module_path, current_module_path);
    }
  else
    {
    schemerlicht_putenv("SCHEMERLICHT_MODULE_PATH", module_path);
    }
  char* script = "(define *modules* '())\n"
    "(define *loaded-modules* '())\n"    
    "(define (module-name->strings ls res)\n"
    "  (if (null? ls)\n"
    "      res\n"
    "      (let ((str (cond ((symbol? (car ls)) (symbol->string (car ls)))\n"
    "                       ((number? (car ls)) (number->string (car ls)))\n"
    "                       ((string? (car ls)) (car ls))\n"
    "                       (else (error \"invalid module name\" (car ls))))))\n"
    "        (module-name->strings (cdr ls) (cons \"/\" (cons str res))))))\n"
    "(define (module-name->file name)\n"
    "  (apply string-append\n"
    "   (reverse (cons \".scm\" (cdr (module-name->strings name '()))))))\n"
    "(define (module-name->path name)\n"
    "  (let ((file (module-name->file name))\n"
    "        (module-path (getenv \"SCHEMERLICHT_MODULE_PATH\")))\n"
    "    (if module-path\n"
    "        (string-append module-path file)  \n"
    "        (begin (error \"invalid schemerlicht module path\") #f)\n"
    "    )\n"
    "  )\n"
    ") \n"
    "(define (load-module name)\n"
    "  (let ((path (module-name->path name)))\n"
    "    (if path (load path))\n"
    "  )\n"
    ")\n"
    "(define (find-module name)\n"
    "  (cond\n"
    "   ((assoc name *modules*) => cdr)\n"
    "   (else #f)))\n"
    "(define (find-loaded-module name)\n"
    "  (cond\n"
    "   ((assoc name *loaded-modules*) => cdr)\n"
    "   (else #f)))  \n"
    "(define (add-module! name module)\n"
    "  (set! *modules* (cons (cons name module) *modules*))) \n"
    "(define (add-loaded-module! name module)\n"
    "  (set! *loaded-modules* (cons (cons name module) *loaded-modules*))) \n"
    "(define (imported? name)\n"
    "  (let ((m (find-loaded-module name)))\n"
    "    (if m #t #f)\n"
    "  )\n"
    ") \n"
    "(define (import name)\n"
    "  (if (not (imported? name))\n"
    "      (let ((m (find-module name)))\n"
    "        (if (not m)\n"
    "            (error \"Invalid module\")\n"
    "            (begin\n"
    "              (load-module (vector-ref m 1))\n"
    "              (add-loaded-module! name m)\n"
    "              #t\n"
    "            )\n"
    "        )\n"
    "      )\n"
    "      #t\n"
    "  )    \n"
    ")\n"
    "(define (make-module name files) (vector name files))\n"
    "(defmacro define-module (name filelist)\n"
    "  `(let ((m (make-module (quote ,name) (let ((files (quote ,filelist))) \n"
    "                                (if (list? files)\n"
    "                                    files\n"
    "                                    (list files)\n"
    "                                )    \n"
    "                              )\n"
    "            )))\n"
    "   (if (not (find-module (quote ,name)))\n"
    "            (begin (add-module! (quote ,name) m) #t)\n"
    "            (error \"module already defined\")\n"
    "            )\n"
    "   )\n"
    ")\n"
    "(load-module '(packages))\n";

  schemerlicht_vector tokens = schemerlicht_script2tokens(ctxt, script);
  schemerlicht_program prog = make_program(ctxt, &tokens);
  schemerlicht_preprocess(ctxt, &prog);
#if 0
  schemerlicht_string dmp = schemerlicht_dump(ctxt, &prog);
  printf("%s\n\n", dmp.string_ptr);
  schemerlicht_string_destroy(ctxt, &dmp);
#endif
  schemerlicht_vector modules = schemerlicht_compile_program(ctxt, &prog);
  schemerlicht_run_program(ctxt, &modules);
  destroy_tokens_vector(ctxt, &tokens);
  schemerlicht_program_destroy(ctxt, &prog);
  schemerlicht_compiled_program_register(ctxt, &modules);
  }
