#include "global_define_env.h"
#include "visitor.h"
#include "environment.h"
#include "repl_data.h"
#include "context.h"
#include "types.h"

COMPILER_BEGIN
namespace
  {
  
  struct global_define_env_visitor : public VisitorBaseClass
    {
    environment_map* env;
    repl_data* rd;
    context* p_ctxt;

    bool PreVisitSet(Expression& e)
      {
      Set& s = std::get<Set>(e);
      if (s.originates_from_define)
        {
        environment_entry e;
        e.st = environment_entry::st_global;
        e.pos = rd->global_index * 8;
        ++(rd->global_index);
        env->push(s.name, e);
        uint64_t* addr = p_ctxt->globals + (e.pos >> 3);
        *addr = reserved_tag; // This is a new address, previously equal to unalloc_tag. To avoid that gc stops here when cleaning, we change its value to reserved_tag.
        }
      return true;
      }
    };
  }
    
void global_define_environment_allocation(Program& prog, environment_map& env, repl_data& data, context& ctxt)
  {
  assert(prog.alpha_converted);
  global_define_env_visitor gdev;
  gdev.env = &env;
  gdev.rd = &data;
  gdev.p_ctxt = &ctxt;
  visit(prog, gdev);
  prog.global_define_env_allocated = true;
  }

COMPILER_END
