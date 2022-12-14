#include "test_assert.h"

#include <assert.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#include <windows.h>
#else
#include <string.h>
#endif

#if defined(MEMORY_LEAK_TRACKING) && defined(_MSC_VER)
#ifndef NDEBUG
#define MEMORY_LEAK_TRACKING_MSVC
#endif
#endif  

int testing_fails = 0;
int testing_success = 0;

void TestFail(const char* expval, const char* val, const char* file, int line, const char* func)
  {
  TEST_OUTPUT_LINE("VALUE:\n%s", val);
  TEST_OUTPUT_LINE("EXPECTED:\n%s", expval);
  if (func)
    {
    TEST_OUTPUT_LINE("%s(%d): error: Failure in %s: expected %s but was %s", file, line, func, expval, val);
    }
  else
    {
    TEST_OUTPUT_LINE("%s(%d): error: expected %s but was %s", file, line, expval, val);
    }
  ++testing_fails;
  }

void InitTestEngine()
  {
  testing_fails = 0;
  
#ifdef _MSC_VER
  // Disable stdout buffering to prevent information lost on assertion or core
  // dump.
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  // By default, send all reports to STDOUT to prevent CI hangs.
  // Enable assert report box [Abort|Retry|Ignore] if a debugger is present.
  const int dbg_mode = (_CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG) |
    (IsDebuggerPresent() ? _CRTDBG_MODE_WNDW : 0);
  (void)dbg_mode; // release mode fix
  // CrtDebug reports to _CRT_WARN channel.
  _CrtSetReportMode(_CRT_WARN, dbg_mode);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
  // The assert from <assert.h> reports to _CRT_ERROR channel
  _CrtSetReportMode(_CRT_ERROR, dbg_mode);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
  // Internal CRT assert channel?
  _CrtSetReportMode(_CRT_ASSERT, dbg_mode);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
#endif

#if defined(MEMORY_LEAK_TRACKING_MSVC)
  // For more thorough checking:
  // _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  _CrtSetDbgFlag(flags | _CRTDBG_ALLOC_MEM_DF);
#endif
  }

int CloseTestEngine(int force_report)
  {
  if (!testing_fails || force_report)
    {

#if defined(MEMORY_LEAK_TRACKING_MSVC)
    int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flags &= ~_CRTDBG_DELAY_FREE_MEM_DF;
    flags |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(flags);
#endif

    }

  return (0 != testing_fails);
  }
