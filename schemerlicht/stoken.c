#include "stoken.h"
#include "slimits.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

schemerlicht_flonum to_flonum(const char* value)
  {
  return cast(schemerlicht_flonum, atof(value));
  }

int is_number(int* is_real, int* is_scientific, const char* value)
  {
  if (value[0] == '\0')
    return 0;
  int i = 0;
  if (value[0] == 'e' || value[0] == 'E')
    return 0;
  if (value[0] == '-' || value[0] == '+')
    {
    ++i;
    if (value[1] == '\0')
      return 0;
    }
  *is_real = 0;
  *is_scientific = 0;
  const char* s = value + i;
  while (*s != '\0')
    {
    if (isdigit((unsigned char)(*s)) == 0)
      {
      if ((*s == '.') && (*is_real == 0) && (*is_scientific == 0))
        *is_real = 1;
      else if ((*s == 'e' || *s == 'E') && (*is_scientific == 0))
        {
        *is_scientific = 1;
        *is_real = 1;
        if (*(s + 1) == '\0')
          return 0;
        if (*(s + 1) == '-' || *(s + 1) == '+')
          {
          ++s;
          }
        if (*(s + 1) == '\0')
          return 0;
        }
      else
        return 0;
      }
    ++s;
    }
  return 1;
  }