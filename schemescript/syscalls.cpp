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

int _scheme_close(int const fd)
  {
  if (fd < 0)
    return 0;
#ifdef _WIN32
  return _close(fd);
#else
  return close(fd);
#endif
  }

int _scheme_read(int const fd, void* const buffer, unsigned const buffer_size)
  {
  if (fd < 0)
    return 0;
#ifdef _WIN32
  return _read(fd, buffer, buffer_size);
#else
  return ::read(fd, buffer, buffer_size);
#endif
  }

int _scheme_write(int fd, const void* buffer, unsigned int count)
  {
  if (fd < 0)
    return 0;
#ifdef _WIN32
  return _write(fd, buffer, count);
#else
  return write(fd, buffer, count);
#endif
  }

const char* _scheme_getenv(const char* name)
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

int _scheme_putenv(const char* name, const char* value)
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

int _scheme_file_exists(const char* filename)
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

uint64_t _scheme_current_seconds()
  {
  uint64_t secondsUTC = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  return secondsUTC;
  }

uint64_t _scheme_current_milliseconds()
  {
  uint64_t millisecondsUTC = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  return millisecondsUTC;
  }

uint64_t _scheme_sprintf_floating(char * a, const char * b, double c)
{
    int value = sprintf(a, b, c);
    return (uint64_t)value;
}

uint64_t _scheme_sprintf(char * a, const char * b, const char* c)
{
    int value = sprintf(a, b, c);
    return (uint64_t)value;
}


void add_system_calls(std::map<std::string, external_function>& externals)
  {
  external_function ef;
  ef.name = "_write";
#ifdef _WIN32
  ef.address = (uint64_t)&_write;
#else
  ef.address = (uint64_t)&write;
#endif
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_INT64);
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  ef.arguments.push_back(external_function::T_INT64);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "_open";
#ifdef _WIN32
  ef.address = (uint64_t)&_open;
#else
  ef.address = (uint64_t)&open;
#endif
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  ef.arguments.push_back(external_function::T_INT64);
  ef.arguments.push_back(external_function::T_INT64);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "strtoll";
  ef.address = (uint64_t)&strtoll;
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  ef.arguments.push_back(external_function::T_INT64);
  ef.arguments.push_back(external_function::T_INT64);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "strtod";
  ef.address = (uint64_t)&strtod;
  ef.return_type = external_function::T_DOUBLE;
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  ef.arguments.push_back(external_function::T_INT64);
  externals[ef.name] = ef;
  
  ef.arguments.clear();
  ef.name = "_scheme_sprintf";
  ef.address = (uint64_t)&_scheme_sprintf;
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  externals[ef.name] = ef;
    
ef.arguments.clear();
ef.name = "_scheme_sprintf_floating";
ef.address = (uint64_t)&_scheme_sprintf_floating;
ef.return_type = external_function::T_INT64;
ef.arguments.push_back(external_function::T_CHAR_POINTER);
ef.arguments.push_back(external_function::T_CHAR_POINTER);
ef.arguments.push_back(external_function::T_DOUBLE);
externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "_scheme_close";
  ef.address = (uint64_t)&_scheme_close;
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_INT64);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "_scheme_read";
  ef.address = (uint64_t)&_scheme_read;
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_INT64);
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  ef.arguments.push_back(external_function::T_INT64);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "_scheme_write";
  ef.address = (uint64_t)&_scheme_write;
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_INT64);
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  ef.arguments.push_back(external_function::T_INT64);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "_scheme_getenv";
  ef.address = (uint64_t)&_scheme_getenv;
  ef.return_type = external_function::T_CHAR_POINTER;
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "_scheme_putenv";
  ef.address = (uint64_t)&_scheme_putenv;
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "_scheme_file_exists";
  ef.address = (uint64_t)&_scheme_file_exists;
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "_scheme_current_seconds";
  ef.address = (uint64_t)&_scheme_current_seconds;
  ef.return_type = external_function::T_INT64;
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "_scheme_current_milliseconds";
  ef.address = (uint64_t)&_scheme_current_milliseconds;
  ef.return_type = external_function::T_INT64;
  externals[ef.name] = ef;
/*
  ef.arguments.clear();
  ef.name = "c_prim_load";
  ef.address = (uint64_t)&c_prim_load;
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  externals[ef.name] = ef;

  ef.arguments.clear();
  ef.name = "c_prim_eval";
  ef.address = (uint64_t)&c_prim_eval;
  ef.return_type = external_function::T_INT64;
  ef.arguments.push_back(external_function::T_CHAR_POINTER);
  externals[ef.name] = ef;
*/
  }

namespace {
VM::external_function::argtype convert(COMPILER::external_function::argtype arg)
  {
  switch (arg)
    {
    case COMPILER::external_function::T_BOOL: return VM::external_function::T_BOOL;
    case COMPILER::external_function::T_CHAR_POINTER: return VM::external_function::T_CHAR_POINTER;
    case COMPILER::external_function::T_DOUBLE: return VM::external_function::T_DOUBLE;
    case COMPILER::external_function::T_INT64: return VM::external_function::T_INT64;
    case COMPILER::external_function::T_VOID: return VM::external_function::T_VOID;
    case COMPILER::external_function::T_SCM: return VM::external_function::T_INT64;
    default: return VM::external_function::T_VOID;
    }
  }
}

VM::external_function convert_external_to_vm(const external_function& ext)
{
    VM::external_function ef;
    ef.name = ext.name;
    ef.address = ext.address;
    ef.return_type = convert(ext.return_type);
    for (auto arg : ext.arguments)
      {
      ef.arguments.push_back(convert(arg));
      }
    return ef;
}
std::vector<VM::external_function> convert_externals_to_vm(std::map<std::string, external_function>& externals)
  {
  std::vector<VM::external_function> out;
  for (const auto& e : externals)
    {
    out.push_back(convert_external_to_vm(e.second));
    }
    return out;
  }

COMPILER_END
