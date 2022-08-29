#include "quote_collector.h"
#include <cassert>
#include <sstream>

#include "visitor.h"

COMPILER_BEGIN

namespace
  {
  struct quote_collector_visitor : public VisitorBaseClass
    {
    std::map<std::string, cell> collected_quotes;

    bool PreVisitQuote(Expression& e)
      {
      Quote& q = std::get<Quote>(e);
      std::stringstream str;
      str << q.arg;
      collected_quotes[str.str()] = q.arg;
      return true;
      }
    };
  }
  
void collect_quotes(Program& prog, repl_data& data)
  {
  assert(!prog.quotes_collected);
  quote_collector_visitor qcv;
  visit(prog, qcv);
  prog.quotes_collected = true;
  prog.quotes.clear();
  for (auto q : qcv.collected_quotes)
    {
    if (data.quote_to_index.find(q.first) == data.quote_to_index.end())
      prog.quotes.insert(q);
    }
  }

COMPILER_END
