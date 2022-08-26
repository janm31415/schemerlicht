#pragma once

#include "namespace.h"
#include "parse.h"
#include <vector>

COMPILER_BEGIN

enum class visitor_entry_type
  {
  vet_expression,
  vet_begin,
  vet_case,
  vet_cond,
  vet_do,
  vet_foreigncall,
  vet_funcall,
  vet_if,
  vet_lambda,
  vet_let,
  vet_literal,
  vet_nop,
  vet_primitivecall,
  vet_quote,
  vet_set,
  vet_variable
  };

struct visitor_entry
  {
  Expression* expr;
  visitor_entry_type type;
  };
  
inline visitor_entry make_visitor_entry(Expression* e, visitor_entry_type t)
{
  visitor_entry entry;
  entry.expr = e;
  entry.type = t;
  return entry;
}

template <class TOperationFunctor>
inline void visit(visitor_entry entry, std::vector<visitor_entry>& expression_stack, TOperationFunctor& func)
{
  switch (entry.type)
  {
    case visitor_entry_type::vet_expression:
    {
      if (func.PreVisit(*entry.expr))
        {
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
        func.PostVisit(*entry.expr);
    break;
    }
    case visitor_entry_type::vet_begin:
    {
    if (func.PreVisitBegin(*entry.expr))
      {
      Begin& b = std::get<Begin>(*entry.expr);
      auto rit = b.arguments.rbegin();
      auto rend = b.arguments.rend();
      for (; rit != rend; ++rit)
        expression_stack.push_back(make_visitor_entry(&(*rit), visitor_entry_type::vet_expression));
      }
    func.PostVisitBegin(*entry.expr);
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
