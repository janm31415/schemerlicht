#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

#include <stdio.h>
#include <string.h>

extern int testing_fails;
extern int testing_success;

#define TEST_OUTPUT_LINE(...) \
      { printf(__VA_ARGS__); printf("\n"); }

void TestFail(const char* expval, const char* val, const char* file, int line, const char* func);
void InitTestEngine();
int CloseTestEngine(int force_report);

#if defined(_WIN32) || defined(__APPLE__)
#define TestEqInt(expval, val, file, line, func) \
  if ((int64_t)(expval) != (int64_t)(val)) \
    { \
    char expval_str[256]; \
    char val_str[256]; \
    sprintf(expval_str, "%lld", (int64_t)(expval)); \
    sprintf(val_str, "%lld", (int64_t)(val)); \
    TestFail(expval_str, val_str, file, line, func); \
    } \
  else \
    { \
    ++testing_success; \
    }
#else
#define TestEqInt(expval, val, file, line, func) \
  if ((int64_t)(expval) != (int64_t)(val)) \
    { \
    char expval_str[256]; \
    char val_str[256]; \
    sprintf(expval_str, "%ld", (int64_t)(expval)); \
    sprintf(val_str, "%ld", (int64_t)(val)); \
    TestFail(expval_str, val_str, file, line, func); \
    } \
  else \
    { \
    ++testing_success; \
    }
#endif
#define TestEqDouble(expval, val, file, line, func) \
  if ((double)(expval) != (double)(val)) \
    { \
    char expval_str[256]; \
    char val_str[256]; \
    sprintf(expval_str, "%f", (double)(expval)); \
    sprintf(val_str, "%f", (double)(val)); \
    TestFail(expval_str, val_str, file, line, func); \
    } \
  else \
    { \
    ++testing_success; \
    }

#define TestEqString(expval, val, file, line, func) \
  if (strcmp(expval, val) != 0) \
    { \
    TestFail(expval, val, file, line, func); \
    } \
  else \
    { \
    ++testing_success; \
    }

#ifdef _WIN32
#define TEST_EQ_INT(exp, val) TestEqInt(exp, val, __FILE__, __LINE__, __FUNCTION__)
#define TEST_EQ_DOUBLE(exp, val) TestEqDouble(exp, val, __FILE__, __LINE__, __FUNCTION__)
#define TEST_EQ_STRING(exp, val) TestEqString(exp, val, __FILE__, __LINE__, __FUNCTION__)
#else
#define TEST_EQ_INT(exp, val) TestEqInt(exp, val, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define TEST_EQ_DOUBLE(exp, val) TestEqDouble(exp, val, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define TEST_EQ_STRING(exp, val) TestEqString(exp, val, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif
#endif // TEST_ASSERT_H
