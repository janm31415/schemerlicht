#include "tokenize.h"

#include <sstream>

COMPILER_BEGIN

double to_double(const char* value)
  {
  return atof(value);
  }

bool is_number(bool& is_real, const char* value)
  {
  if (value[0] == '\0')
    return false;
  int i = 0;
  if (value[0] == '-')
    {
    ++i;
    if (value[1] == '\0')
      return false;
    }
  is_real = false;
  const char* s = value + i;
  while (*s != '\0')
    {
    if (isdigit((unsigned char)(*s)) == 0)
      {
      if ((*s == '.') && (!is_real))
        is_real = true;
      else
        return is_real && (*s == 'f') && (*(++s) == '\0');
      }
    ++s;
    }
  return true;
  }

namespace
  {


  void _treat_buffer(std::string& buff, std::vector<token>& tokens, int line_nr)
    {
    if (!buff.empty())
      {
      bool is_real;
      if (is_number(is_real, buff.c_str()))
        {
        if (is_real)
          tokens.emplace_back(token::T_REAL, buff, line_nr);
        else
          tokens.emplace_back(token::T_INTEGER, buff, line_nr);
        }
      else
        {
        tokens.emplace_back(token::T_ID, buff, line_nr);
        }
      buff.clear();
      }
    }

  }

tokens tokenize(const std::string& str)
  {
  tokens tokens;
  std::string buff;

  const char* s = str.c_str();
  const char* s_end = str.c_str() + str.length();

  int line_nr = 1;

  while (s < s_end)
    {
    if (*s == ' ' || *s == '\n')
      {
      _treat_buffer(buff, tokens, line_nr);
      while (*s == ' ' || *s == '\n')
        {
        if (*s == '\n')
          {
          ++line_nr;
          }
        ++s;
        }
      }

    const char* s_copy = s;
    switch (*s)
      {
      case '(':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_LEFT_ROUND_BRACKET, "(", line_nr);
      ++s;
      break;
      }
      case ')':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_RIGHT_ROUND_BRACKET, ")", line_nr);
      ++s;
      break;
      }
      case '[':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_LEFT_SQUARE_BRACKET, "[", line_nr);
      ++s;
      break;
      }
      case ']':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_RIGHT_SQUARE_BRACKET, "]", line_nr);
      ++s;
      break;
      }
      case '{':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_LEFT_CURLY_BRACE, "{", line_nr);
      ++s;
      break;
      }
      case '}':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_RIGHT_CURLY_BRACE, "}", line_nr);
      ++s;
      break;
      }
      case '%':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_PERCENT, "%", line_nr);
      ++s;
      break;
      }
      case '+':
      {
      _treat_buffer(buff, tokens, line_nr);
      const char* t = s; ++t;
      if (*t && *t == '=')
        {
        s = t;
        tokens.emplace_back(token::T_ASSIGNMENT_PLUS, "+=", line_nr);
        }
      else if (*t && *t == '+')
        {
        s = t;
        tokens.emplace_back(token::T_INCREMENT, "++", line_nr);
        }
      else
        tokens.emplace_back(token::T_PLUS, "+", line_nr);
      ++s;
      break;
      }
      case '-':
      {
      _treat_buffer(buff, tokens, line_nr);
      const char* t = s; ++t;
      if (*t && *t == '=')
        {
        s = t;
        tokens.emplace_back(token::T_ASSIGNMENT_MINUS, "-=", line_nr);
        }
      else if (*t && *t == '-')
        {
        s = t;
        tokens.emplace_back(token::T_DECREMENT, "--", line_nr);
        }
      else
        tokens.emplace_back(token::T_MINUS, "-", line_nr);
      ++s;
      break;
      }      
      case '*':
      {
      _treat_buffer(buff, tokens, line_nr);
      const char* t = s; ++t;
      if (*t && *t == '=')
        {
        s = t;
        tokens.emplace_back(token::T_ASSIGNMENT_MUL, "*=", line_nr);
        }
      else
        tokens.emplace_back(token::T_MUL, "*", line_nr);
      ++s;
      break;
      }
      case '/':
      {
      _treat_buffer(buff, tokens, line_nr);
      const char* t = s; ++t;
      if (*t && *t == '=')
        {
        s = t;
        tokens.emplace_back(token::T_ASSIGNMENT_DIV, "/=", line_nr);
        }
      else if (*t && *t == '/')
        {
        s = t;
        while (*s && *s != '\n') // comment, so skip till end of the line
          ++s;
        }
      else if (*t && *t == '*')
        {
        s = t;
        ++s;
        bool end_of_comment_found = false;
        while (!end_of_comment_found)
          {
          while (*s && *s != '*')
            ++s;
          if (!(*s) || (*(++s) == '/'))
            end_of_comment_found = true;
          }
        }
      else
        tokens.emplace_back(token::T_DIV, "/", line_nr);
      ++s;
      break;
      }
      case '>':
      {
      const char* t = s; ++t;
      if (*t && *t == '=')
        {
        _treat_buffer(buff, tokens, line_nr);
        tokens.emplace_back(token::T_RELATIVE_GEQ, ">=", line_nr);
        ++s; ++s;
        }
      else
        {
        _treat_buffer(buff, tokens, line_nr);
        tokens.emplace_back(token::T_RELATIVE_GREATER, ">", line_nr);
        ++s;
        }
      break;
      }
      case '<':
      {
      const char* t = s; ++t;
      if (*t && *t == '=')
        {
        _treat_buffer(buff, tokens, line_nr);
        tokens.emplace_back(token::T_RELATIVE_LEQ, "<=", line_nr);
        ++s; ++s;
        }
      else
        {
        _treat_buffer(buff, tokens, line_nr);
        tokens.emplace_back(token::T_RELATIVE_LESS, "<", line_nr);
        ++s;
        }
      break;
      }
      case '!':
      {
      const char* t = s; ++t;
      if (*t && *t == '=')
        {
        _treat_buffer(buff, tokens, line_nr);
        tokens.emplace_back(token::T_RELATIVE_NOTEQUAL, "!=", line_nr);
        ++s; ++s;
        }
      break;
      }
      case '=':
      {
      const char* t = s; ++t;
      if (*t && *t == '=')
        {
        _treat_buffer(buff, tokens, line_nr);
        tokens.emplace_back(token::T_RELATIVE_EQUAL, "==", line_nr);
        ++s; ++s;
        }
      else
        {
        _treat_buffer(buff, tokens, line_nr);
        tokens.emplace_back(token::T_ASSIGNMENT, "=", line_nr);
        ++s;
        }
      break;
      }
      case ';':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_SEMICOLON, ";", line_nr);
      ++s;
      break;
      }
      case ',':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_COMMA, ",", line_nr);
      ++s;
      break;
      }
      case '&':
      {
      _treat_buffer(buff, tokens, line_nr);
      tokens.emplace_back(token::T_AMPERSAND, "&", line_nr);
      ++s;
      break;
      }
      }


    if (*s_copy && (s_copy == s))
      {
      buff += *s;
      ++s;
      }

    }

  _treat_buffer(buff, tokens, line_nr);

  return tokens;
  }

COMPILER_END