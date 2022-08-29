#include "single_begin_conversion.h"
#include "visitor.h"

COMPILER_BEGIN

namespace
  {
  struct remove_nested_begins : public VisitorBaseClass
    {
    void PostVisitBegin(Expression& e)
      {
      Begin& b = std::get<Begin>(e);
      auto it = b.arguments.begin();
      while (it != b.arguments.end())
        {
        if (std::holds_alternative<Begin>(*it))
          {
          Begin b2 = std::get<Begin>(*it);
          it = b.arguments.erase(it);
          it = b.arguments.insert(it, b2.arguments.begin(), b2.arguments.end());
          }
        else
          ++it;
        }
      }

    void PostVisit(Program& p)
      {
      auto it = p.expressions.begin();
      while (it != p.expressions.end())
        {
        if (std::holds_alternative<Begin>(*it))
          {
          Begin b2 = std::get<Begin>(*it);
          it = p.expressions.erase(it);
          it = p.expressions.insert(it, b2.arguments.begin(), b2.arguments.end());
          }
        else
          ++it;
        }
      }
    };
  } // anonymous namespace

void single_begin_conversion(Program& prog)
  {
  remove_nested_begins rnb;
  visit(prog, rnb);

  if (prog.expressions.size() > 1)
    {
    Begin b;
    b.arguments = prog.expressions;
    prog.expressions.clear();
    prog.expressions.push_back(b);
    }
  assert(prog.expressions.size() <= 1);

  prog.single_begin_conversion = true;
  }

void remove_nested_begin_expressions(Program& p)
  {
  remove_nested_begins rnb;
  visit(p, rnb);
  }
  
void remove_nested_begin_expressions(Expression& e)
  {
  remove_nested_begins rnb;
  visit(e, rnb);
  }

COMPILER_END
