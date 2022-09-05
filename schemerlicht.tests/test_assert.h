#ifndef TEST_ASSERT_H
#define TEST_ASSERT_H

#include <stdio.h>

extern int testing_fails;
extern int testing_success;

#define TEST_OUTPUT_LINE(...) \
      { printf(__VA_ARGS__); printf("\n"); }

void TestFail(const char* expval, const char* val, const char* file, int line, const char* func);
void InitTestEngine();
int CloseTestEngine(int force_report);

#define TestEqInt(expval, val, file, line, func) \
  if ((int)(expval) != (int)(val)) \
    { \
    char expval_str[256]; \
    char val_str[256]; \
    sprintf(expval_str, "%d", (int)(expval)); \
    sprintf(val_str, "%d", (int)(val)); \
    TestFail(expval_str, val_str, file, line, func); \
    } \
  else \
    { \
    ++testing_success; \
    }

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

#ifdef _WIN32
#define TEST_EQ_INT(exp, val) TestEqInt(exp, val, __FILE__, __LINE__, __FUNCTION__)
#define TEST_EQ_DOUBLE(exp, val) TestEqDouble(exp, val, __FILE__, __LINE__, __FUNCTION__)
#else
#define TEST_EQ_INT(exp, val) TestEqInt(exp, val, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define TEST_EQ_DOUBLE(exp, val) TestEqDouble(exp, val, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif
#endif // TEST_ASSERT_H