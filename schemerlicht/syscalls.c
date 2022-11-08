#include "syscalls.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

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