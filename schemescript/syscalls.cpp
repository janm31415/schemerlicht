#include "syscalls.h"

#include <stdint.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <fstream>
#include <chrono>
//#include "file_utils.h"
//#include "c_prim_decl.h"

COMPILER_BEGIN

int _skiwi_close(int const fd)
  {
  if (fd < 0)
    return 0;
#ifdef _WIN32
  return _close(fd);
#else
  return close(fd);
#endif
  }

int _skiwi_read(int const fd, void* const buffer, unsigned const buffer_size)
  {
  if (fd < 0)
    return 0;
#ifdef _WIN32
  return _read(fd, buffer, buffer_size);
#else
  return ::read(fd, buffer, buffer_size);
#endif
  }

int _skiwi_write(int fd, const void* buffer, unsigned int count)
  {
  if (fd < 0)
    return 0;
#ifdef _WIN32
  return _write(fd, buffer, count);
#else
  return write(fd, buffer, count);
#endif
  }

const char* _skiwi_getenv(const char* name)
  {
  static std::string env_value;
#ifdef _WIN32
  std::string s(name);
  std::wstring ws = convert_string_to_wstring(s);
  wchar_t* path = _wgetenv(ws.c_str());
  if (!path)
    return nullptr;
  std::wstring wresult(path);
  env_value = convert_wstring_to_string(wresult);
  return env_value.c_str();
#else
  return ::getenv(name);
#endif
  }

int _skiwi_putenv(const char* name, const char* value)
  {
#ifdef _WIN32
  std::string sname(name);
  std::string svalue(value);
  std::wstring wname = convert_string_to_wstring(sname);
  std::wstring wvalue = convert_string_to_wstring(svalue);
  return (int)_wputenv_s(wname.c_str(), wvalue.c_str());
#else
  return setenv(name, value, 1);
#endif
  }

int _skiwi_file_exists(const char* filename)
  {
#ifdef _WIN32
  int res = 0;
  std::string sname(filename);
  std::wstring wname = convert_string_to_wstring(sname);
  std::ifstream f(wname);
  if (f.is_open())
    {
    res = 1;
    f.close();
    }
  return res;
#else
  int res = 0;
  std::ifstream f(filename);
  if (f.is_open())
    {
    res = 1;
    f.close();
    }
  return res;
#endif
  }

uint64_t _skiwi_current_seconds()
  {
  uint64_t secondsUTC = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  return secondsUTC;
  }

uint64_t _skiwi_current_milliseconds()
  {
  uint64_t millisecondsUTC = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  return millisecondsUTC;
  }

uint64_t _skiwi_sprintf_floating(char * a, const char * b, double c)
{
    int value = sprintf(a, b, c);
    return (uint64_t)value;
}

uint64_t _skiwi_sprintf(char * a, const char * b, const char* c)
{
    int value = sprintf(a, b, c);
    return (uint64_t)value;
}

COMPILER_END
