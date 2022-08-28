#include "preprocess.h"
#include "simplify_to_core.h"

#include <chrono>

COMPILER_BEGIN

namespace
  {
  struct timer
    {
    void start()
      {
      tic = std::chrono::high_resolution_clock::now();
      }

    double time_elapsed()
      {
      toc = std::chrono::high_resolution_clock::now();
      diff = toc - tic;
      return (double)diff.count();
      }

    std::chrono::high_resolution_clock::time_point tic, toc;
    std::chrono::duration<double> diff;
    };
    
  timer g_timer;

  void tic()
    {
    g_timer.start();
    }

  void debug_string(const char* txt)
    {
    (void)txt;
    //printf(txt); printf("\n");
    }

  void toc()
    {
    auto t = g_timer.time_elapsed();
    (void)t;
    //printf("Time spent: %fs\n", t);
    }
  }
void preprocess(environment_map& env, repl_data& data, macro_data& md, context& ctxt, cinput_data& cinput, Program& prog, const primitive_map& pm, const compiler_options& options)
  {
  
  tic();
  debug_string("start simplify_to_core_forms");
  if (options.do_simplify_to_core_forms)
    simplify_to_core_forms(prog);
  debug_string("done simplify_to_core_forms");
  toc();
  }

COMPILER_END
