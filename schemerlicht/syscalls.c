#include "syscalls.h"

#ifdef _WIN32
#include <io.h>
#include <sys\stat.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int schemerlicht_write(int fd, const void* buffer, unsigned int count)
  {
  if (fd < 0)
    return 0;
#ifdef _WIN32
  return _write(fd, buffer, count);
#else
  return write(fd, buffer, count);
#endif
  }

int schemerlicht_read(int fd, void* buffer, unsigned int buffer_size)
  {
  if (fd < 0)
    return 0;
#ifdef _WIN32
  return _read(fd, buffer, buffer_size);
#else
  return read(fd, buffer, buffer_size);
#endif
  }

int schemerlicht_open_output_file(const char* filename)
  {
#ifdef _WIN32
  return _open(filename, _O_CREAT | O_WRONLY | O_TRUNC, _S_IREAD | _S_IWRITE);
#elif defined(__APPLE__)
  return open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IREAD | S_IWRITE);
#else // unix
  return open(filename, O_CREAT | O_WRONLY | O_TRUNC, __S_IREAD | __S_IWRITE);
#endif
  }

int schemerlicht_open_input_file(const char* filename)
  {
#ifdef _WIN32
  return _open(filename, _O_RDONLY, _S_IREAD | _S_IWRITE);
#elif defined(__APPLE__)
  return open(filename, O_RDONLY, S_IREAD | S_IWRITE);
#else // unix
  return open(filename, O_RDONLY, __S_IREAD | __S_IWRITE);
#endif
  }

int schemerlicht_close_file(int fd)
  {
  if (fd < 0)
    return 0;
#ifdef _WIN32
  return _close(fd);
#else
  return close(fd);
#endif
  }

long schemerlicht_tell(int fd)
  {
#ifdef _WIN32
  return _tell(fd);
#else
  return lseek(fd, 0, SEEK_CUR);
#endif
  }

const char* schemerlicht_getenv(const char* name)
  {
  return getenv(name);
  }

int schemerlicht_putenv(const char* name, const char* value)
  {
#ifdef _WIN32
  return (int)_putenv_s(name, value);
#else
  return setenv(name, value, 1);
#endif
  }

int schemerlicht_file_exists(const char* filename)
  {
  int res = 0;
  FILE* f = fopen(filename, "r");
  if (f)
    {
    res = 1;
    fclose(f);
    }
  return res;
  }

void schemerlicht_fixnum_to_char(char* buffer, schemerlicht_fixnum fx)
  {
  sprintf(buffer, "%lld", fx);
  }

void schemerlicht_fixnum_to_binary_char(char* str, schemerlicht_fixnum a)
  {
  if (a == 0)
    {
    str[0] = '0';
    str[1] = 0;
    }
  else
    {
    int len = cast(int, ceil(log2(cast(double, a))));
    char* s = str + len;
    *s-- = 0;
    while (a > 0)
      {
      *s-- = a % 2 + '0';
      a /= 2;
      }
    }
  }

void schemerlicht_fixnum_to_hex_char(char* buffer, schemerlicht_fixnum fx)
  {
  sprintf(buffer, "%llx", fx);
  }

void schemerlicht_fixnum_to_oct_char(char* buffer, schemerlicht_fixnum fx)
  {
  sprintf(buffer, "%llo", fx);
  }

void schemerlicht_flonum_to_char(char* buffer, schemerlicht_flonum fl)
  {
  sprintf(buffer, "%f", fl);
  }

void schemerlicht_flonum_to_char_scientific(char* buffer, schemerlicht_flonum fl)
  {
  sprintf(buffer, "%.17e", fl);
  }

void schemerlicht_memsize_to_char(char* buffer, schemerlicht_memsize s)
  {
  sprintf(buffer, "%d", s);
  }

void schemerlicht_int_to_char(char* buffer, int i)
  {
  sprintf(buffer, "%d", i);
  }