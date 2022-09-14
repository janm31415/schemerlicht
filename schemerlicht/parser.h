#ifndef SCHEMERLICHT_PARSER_H
#define SCHEMERLICHT_PARSER_H

#include "schemerlicht.h"
#include "string.h"
#include "vector.h"
#include "map.h"
#include "reader.h"

#define schemerlicht_type_fixnum 0
#define schemerlicht_type_flonum 1
#define schemerlicht_type_nil 2
#define schemerlicht_type_string 3
#define schemerlicht_type_true 4
#define schemerlicht_type_false 5
#define schemerlicht_type_character 6
#define schemerlicht_type_symbol 7

typedef struct schemerlicht_parsed_fixnum
  {
  schemerlicht_fixnum value;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_fixnum;

typedef struct schemerlicht_parsed_flonum
  {
  schemerlicht_flonum value;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_flonum;

typedef struct schemerlicht_parsed_nil
  {
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_nil;

typedef struct schemerlicht_parsed_string
  {
  schemerlicht_string value;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_string;

typedef struct schemerlicht_parsed_true
  {
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_true;

typedef struct schemerlicht_parsed_false
  {
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_false;

typedef struct schemerlicht_parsed_character
  {
  char value;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_character;

typedef struct schemerlicht_parsed_symbol
  {
  schemerlicht_string value;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_symbol;

typedef union
  {
  schemerlicht_parsed_fixnum fx;
  schemerlicht_parsed_flonum fl;
  schemerlicht_parsed_nil nil;
  schemerlicht_parsed_string str;
  schemerlicht_parsed_true t;
  schemerlicht_parsed_false f;
  schemerlicht_parsed_character ch;
  schemerlicht_parsed_symbol sym;
  } schemerlicht_parsed_literal;

typedef struct schemerlicht_literal
  {
  int type;
  schemerlicht_parsed_literal lit;
  } schemerlicht_literal;

#define schemerlicht_type_literal 0
#define schemerlicht_type_variable 1
#define schemerlicht_type_nop 2
#define schemerlicht_type_primitive_call 3
#define schemerlicht_type_foreign_call 4
#define schemerlicht_type_if 5
#define schemerlicht_type_cond 6
#define schemerlicht_type_do 7
#define schemerlicht_type_case 8
#define schemerlicht_type_quote 9
#define schemerlicht_type_let 10
#define schemerlicht_type_lambda 11
#define schemerlicht_type_funcall 12
#define schemerlicht_type_begin 13
#define schemerlicht_type_set 14

typedef struct schemerlicht_parsed_variable
  {
  schemerlicht_string name;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_variable;

typedef struct schemerlicht_parsed_nop
  {
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_nop;

typedef struct schemerlicht_parsed_primitive_call
  {
  schemerlicht_string name;
  schemerlicht_vector arguments;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.
  int as_object; // primitives can be passed as object
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_primitive_call;

typedef struct schemerlicht_parsed_foreign_call
  {
  schemerlicht_string name;
  schemerlicht_vector arguments;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_foreign_call;

typedef struct schemerlicht_parsed_if
  {
  schemerlicht_vector arguments;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_if;

typedef struct schemerlicht_parsed_cond
  {
  schemerlicht_vector arguments;
  schemerlicht_vector is_proc;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_cond;

typedef struct schemerlicht_parsed_do
  {
  schemerlicht_vector bindings; // [<variable1> <init1> <step1>] ...
  schemerlicht_vector test; // <test> <expr> ...
  schemerlicht_vector commands; // <command> ...  
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_do;

typedef struct schemerlicht_parsed_case
  {
  schemerlicht_vector val_expr;
  schemerlicht_vector datum_args;
  schemerlicht_vector then_bodies;
  schemerlicht_vector else_body;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_case;

enum schemerlicht_quote_type
  {
  schemerlicht_qt_quote,
  schemerlicht_qt_backquote,
  schemerlicht_qt_unquote,
  schemerlicht_qt_unquote_splicing
  };

typedef struct schemerlicht_parsed_quote
  {
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  enum schemerlicht_quote_type qt;
  schemerlicht_cell arg;
  } schemerlicht_parsed_quote;

enum schemerlicht_binding_type
  {
  schemerlicht_bt_let,
  schemerlicht_bt_letrec,
  schemerlicht_bt_let_star
  };

typedef struct schemerlicht_parsed_let
  {
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  schemerlicht_vector bindings;
  schemerlicht_vector body;
  enum schemerlicht_binding_type bt;
  schemerlicht_vector assignable_variables;
  int named_let;
  schemerlicht_string let_name;
  } schemerlicht_parsed_let;

typedef struct schemerlicht_parsed_lambda
  {
  int line_nr, column_nr;
  int variable_arity;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  schemerlicht_vector body;
  schemerlicht_vector variables;
  schemerlicht_vector free_variables;
  schemerlicht_vector assignable_variables;
  } schemerlicht_parsed_lambda;

typedef struct schemerlicht_parsed_funcall
  {
  schemerlicht_vector arguments;
  schemerlicht_vector fun;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_funcall;

typedef struct schemerlicht_parsed_begin
  {
  schemerlicht_vector arguments;
  int line_nr, column_nr;
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_begin;

typedef struct schemerlicht_parsed_set
  {
  schemerlicht_string name;
  schemerlicht_vector value;
  int line_nr, column_nr;
  int originates_from_define; // external defines are rewritten as set! by define_conversion. if so, originates_from_define will be true.
  int originates_from_quote; // new quotes are rewritten as set! by quote_conversion. if so, originates_from_quote will be true.
  int tail_position; // true if expr is in tail position. boolean is set by tail_call_analysis.  
  schemerlicht_string filename; // the name of the file where this expression is read (if load is used, empty otherwise)
  } schemerlicht_parsed_set;

typedef union
  {
  schemerlicht_literal lit;
  schemerlicht_parsed_variable var;
  schemerlicht_parsed_nop nop;
  schemerlicht_parsed_primitive_call prim;
  schemerlicht_parsed_foreign_call foreign;
  schemerlicht_parsed_if i;
  schemerlicht_parsed_cond cond;
  schemerlicht_parsed_do d;
  schemerlicht_parsed_case cas;
  schemerlicht_parsed_quote quote;
  schemerlicht_parsed_let let;
  schemerlicht_parsed_lambda lambda;
  schemerlicht_parsed_funcall funcall;
  schemerlicht_parsed_begin beg;
  schemerlicht_parsed_set set;
  } schemerlicht_parsed_expression;

typedef struct schemerlicht_expression
  {
  int type;
  schemerlicht_parsed_expression expr;
  } schemerlicht_expression;

typedef struct schemerlicht_program
  {
  schemerlicht_vector expressions;
  } schemerlicht_program;


SCHEMERLICHT_API schemerlicht_program make_program(schemerlicht_context* ctxt, schemerlicht_vector* tokens);
SCHEMERLICHT_API void schemerlicht_program_destroy(schemerlicht_context* ctxt, schemerlicht_program* p);
SCHEMERLICHT_API void schemerlicht_expression_destroy(schemerlicht_context* ctxt, schemerlicht_expression* e);

schemerlicht_map* generate_expression_map(schemerlicht_context* ctxt);

typedef struct schemerlicht_let_binding
  {
  schemerlicht_string binding_name;
  schemerlicht_expression binding_expr;
  } schemerlicht_let_binding;


schemerlicht_expression schemerlicht_make_variable_expression(schemerlicht_parsed_variable* v);
schemerlicht_expression schemerlicht_make_let_expression(schemerlicht_parsed_let* l);
schemerlicht_expression schemerlicht_make_lambda_expression(schemerlicht_parsed_lambda* l);
schemerlicht_expression schemerlicht_make_begin_expression(schemerlicht_parsed_begin* b);
schemerlicht_expression schemerlicht_make_literal_expression(schemerlicht_literal* l);

schemerlicht_expression schemerlicht_init_let(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_begin(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_lambda(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_set(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_nop(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_funcall(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_primcall(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_variable(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_foreign(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_if(schemerlicht_context* ctxt);
schemerlicht_expression schemerlicht_init_quote(schemerlicht_context* ctxt);

#endif // SCHEMERLICHT_PARSER_H