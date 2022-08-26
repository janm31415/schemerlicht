#pragma once

#include <string>
#include <memory>
#include <vector>
#include <array>

#include "namespace.h"
#include "schemescript_api.h"
#include "tokenize.h"

#include <cassert>

COMPILER_BEGIN

enum cell_type
  {
  ct_symbol,
  ct_fixnum,
  ct_flonum,
  ct_string,
  ct_pair,
  ct_vector
  };

struct cell
  {
  cell_type type;
  std::string value;
  std::vector<cell> pair;
  std::vector<cell> vec;
  
  COMPILER_SCHEMESCRIPT_API cell();
  COMPILER_SCHEMESCRIPT_API cell(cell_type t);
  COMPILER_SCHEMESCRIPT_API cell(cell_type t, const std::string& val);
  
  };

COMPILER_SCHEMESCRIPT_API bool operator == (const cell& left, const cell& right);
COMPILER_SCHEMESCRIPT_API bool operator != (const cell& left, const cell& right);
COMPILER_SCHEMESCRIPT_API std::ostream& operator << (std::ostream& os, const cell& c);

const cell false_sym(ct_symbol, "#f");
const cell true_sym(ct_symbol, "#t");
const cell nil_sym(ct_pair, "");


COMPILER_SCHEMESCRIPT_API cell read_from(std::vector<token>& tokens, bool quasiquote = false);
COMPILER_SCHEMESCRIPT_API cell read(const std::string& s);


COMPILER_END