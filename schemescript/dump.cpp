#include "dump.h"
#include "visitor.h"
#include <sstream>
#include <iomanip>

COMPILER_BEGIN

namespace
  {
  
  class DumpOperationFunctor
    {
    public:
      std::stringstream str;
      char sbl, sbr;
    
      DumpOperationFunctor() : sbl('['), sbr(']') {}
      
      bool PreVisit(Program&) { return true; }
      void PostVisit(Program&) {}
      bool PreVisitExpression(Expression&) { return true; }
      bool PreVisitBinding(Expression&, cell& binding) // cell binding can be invalid
        {
        str << sbl << " ";
        if (binding.type != ct_invalid_cell)
          str << binding << " ";
        return true;
        }
      void PostVisitBinding(Expression&)
        {
        str << sbr << " ";
        }
      void PostVisitExpression(Expression&) {}
      bool PreVisitBegin(Expression&) { str << "( begin "; return true; }
      void VisitFixnum(Expression& e) { str << std::get<Fixnum>(std::get<Literal>(e)).value << " "; }
      void VisitFlonum(Expression& e) { str << std::get<Flonum>(std::get<Literal>(e)).value << " "; }
      void VisitNil(Expression&) { str << "() " << " "; }
      void VisitString(Expression& e) { str << "\"" << std::get<String>(std::get<Literal>(e)).value << "\" "; }
      void VisitSymbol(Expression& e) { str << std::get<Symbol>(std::get<Literal>(e)).value << " "; }
      void VisitTrue(Expression&) { str << "#t "; }
      void VisitFalse(Expression&) { str << "#f "; }
      void VisitNop(Expression&) { str << "#undefined "; }
      void VisitCharacter(Expression& e)
        {
        unsigned char ch = (unsigned char)std::get<Character>(std::get<Literal>(e)).value;
        str << "#\\" << int(ch) << " ";
        return true;
        }
      bool PreVisitPrimitiveCall(Expression& e)
        {
        PrimitiveCall& p = std::get<PrimitiveCall>(e);
        if (p.as_object)
          {
          str << p.primitive_name << " ";
          return false;
          }
        str << "( ";
        str << p.primitive_name;
        str << " ";
        return true;
        }
      void VisitVariable(Expression& e) { str << std::get<Variable>(e).name << " "; }
      bool PreVisitIf(Expression&) { str << "( if ";  return true; }
      void VisitQuote(Expression& e)
        {
        Quote& q = std::get<Quote>(e);
        switch (q.type)
          {
          case Quote::qt_quote: str << "( quote "; break;
          case Quote::qt_backquote: str << "( quasiquote "; break;
          case Quote::qt_unquote: str << "( unquote "; break;
          case Quote::qt_unquote_splicing: str << "( unquote-splicing "; break;
          }
        str << q.arg << " ) ";
        }
      bool PreVisitSet(Expression& e)
        {
        str << "( set! " << std::get<Set>(e).name << " ";
        return true;
        }
      bool PreVisitLambda(Expression& e)
        {
        Lambda& l = std::get<Lambda>(e);
        str << "( lambda ( ";
        for (const auto& v : l.variables)
          str << v << " ";
        str << ") ";
        return true;
        }
      bool PreVisitLet(Expression& e)
        {
        Let& l = std::get<Let>(e);
        switch (l.bt)
          {
          case bt_let: str << "( let ( "; break;
          case bt_let_star: str << "( let* ( "; break;
          case bt_letrec: str << "( letrec ( "; break;
          }
        return true;
        }
      void VisitLetPostBindings(Expression& e)
        {
        str << " ) ";
        }
      bool PreVisitFunCall(Expression&)
        {
        str << "( ";
        return true;
        }
      bool PreVisitCase(Expression& e)
        {
        str << "( case ";
        return true;
        }
      void VisitCaseElse(Expression& e)
        {
        str << sbl << " else ";
        }
      bool PreVisitCond(Expression& e)
        {
        str << "( cond ";
        return true;
        }
      bool PreVisitDo(Expression& e)
        {
        str << "( do ( ";
        return true;
        }
      void VisitDoPostBindings(Expression& e)
        {
        str << ") ( ";
        }
      void VisitDoPostTest(Expression& e)
        {
        str << ") ( ";
        }
      void PostVisitBegin(Expression&) { str << ") "; }
      void PostVisitFunCall(Expression&) { str << ") "; }
      void PostVisitIf(Expression&) { str << ") "; }
      void PostVisitLambda(Expression&) { str << ") "; }
      void PostVisitLet(Expression&) { str << ") "; }
      void PostVisitForeignCall(Expression&) { str << ") "; }
      void PostVisitPrimitiveCall(Expression& e) { if (!std::get<PrimitiveCall>(e).as_object) str << ") "; }
      void PostVisitSet(Expression&) { str << ") "; }
      void PostVisitCase(Expression& e)
        {
          if (!std::get<Case>(e).else_body.empty())
            str << sbr << " ";
          str << ") ";
        }
      void PostVisitCond(Expression&) { str << ") "; }
      void PostVisitDo(Expression&) { str << ") ) "; }
    };

  }

void dump(std::ostream& out, Program& prog, bool use_square_brackets)
{
  DumpOperationFunctor func;
  if (!use_square_brackets)
    {
    func.sbl = '(';
    func.sbr = ')';
    }
  visit(prog, func);
  out << func.str.str();
}

void dump(std::ostream& out, Expression& expr, bool use_square_brackets)
{
  DumpOperationFunctor func;
  if (!use_square_brackets)
    {
    func.sbl = '(';
    func.sbr = ')';
    }
  std::vector<visitor_entry> expression_stack;
  expression_stack.push_back(make_visitor_entry(&expr, visitor_entry_type::vet_expression));
  visit(expression_stack, func);
  out << func.str.str();
}

COMPILER_END
