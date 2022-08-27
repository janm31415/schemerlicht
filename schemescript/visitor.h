#pragma once

#include "namespace.h"
#include "parse.h"
#include "reader.h"
#include <vector>

COMPILER_BEGIN

  class VisitorBaseClass
    {
    public:
      
      bool PreVisit(Program&) { return true; }
      void PostVisit(Program&) {}
      bool PreVisitExpression(Expression&) { return true; }
      void PostVisitExpression(Expression&) {}
      bool PreVisitBinding(Expression&, cell&) { return true; }
      void PostVisitBinding(Expression&) {}
      bool PreVisitBegin(Expression&) { return true; }
      void PostVisitBegin(Expression&) { }
      
      void VisitFixnum(Expression&) { }
      void VisitFlonum(Expression&) { }
      void VisitNil(Expression&) { }
      void VisitString(Expression&) { }
      void VisitSymbol(Expression&) { }
      void VisitTrue(Expression&) { }
      void VisitFalse(Expression&) { }
      void VisitNop(Expression&) { }
      void VisitCharacter(Expression&) {}
      
      bool PreVisitPrimitiveCall(Expression&) { return true; }
      void PostVisitPrimitiveCall(Expression&) { }
      
      bool PreVisitForeignCall(Expression&) { return true; }      
      void PostVisitForeignCall(Expression&) {  }
            
      void VisitVariable(Expression&) {  }
      
      
      bool PreVisitIf(Expression&) { return true; }
      void PostVisitIf(Expression&) { }
      
      void VisitQuote(Expression&) {}
      
      bool PreVisitSet(Expression&) { return true; }
      void PostVisitSet(Expression&) { }
      
      bool PreVisitLambda(Expression&) { return true; }
      void PostVisitLambda(Expression&) { }
      
      bool PreVisitLet(Expression&) { return true; }
      void VisitLetPostBindings(Expression&) {}
      void PostVisitLet(Expression&) { }
      
      bool PreVisitFunCall(Expression&) { return true; }
      void PostVisitFunCall(Expression&) { }
      
      bool PreVisitCase(Expression&) { return true; }
      void VisitCaseElse(Expression&) {}
      void PostVisitCase(Expression& ) { }

      bool PreVisitCond(Expression&) { return true; }
      void PostVisitCond(Expression&) { }
      
      bool PreVisitDo(Expression& e) { return true; }
      void VisitDoPostBindings(Expression&) {}
      void VisitDoPostTest(Expression&) {}
      void PostVisitDo(Expression&) { }
    };
  

enum class visitor_entry_type
  {
  vet_expression,
  vet_expression_post,
  vet_binding,
  vet_binding_post,
  vet_begin,
  vet_begin_post,
  vet_case,
  vet_case_else,
  vet_case_post,
  vet_cond,
  vet_cond_post,
  vet_do,
  vet_do_post_bindings,
  vet_do_post_test,
  vet_do_post,
  vet_foreigncall,
  vet_foreigncall_post,
  vet_funcall,
  vet_funcall_post,
  vet_if,
  vet_if_post,
  vet_lambda,
  vet_lambda_post,
  vet_let,
  vet_let_post_bindings,
  vet_let_post,
  vet_literal,
  vet_nop,
  vet_primitivecall,
  vet_primitivecall_post,
  vet_quote,
  vet_set,
  vet_set_post,
  vet_variable
  };

struct visitor_entry
  {
  Expression* expr;
  visitor_entry_type type;
  cell binding;
  std::vector<Expression*> bindings;
  };
  
inline visitor_entry make_visitor_entry(Expression* e, visitor_entry_type t)
{
  visitor_entry entry;
  entry.expr = e;
  entry.type = t;
  return entry;
}

inline visitor_entry make_visitor_entry(Expression* e, visitor_entry_type t, const std::string& binding)
{
  visitor_entry entry;
  entry.expr = e;
  entry.type = t;
  entry.binding = cell(ct_string, binding);
  return entry;
}

inline visitor_entry make_visitor_entry(Expression* e, visitor_entry_type t, const cell& binding)
{
  visitor_entry entry;
  entry.expr = e;
  entry.type = t;
  entry.binding = binding;
  return entry;
}

inline visitor_entry make_visitor_entry(Expression* e, visitor_entry_type t, Expression* binding)
{
  visitor_entry entry;
  entry.expr = e;
  entry.type = t;
  entry.bindings.push_back(binding);
  return entry;
}

inline visitor_entry make_visitor_entry(Expression* e, visitor_entry_type t, std::vector<Expression*>& bindings)
{
  visitor_entry entry;
  entry.expr = e;
  entry.type = t;
  entry.bindings = bindings;
  return entry;
}

template <class TOperationFunctor>
inline void visit(visitor_entry entry, std::vector<visitor_entry>& expression_stack, TOperationFunctor& func)
{
  switch (entry.type)
  {
    case visitor_entry_type::vet_expression:
    {
      if (func.PreVisitExpression(*entry.expr))
        {
        expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_expression_post));
        switch (entry.expr->index())
          {
          case 0: // begin
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_begin));
            break;
          case 1: // Case
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_case));
            break;
          case 2: // Cond
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_cond));
            break;
          case 3: // Do
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_do));
            break;
          case 4: // ForeignCall
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_foreigncall));
            break;
          case 5: // FunCall
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_funcall));
            break;
          case 6: // If
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_if));
            break;
          case 7: // Lambda
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_lambda));
            break;
          case 8: // Let
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_let));
            break;
          case 9: // Literal
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_literal));
            break;
          case 10: // Nop
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_nop));
            break;
          case 11: // PrimitiveCall
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_primitivecall));
            break;
          case 12: // Quote
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_quote));
            break;
          case 13: // Set
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_set));
            break;
          case 14: // Variable
            expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_variable));
            break;
          default:
            break;
          }
        }
    break;
    }
    case visitor_entry_type::vet_expression_post:
    {
    func.PostVisitExpression(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_begin:
    {
    if (func.PreVisitBegin(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_begin_post));
      Begin& b = std::get<Begin>(*entry.expr);
      auto rit = b.arguments.rbegin();
      auto rend = b.arguments.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      }
    break;
    }
    case visitor_entry_type::vet_begin_post:
    {
    func.PostVisitBegin(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_literal:
    {
    Literal& lit = std::get<Literal>(*entry.expr);
    switch (lit.index())
    { //std::variant<Character, False, Fixnum, Flonum, Nil, String, Symbol, True>
      case 0:
        func.VisitCharacter(*entry.expr);
        break;
      case 1:
        func.VisitFalse(*entry.expr);
        break;
      case 2:
        func.VisitFixnum(*entry.expr);
        break;
      case 3:
        func.VisitFlonum(*entry.expr);
        break;
      case 4:
        func.VisitNil(*entry.expr);
        break;
      case 5:
        func.VisitString(*entry.expr);
        break;
      case 6:
        func.VisitSymbol(*entry.expr);
        break;
      case 7:
        func.VisitTrue(*entry.expr);
        break;
      default:
      break;
    }
    break;
    }
    case visitor_entry_type::vet_nop:
    {
    func.VisitNop(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_variable:
    {
    func.VisitVariable(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_quote:
    {
    func.VisitQuote(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_funcall:
    {
    if (func.PreVisitFunCall(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_funcall_post));
      FunCall& f = std::get<FunCall>(*entry.expr);
      auto rit = f.arguments.rbegin();
      auto rend = f.arguments.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      rit = f.fun.rbegin();
      rend = f.fun.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      }
    break;
    }
    case visitor_entry_type::vet_funcall_post:
    {
    func.PostVisitFunCall(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_primitivecall:
    {
    if (func.PreVisitPrimitiveCall(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_primitivecall_post));
      PrimitiveCall& p = std::get<PrimitiveCall>(*entry.expr);
      auto rit = p.arguments.rbegin();
      auto rend = p.arguments.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      }
    break;
    }
    case visitor_entry_type::vet_primitivecall_post:
    {
    func.PostVisitPrimitiveCall(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_foreigncall:
    {
    if (func.PreVisitForeignCall(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_foreigncall_post));
      ForeignCall& f = std::get<ForeignCall>(*entry.expr);
      auto rit = f.arguments.rbegin();
      auto rend = f.arguments.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      }
    break;
    }
    case visitor_entry_type::vet_foreigncall_post:
    {
    func.PostVisitForeignCall(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_if:
    {
    if (func.PreVisitIf(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_if_post));
      If& i = std::get<If>(*entry.expr);
      auto rit = i.arguments.rbegin();
      auto rend = i.arguments.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      }
    break;
    }
    case visitor_entry_type::vet_if_post:
    {
    func.PostVisitIf(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_binding:
    {
    if (func.PreVisitBinding(*entry.expr, entry.binding))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_binding_post));
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_expression));
      auto rit = entry.bindings.rbegin();
      auto rend = entry.bindings.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry((*rit), visitor_entry_type::vet_expression));
      }
    break;
    }
    case visitor_entry_type::vet_binding_post:
    {
    func.PostVisitBinding(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_let:
    {
    if (func.PreVisitLet(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_let_post_bindings));
      Let& l = std::get<Let>(*entry.expr);
      auto rit = l.bindings.rbegin();
      auto rend = l.bindings.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(rit->second), visitor_entry_type::vet_binding, rit->first));
      }
    break;
    }
    case visitor_entry_type::vet_let_post_bindings:
    {
    func.VisitLetPostBindings(*entry.expr);
    expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_let_post));
    Let& l = std::get<Let>(*entry.expr);
    auto rit = l.body.rbegin();
    auto rend = l.body.rend();
    for (; rit != rend; ++rit)
      expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
    break;
    }
    case visitor_entry_type::vet_let_post:
    {
    func.PostVisitLet(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_lambda:
    {
    if (func.PreVisitLambda(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_lambda_post));
      Lambda& l = std::get<Lambda>(*entry.expr);
      auto rit = l.body.rbegin();
      auto rend = l.body.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      }
    break;
    }
    case visitor_entry_type::vet_lambda_post:
    {
    func.PostVisitLambda(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_set:
    {
    if (func.PreVisitSet(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_set_post));
      Set& s = std::get<Set>(*entry.expr);
      auto rit = s.value.rbegin();
      auto rend = s.value.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      }
    break;
    }
    case visitor_entry_type::vet_set_post:
    {
    func.PostVisitSet(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_case:
    {
    if (func.PreVisitCase(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_case_else));
      Case& c = std::get<Case>(*entry.expr);
      for (int32_t i = (int32_t)c.datum_args.size() - 1; i >= 0; --i)
        {
        expression_stack.push_back(make_visitor_entry(&c.then_bodies[i].front(), visitor_entry_type::vet_binding, c.datum_args[i]));
        }
      auto rit = c.val_expr.rbegin();
      auto rend = c.val_expr.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      }
    break;
    }
    case visitor_entry_type::vet_case_else:
    {
    expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_case_post));
    Case& c = std::get<Case>(*entry.expr);
    if (!c.else_body.empty())
      func.VisitCaseElse(*entry.expr);
    auto rit = c.else_body.rbegin();
    auto rend = c.else_body.rend();
    for (; rit != rend; ++rit)
      expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
    break;
    }
    case visitor_entry_type::vet_case_post:
    {
    func.PostVisitCase(*entry.expr);
    break;
    }
    
    case visitor_entry_type::vet_cond:
    {
    if (func.PreVisitCond(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_cond_post));
      Cond& c = std::get<Cond>(*entry.expr);
      auto rit = c.arguments.rbegin();
      auto rend = c.arguments.rend();
      for (; rit != rend; ++rit)
        {
        expression_stack.push_back(make_visitor_entry(&(rit->back()), visitor_entry_type::vet_binding, &(rit->front())));
        }
      }
    break;
    }
    case visitor_entry_type::vet_cond_post:
    {
    func.PostVisitCond(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_do:
    {
      if (func.PreVisitDo(*entry.expr))
      {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_do_post_bindings));
      Do& d = std::get<Do>(*entry.expr);
      auto rit = d.bindings.rbegin();
      auto rend = d.bindings.rend();
      for (; rit != rend; ++rit)
        {
        // This is kind of a hack of the binding system, but it's not too bad here, as Do will be rewritten to core forms
        // early on.
        std::vector<Expression*> bindings;
        auto rit2 = rit->begin();
        auto rend2 = rit->end()-1;
        for (; rit2 != rend2; ++rit2)
          bindings.push_back(&(*rit2));
        expression_stack.push_back(make_visitor_entry(&rit->back(), visitor_entry_type::vet_binding, bindings));
        }
      }
      break;
    }
    case visitor_entry_type::vet_do_post_bindings:
    {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_do_post_test));
      func.VisitDoPostBindings(*entry.expr);
      Do& d = std::get<Do>(*entry.expr);
      auto rit = d.test.rbegin();
      auto rend = d.test.rend();
      for (; rit != rend; ++rit)
        {
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
        }
      break;
    }
    case visitor_entry_type::vet_do_post_test:
    {
      expression_stack.push_back(make_visitor_entry(entry.expr, visitor_entry_type::vet_do_post));
      func.VisitDoPostTest(*entry.expr);
      Do& d = std::get<Do>(*entry.expr);
      auto rit = d.commands.rbegin();
      auto rend = d.commands.rend();
      for (; rit != rend; ++rit)
        {
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
        }
      break;
    }
    case visitor_entry_type::vet_do_post:
    {
      func.PostVisitDo(*entry.expr);
      break;
    }
    default:
    break;
  }
}

template <class TOperationFunctor>
inline void visit(std::vector<visitor_entry>& expression_stack, TOperationFunctor& func)
{
  while (!expression_stack.empty())
    {
    visitor_entry current_expr = expression_stack.back();
    expression_stack.pop_back();
    visit(current_expr, expression_stack, func);
    }
}

template <class TOperationFunctor>
inline void visit(Program& prog, TOperationFunctor& func)
  {
  if (func.PreVisit(prog))
    {
    std::vector<visitor_entry> expression_stack;
    expression_stack.reserve(prog.expressions.size());
    auto rit = prog.expressions.rbegin();
    auto rend = prog.expressions.rend();
    for (; rit != rend; ++rit)
      expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
    visit(expression_stack, func);
    }
  func.PostVisit(prog);
  }


COMPILER_END
