#include "syscalls.h"

#ifdef _WIN32
#include <io.h>
#include <sys\stat.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdlib.h>

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
  return _open(filename, _O_CREAT | O_WRONLY | O_TRUNC | O_BINARY, _S_IREAD | _S_IWRITE);
#elif defined(__APPLE__)
  return open(filename, O_CREAT | O_WRONLY | O_TRUNC, __S_IREAD | __S_IWRITE);
#else // unix
  return open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IREAD | S_IWRITE);
#endif
  }

int schemerlicht_open_input_file(const char* filename)
  {
#ifdef _WIN32
  return _open(filename, _O_RDONLY | O_BINARY, _S_IREAD | _S_IWRITE);
#elif defined(__APPLE__)
  return open(filename, O_RDONLY, __S_IREAD | __S_IWRITE);
#else // unix
  return open(filename, O_RDONLY, S_IREAD | S_IWRITE);
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
  return tell(fd);
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