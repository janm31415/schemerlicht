#pragma once

#include "namespace.h"

#include <string>
#include <stdint.h>
#include <vector>

#include "schemescript_api.h"

COMPILER_BEGIN

COMPILER_SCHEMESCRIPT_API double to_double(const char* value);
COMPILER_SCHEMESCRIPT_API bool is_number(bool& is_real, const char* value);

struct token
  {
  enum e_type
    {
    T_BAD,
    T_LEFT_ROUND_BRACKET,
    T_RIGHT_ROUND_BRACKET,
    T_LEFT_SQUARE_BRACKET,
    T_RIGHT_SQUARE_BRACKET,
    T_LEFT_CURLY_BRACE,
    T_RIGHT_CURLY_BRACE,
    T_RELATIVE_LESS,
    T_RELATIVE_LEQ,
    T_RELATIVE_GREATER,
    T_RELATIVE_GEQ,
    T_RELATIVE_EQUAL,
    T_RELATIVE_NOTEQUAL,
    T_INTEGER,
    T_REAL,
    T_ID,
    T_ASSIGNMENT,
    T_ASSIGNMENT_PLUS,
    T_ASSIGNMENT_MINUS,
    T_ASSIGNMENT_MUL,
    T_ASSIGNMENT_DIV,
    T_INCREMENT,
    T_DECREMENT,
    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_SEMICOLON,
    T_COMMA,
    T_AMPERSAND,
    T_PERCENT
    };

  e_type type;
  std::string value;
  int line_nr;

  token(e_type i_type, const std::string& v, int i_line_nr) : type(i_type), value(v), line_nr(i_line_nr) {}
  };

typedef std::vector<token> tokens;
COMPILER_SCHEMESCRIPT_API tokens tokenize(const std::string& str);

COMPILER_END
