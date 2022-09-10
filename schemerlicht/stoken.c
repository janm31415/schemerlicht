#include "stoken.h"
#include "slimits.h"
#include "sstring.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

schemerlicht_flonum to_flonum(const char* value)
  {
  return cast(schemerlicht_flonum, atof(value));
  }

schemerlicht_fixnum to_fixnum(const char* value)
  {
  return cast(schemerlicht_fixnum, _atoi64(value));
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

static int ignore_character(char ch)
  {
  return (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r');
  }

token make_token(schemerlicht_context* ctxt, int type, int line_nr, int column_nr, schemerlicht_string* value)
  {
  token t;
  t.type = type;
  t.line_nr = line_nr;
  t.column_nr = column_nr;
  schemerlicht_string_init(ctxt, &t.value, value->string_ptr);
  return t;
  }

token make_token_cstr(schemerlicht_context* ctxt, int type, int line_nr, int column_nr, const char* value)
  {
  token t;
  t.type = type;
  t.line_nr = line_nr;
  t.column_nr = column_nr;
  schemerlicht_string_init(ctxt, &t.value, value);
  return t;
  }

token make_token_flonum(schemerlicht_context* ctxt, int line_nr, int column_nr, schemerlicht_string* value)
  {
  token t;
  t.type = SCHEMERLICHT_T_FLONUM;
  t.line_nr = line_nr;
  t.column_nr = column_nr;
  schemerlicht_string_init(ctxt, &t.value, value->string_ptr);
  return t;
  }

token make_token_fixnum(schemerlicht_context* ctxt, int line_nr, int column_nr, schemerlicht_string* value)
  {
  token t;
  t.type = SCHEMERLICHT_T_FIXNUM;
  t.line_nr = line_nr;
  t.column_nr = column_nr;
  schemerlicht_string_init(ctxt, &t.value, value->string_ptr);
  return t;
  }

static void replace_escape_chars(schemerlicht_string* str)
  {
  if (str->string_length == 0)
    return;
  char* s = str->string_ptr;
  int escapes = 0;
  int char_index = 0;
  while (*s)
    {
    if (*s == '\\')
      {
      char* t = s;
      ++t;
      switch (*t)
        {
        case 'a': *s = '\a'; ++escapes; memmove(t, t+1, str->string_length-escapes-char_index); break;
        case 'b': *s = '\b'; ++escapes; memmove(t, t+1, str->string_length-escapes-char_index); break;
        case 'n': *s = '\n'; ++escapes; memmove(t, t+1, str->string_length-escapes-char_index); break;
        case 'r': *s = '\r'; ++escapes; memmove(t, t+1, str->string_length-escapes-char_index); break;
        case 't': *s = '\t'; ++escapes; memmove(t, t+1, str->string_length-escapes-char_index); break;
        default:
          ++s; ++char_index; break;
        }
      }
    else
      {
      ++s;
      ++char_index;
      }
    } 
  }

static void treat_buffer(schemerlicht_context* ctxt, schemerlicht_string* buff, schemerlicht_vector* tokens, int line_nr, int column_nr, int* is_a_symbol)
  {
  if (*is_a_symbol)
    {
    *is_a_symbol = 0;
    if (buff->string_length == 0) // empty
      {
      token t = make_token(ctxt, SCHEMERLICHT_T_BAD, line_nr, column_nr, buff);
      schemerlicht_vector_push_back(ctxt, tokens, t, token);
      }
    else
      {
      if (*schemerlicht_string_front(buff) == '#') // special case. some primitives (inlined ones) can start with two ##      
        {
        schemerlicht_string_push_front(ctxt, buff, '#');
        token t = make_token(ctxt, SCHEMERLICHT_T_ID, line_nr, column_nr - (int)buff->string_length, buff);
        schemerlicht_vector_push_back(ctxt, tokens, t, token);
        }
      else
        {
        schemerlicht_string_push_front(ctxt, buff, '#');
        token t = make_token(ctxt, SCHEMERLICHT_T_SYMBOL, line_nr, column_nr - (int)buff->string_length, buff);
        schemerlicht_vector_push_back(ctxt, tokens, t, token);
        }
      }
    }
  else if (buff->string_length != 0 && *schemerlicht_string_front(buff) != '\0')
    {
    int is_real;
    int is_scientific;
    if (is_number(&is_real, &is_scientific, buff->string_ptr))
      {
      if (is_real)
        {        
        token t = make_token_flonum(ctxt, line_nr, column_nr - (int)buff->string_length, buff);
        schemerlicht_vector_push_back(ctxt, tokens, t, token);
        }
      else
        {
        token t = make_token_fixnum(ctxt, line_nr, column_nr - (int)buff->string_length, buff);
        schemerlicht_vector_push_back(ctxt, tokens, t, token);
        }
      }
    else
      {
      token t = make_token(ctxt, SCHEMERLICHT_T_ID, line_nr, column_nr - (int)buff->string_length, buff);
      schemerlicht_vector_push_back(ctxt, tokens, t, token);
      }
    }

  schemerlicht_string_clear(buff);
  }

schemerlicht_vector tokenize(schemerlicht_context* ctxt, schemerlicht_stream* str)
  {
  schemerlicht_vector tokens;
  schemerlicht_vector_init(ctxt, &tokens, token);

  schemerlicht_string buff;
  schemerlicht_string_init(ctxt, &buff, "");

  const char* s = str->data + str->position;
  const char* s_end = str->data + str->length;

  int is_a_symbol = 0;
  int is_a_character = 0;

  int line_nr = 1;
  int column_nr = 1;

  while (s < s_end)
    {
    if (ignore_character(*s))
      {
      treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
      is_a_character = 0;
      while (ignore_character(*s) && (s < s_end))
        {
        if (*s == '\n')
          {
          ++line_nr;
          column_nr = 0;
          }
        ++s;
        ++column_nr;
        }
      if (s >= s_end)
        break;
      }

    const char* s_copy = s;
    if (!is_a_character) // any special sign can appear as a character, e.g. #\(
      {
      switch (*s)
        {
        case '(':
        {
        if (is_a_symbol && buff.string_length == 0) // #(
          {
          is_a_symbol = 0;
          token t = make_token_cstr(ctxt, SCHEMERLICHT_T_LEFT_ROUND_BRACKET, line_nr, column_nr, "#(");
          schemerlicht_vector_push_back(ctxt, &tokens, t, token);
          ++s;
          ++column_nr;
          break;
          }
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_LEFT_ROUND_BRACKET, line_nr, column_nr, "(");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        ++s;
        ++column_nr;
        break;
        }
        case ')':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_RIGHT_ROUND_BRACKET, line_nr, column_nr, ")");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        ++s;
        ++column_nr;
        break;
        }
        case '[':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_LEFT_SQUARE_BRACKET, line_nr, column_nr, "[");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        ++s;
        ++column_nr;
        break;
        }
        case ']':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_RIGHT_SQUARE_BRACKET, line_nr, column_nr, "]");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        ++s;
        ++column_nr;
        break;
        }
        case '#':
        {
        if (is_a_symbol)
          break;
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        const char* t = s;
        ++t;
        if (t && *t == ';') //treat as comment
          {
          while (*s && *s != '\n') // comment, so skip till end of the line
            ++s;
          ++s;
          ++line_nr;
          column_nr = 1;
          }
        else if (t && *t == '|') //treat as multiline comment
          {
          s = t;
          ++s; ++column_nr;
          int end_of_comment_found = 0;
          while (!end_of_comment_found)
            {
            while (*s && *s != '|')
              {
              if (*s == '\n')
                {
                ++line_nr;
                column_nr = 0;
                }
              ++s;
              ++column_nr;
              }
            if (!(*s) || (*(++s) == '#'))
              end_of_comment_found = 1;
            }
          ++s; ++column_nr;
          }
        else
          {
          is_a_symbol = 1;
          ++s;
          ++column_nr;
          }
        break;
        }
        case ';':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        while (*s && *s != '\n') // comment, so skip till end of the line
          ++s;
        ++s;
        ++line_nr;
        column_nr = 1;
        break;
        }
        case '\'':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_QUOTE, line_nr, column_nr, "'");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        ++s;
        ++column_nr;
        break;
        }
        case '`':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_BACKQUOTE, line_nr, column_nr, "`");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        ++s;
        ++column_nr;
        break;
        }
        case ',':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        const char* t = s;
        ++t;
        if (*t && *t == '@')
          {
          token tok = make_token_cstr(ctxt, SCHEMERLICHT_T_UNQUOTE_SPLICING, line_nr, column_nr, ",@");
          schemerlicht_vector_push_back(ctxt, &tokens, tok, token);
          ++s;
          ++column_nr;
          }
        else
          {
          token tok = make_token_cstr(ctxt, SCHEMERLICHT_T_UNQUOTE, line_nr, column_nr, ",");
          schemerlicht_vector_push_back(ctxt, &tokens, tok, token);
          }
        ++s;
        ++column_nr;
        break;
        }
        case '"':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        int temp_column_nr = column_nr;
        int temp_line_nr = line_nr;
        const char* t = s;
        ++t;
        ++column_nr;
        while (*t && *t != '"')
          {
          if (*t == '\n')
            {
            ++line_nr;
            column_nr = 0;
            }
          if (*t == '\\') // escape syntax
            {
            ++t;
            if (!*t)
              {
              schemerlicht_string tmp;
              schemerlicht_string_init_ranged(ctxt, &tmp, s, t);
              token tok = make_token(ctxt, SCHEMERLICHT_T_BAD, temp_line_nr, temp_column_nr, &tmp);
              schemerlicht_vector_push_back(ctxt, &tokens, tok, token);
              schemerlicht_string_destroy(ctxt, &tmp);
              break;
              }
            }
          ++t;
          ++column_nr;
          }
        if (*t)
          {
          ++t;
          ++column_nr;
          }
        schemerlicht_string tmp;
        schemerlicht_string_init_ranged(ctxt, &tmp, s, t);
        replace_escape_chars(&tmp);
        token tok = make_token(ctxt, SCHEMERLICHT_T_STRING, temp_line_nr, temp_column_nr, &tmp);
        schemerlicht_vector_push_back(ctxt, &tokens, tok, token);
        schemerlicht_string_destroy(ctxt, &tmp);
        s = t;
        break;
        }
        } // switch (*s)
      }

    if (s_copy == s)
      {
      schemerlicht_string_push_back(ctxt, &buff, *s);
      ++s;
      ++column_nr;
      }

    is_a_character = (is_a_symbol && buff.string_length == 1 && buff.string_ptr[0] == '\\');
    }

  treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);

  schemerlicht_string_destroy(ctxt, &buff);
  return tokens;
  }

void destroy_tokens_vector(schemerlicht_context* ctxt, schemerlicht_vector* tokens)
  {
  token* it = schemerlicht_vector_begin(tokens, token);
  token* it_end = schemerlicht_vector_end(tokens, token);
  for (; it != it_end; ++it)
    {
    //if (it->type != SCHEMERLICHT_T_FLONUM && it->type != SCHEMERLICHT_T_FIXNUM)
      {
      schemerlicht_string_destroy(ctxt, &it->value);
      }
    }
  schemerlicht_vector_destroy(ctxt, tokens);
  }