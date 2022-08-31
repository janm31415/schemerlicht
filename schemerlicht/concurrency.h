#include "compiler_options.h"
#include "namespace.h"
#include <functional>
#include <thread>
#include <cassert>
#include <exception>
#include <cstring>
#include <atomic>
#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#elif defined(unix)
#include <pthread.h>
#elif defined(__APPLE__)
#include <pthread.h>
#endif


COMPILER_BEGIN

inline uint64_t get_thread_id()
  {
#ifdef _WIN32
  return (uint64_t)GetCurrentThreadId();
#elif defined(unix)
  return (uint64_t)pthread_self();
#elif defined(__APPLE__)
  uint64_t tid;
  pthread_threadid_np(NULL, &tid);
  return tid;
#endif
  }


template <class _Type, class TFunctor>
void parallel_for(_Type first, _Type last, TFunctor fun, const compiler_options& ops)
  {
  if (ops.parallel)
    {
    const _Type n_threads = (_Type)std::thread::hardware_concurrency();

    const _Type n = last - first;

    const _Type n_max_tasks_per_thread = (n / n_threads) + (n % n_threads == 0 ? 0 : 1);
    const _Type n_lacking_tasks = n_max_tasks_per_thread * n_threads - n;

    auto inner_loop = [&](const _Type thread_index)
      {
      const _Type n_lacking_tasks_so_far = n_threads > (thread_index + n_lacking_tasks) ? 0 : (thread_index + n_lacking_tasks) - n_threads;
      const _Type inclusive_start_index = thread_index * n_max_tasks_per_thread - n_lacking_tasks_so_far;
      const _Type exclusive_end_index = inclusive_start_index + n_max_tasks_per_thread - (thread_index + n_lacking_tasks >= n_threads ? 1 : 0);

      for (_Type k = inclusive_start_index; k < exclusive_end_index; ++k)
        {
        fun(k + first);
        }
      };
    std::vector<std::thread> threads;
    for (_Type j = 0; j < n_threads; ++j) { threads.push_back(std::thread(inner_loop, j)); }
    for (auto& t : threads) { t.join(); }

    }
  else
    {
    for (_Type i = first; i != last; ++i)
      fun(i);
    }
  }

COMPILER_END
