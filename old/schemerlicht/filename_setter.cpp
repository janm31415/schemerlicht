#include "filename_setter.h"
#include <cassert>
#include <sstream>
#include <fstream>

#include "visitor.h"

COMPILER_BEGIN


namespace
  {
  struct filename_setter_visitor : public VisitorBaseClass
    {
    std::string filename;
    
    void VisitFixnum(Expression& ob) { std::get<Fixnum>(std::get<Literal>(ob)).filename = filename; }
    void VisitFlonum(Expression& ob) { std::get<Flonum>(std::get<Literal>(ob)).filename = filename; }
    void VisitNil(Expression& ob) { std::get<Nil>(std::get<Literal>(ob)).filename = filename; }
    void VisitString(Expression& ob) { std::get<String>(std::get<Literal>(ob)).filename = filename; }
    void VisitSymbol(Expression& ob) { std::get<Symbol>(std::get<Literal>(ob)).filename = filename; }
    void VisitTrue(Expression& ob) { std::get<True>(std::get<Literal>(ob)).filename = filename; }
    void VisitFalse(Expression& ob) { std::get<False>(std::get<Literal>(ob)).filename = filename; }
    void VisitCharacter(Expression& ob) { std::get<Character>(std::get<Literal>(ob)).filename = filename; }
    void VisitNop(Expression& ob) { std::get<Nop>(ob).filename = filename; }
    void VisitVariable(Expression& ob) { std::get<Variable>(ob).filename = filename; }
    void PostVisitBegin(Expression& ob) { std::get<Begin>(ob).filename = filename; }
    void PostVisitFunCall(Expression& ob) { std::get<FunCall>(ob).filename = filename; }
    void PostVisitIf(Expression& ob) { std::get<If>(ob).filename = filename; }
    void PostVisitCase(Expression& ob) { std::get<Case>(ob).filename = filename; }
    void PostVisitCond(Expression& ob) { std::get<Cond>(ob).filename = filename; }
    void PostVisitDo(Expression& ob) { std::get<Do>(ob).filename = filename; }
    void PostVisitLambda(Expression& ob) { std::get<Lambda>(ob).filename = filename; }
    void PostVisitLet(Expression& ob) { std::get<Let>(ob).filename = filename; }
    void PostVisitPrimitiveCall(Expression& ob) { std::get<PrimitiveCall>(ob).filename = filename; }
    void PostVisitForeignCall(Expression& ob) { std::get<ForeignCall>(ob).filename = filename; }
    void PostVisitSet(Expression& ob) { std::get<Set>(ob).filename = filename; }
    void VisitQuote(Expression& ob) { std::get<Quote>(ob).filename = filename; }
    };
  }
  
void set_filename(Program& prog, const std::string& filename)
  {
  filename_setter_visitor fsv;
  fsv.filename = filename;
  visit(prog, fsv);
  }

COMPILER_END
