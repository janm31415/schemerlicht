#pragma once

#include "schemescript_api.h"
#include "liveness_range.h"

#include "namespace.h"
#include "vm/vmcode.h"

#include <map>

COMPILER_BEGIN

struct reg_alloc_data
  {
  enum e_register_type
    {
    t_register,
    t_local
    };

  e_register_type type;
  uint32_t local_id;
  VM::vmcode::operand reg;
  liveness_range live_range;


  bool operator < (const reg_alloc_data& other) const
    {
    if (type == other.type)
      {
      if (type == reg_alloc_data::t_local)
        return local_id < other.local_id;
      else
        return (int)reg < (int)other.reg;
      }
    else
      return (int)type < (int)other.type;
    }
  };

inline reg_alloc_data make_reg_alloc_data(reg_alloc_data::e_register_type t, uint64_t val, liveness_range lr)
  {
  reg_alloc_data rad;
  rad.type = t;
  if (t == reg_alloc_data::t_local)
    rad.local_id = (uint32_t)val;
  else
    rad.reg = (VM::vmcode::operand)val;
  rad.live_range = lr;
  return rad;
  }

typedef std::map<reg_alloc_data, std::string> reg_alloc_map;

COMPILER_END
