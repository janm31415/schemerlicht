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
      bool PreVisit(Expression&) {return true;}
      void PostVisit(Expression&) {}
      bool PreVisitBegin(Expression&) { str << "( begin "; return true; }
      void VisitFixnum(Expression& e) { str << std::get<Fixnum>(std::get<Literal>(e)).value << " "; return true; }
      void VisitFlonum(Expression& e) { str << std::get<Flonum>(std::get<Literal>(e)).value << " "; return true; }
      void VisitNil(Expression&) { str << "() " << " "; return true; }
      void VisitString(Expression& e) { str << "\"" << std::get<String>(std::get<Literal>(e)).value << "\" "; return true; }
      void VisitSymbol(Expression& e) { str << std::get<Symbol>(std::get<Literal>(e)).value << " "; return true; }
      void VisitTrue(Expression&) { str << "#t "; return true; }
      void VisitFalse(Expression&) { str << "#f "; return true; }
      void VisitNop(Expression&) { str << "#undefined "; return true; }
      void VisitCharacter(Expression& e)
        {
        unsigned char ch = (unsigned char)std::get<Character>(std::get<Literal>(e)).value;
        str << "#\\" << int(ch) << " ";
        return true;
        }
      /*
      bool PreVisit(Fixnum& f) { str << f.value << " "; return true; }
      bool PreVisit(Flonum& f) { str << f.value << " "; return true; }
      bool PreVisit(Nil&) { str << "() " << " "; return true; }
      bool PreVisit(String& s) { str << "\"" << s.value << "\" "; return true; }
      bool PreVisit(Symbol& s) { str << s.value << " "; return true; }
      bool PreVisit(True&) { str << "#t "; return true; }
      bool PreVisit(False&) { str << "#f "; return true; }
      bool PreVisit(Nop&) { str << "#undefined "; return true; }
      bool PreVisit(Character& cha)
        {
        unsigned char ch = (unsigned char)cha.value;
        str << "#\\" << int(ch) << " ";
        return true;
        }
      bool PreVisit(Variable& v) { str << v.name << " "; return true; }
      bool PreVisit(FunCall& f)
        {
        str << "( ";
        visitor<Expression, dump_visitor>::visit(f.fun.front(), this);
        for (auto& arg : f.arguments)
          visitor<Expression, dump_visitor>::visit(arg, this);
        return false;
        }
      bool PreVisit(If&) { str << "( if ";  return true; }
      bool PreVisit(Lambda& l)
        {
        str << "( lambda ( ";
        for (const auto& v : l.variables)
          str << v << " ";
        str << ") ";
        return true;
        }
      bool PreVisit(Case& c)
        {
        str << "( case ";
        visitor<Expression, dump_visitor>::visit(c.val_expr.front(), this);
        assert(c.datum_args.size() == c.then_bodies.size());
        for (size_t i = 0; i < c.datum_args.size(); ++i)
          {
          str << sbl << " " << c.datum_args[i] << " ";
          for (auto& arg : c.then_bodies[i])
            visitor<Expression, dump_visitor>::visit(arg, this);
          str << sbr << " ";
          }
        str << sbl << " else ";
        for (auto& arg : c.else_body)
          visitor<Expression, dump_visitor>::visit(arg, this);
        str << sbr << " ) ";
        return false;
        }
      bool PreVisit(Cond& c)
        {
        str << "( cond ";
        for (auto& arg_v : c.arguments)
          {
          str << sbl << " ";
          for (auto& arg : arg_v)
            visitor<Expression, dump_visitor>::visit(arg, this);
          str << sbr << " ";
          }
        str << ") ";
        return false;
        }
      bool PreVisit(Do& d)
        {
        str << "( do ";
        for (auto& b : d.bindings)
          {
          str << sbl << " ";
          for (auto& arg : b)
            visitor<Expression, dump_visitor>::visit(arg, this);
          str << sbr << " ";
          }
        str << ") ( ";
        for (auto& tst : d.test)
          visitor<Expression, dump_visitor>::visit(tst, this);
        str << ") ";
        for (auto& c : d.commands)
          visitor<Expression, dump_visitor>::visit(c, this);
        str << ") ";
        return false;
        }
      bool PreVisit(Let& l)
        {
        switch (l.bt)
          {
          case bt_let: str << "( let ( "; break;
          case bt_let_star: str << "( let* ( "; break;
          case bt_letrec: str << "( letrec ( "; break;
          }
        for (auto& arg : l.bindings)
          {
          str << sbl << " " << arg.first << " ";
          visitor<Expression, dump_visitor>::visit(arg.second, this);
          str << sbr << " ";
          }
        str << ") ";
        visitor<Expression, dump_visitor>::visit(l.body.front(), this);
        return false;
        }
      bool PreVisit(Quote& q)
        {
        switch (q.type)
          {
          case Quote::qt_quote: str << "( quote "; break;
          case Quote::qt_backquote: str << "( quasiquote "; break;
          case Quote::qt_unquote: str << "( unquote "; break;
          case Quote::qt_unquote_splicing: str << "( unquote-splicing "; break;
          }
        str << q.arg << " ) ";
        return true;
        }
      bool PreVisit(PrimitiveCall& p)
        {
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
      bool PreVisit(ForeignCall& p)
        {
        str << "( foreign-call ";
        str << p.foreign_name;
        str << " ";
        return true;
        }
      bool PreVisit(Set& s)
        {
        str << "( set! " << s.name << " ";
        return true;
        }
      */
      void PostVisitBegin(Expression&) { str << ") "; }
      void PostVisitFunCall(Expression&) { str << ") "; }
      void PostVisitIf(Expression&) { str << ") "; }
      void PostVisitLambda(Expression&) { str << ") "; }
      void PostVisitLet(Expression&) { str << ") "; }
      void PostVisitForeignCall(Expression&) { str << ") "; }
      void PostVisitPrimitiveCall(Expression& e) { if (!std::get<PrimitiveCall>(e).as_object) str << ") "; }
      void PostVisitSet(Expression&) { str << ") "; }
      
    };

  }

void dump(std::ostream& out, Program& prog, bool use_square_brackets)
{
  DumpOperationFunctor func;
  visit(prog, func);
  out << func.str.str();
}

void dump(std::ostream& out, Expression& expr, bool use_square_brackets)
{
  DumpOperationFunctor func;
  std::vector<visitor_entry> expression_stack;
  expression_stack.push_back(make_visitor_entry(&expr, visitor_entry_type::vet_expression));
  visit(expression_stack, func);
  out << func.str.str();
}

COMPILER_END
