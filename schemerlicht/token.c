#include "token.h"
#include "limits.h"
#include "string.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

schemerlicht_flonum schemerlicht_to_flonum(const char* value)
  {
  return cast(schemerlicht_flonum, atof(value));
  }

schemerlicht_fixnum schemerlicht_to_fixnum(const char* value)
  {
#ifdef _WIN32
  return cast(schemerlicht_fixnum, _atoi64(value));
#else
  char* endptr;
  return cast(schemerlicht_fixnum, strtoull(value, &endptr, 10));
#endif
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
        case 'a': *s = '\a'; ++escapes; memmove(t, t + 1, str->string_length - escapes - char_index); break;
        case 'b': *s = '\b'; ++escapes; memmove(t, t + 1, str->string_length - escapes - char_index); break;
        case 'n': *s = '\n'; ++escapes; memmove(t, t + 1, str->string_length - escapes - char_index); break;
        case 'r': *s = '\r'; ++escapes; memmove(t, t + 1, str->string_length - escapes - char_index); break;
        case 't': *s = '\t'; ++escapes; memmove(t, t + 1, str->string_length - escapes - char_index); break;
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
        token t = make_token(ctxt, SCHEMERLICHT_T_FLONUM, line_nr, column_nr - (int)buff->string_length, buff);
        schemerlicht_vector_push_back(ctxt, tokens, t, token);
        }
      else
        {
        token t = make_token(ctxt, SCHEMERLICHT_T_FIXNUM, line_nr, column_nr - (int)buff->string_length, buff);
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


static int treat_buffer_token(schemerlicht_context* ctxt, schemerlicht_string* buff, token* tok, int line_nr, int column_nr, int* is_a_symbol)
  {
  int result = 0;
  if (*is_a_symbol)
    {
    *is_a_symbol = 0;
    if (buff->string_length == 0) // empty
      {
      *tok = make_token(ctxt, SCHEMERLICHT_T_BAD, line_nr, column_nr, buff);
      result = 1;
      }
    else
      {
      if (*schemerlicht_string_front(buff) == '#') // special case. some primitives (inlined ones) can start with two ##      
        {
        schemerlicht_string_push_front(ctxt, buff, '#');
        *tok = make_token(ctxt, SCHEMERLICHT_T_ID, line_nr, column_nr - (int)buff->string_length, buff);
        result = 1;
        }
      else
        {
        schemerlicht_string_push_front(ctxt, buff, '#');
        *tok = make_token(ctxt, SCHEMERLICHT_T_SYMBOL, line_nr, column_nr - (int)buff->string_length, buff);
        result = 1;
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
        *tok = make_token(ctxt, SCHEMERLICHT_T_FLONUM, line_nr, column_nr - (int)buff->string_length, buff);
        result = 1;
        }
      else
        {
        *tok = make_token(ctxt, SCHEMERLICHT_T_FIXNUM, line_nr, column_nr - (int)buff->string_length, buff);
        result = 1;
        }
      }
    else
      {
      *tok = make_token(ctxt, SCHEMERLICHT_T_ID, line_nr, column_nr - (int)buff->string_length, buff);
      result = 1;
      }
    }

  schemerlicht_string_clear(buff);
  return result;
  }

// read == 0 equals a peek
static int get_char(char* ch, schemerlicht_stream* str, int read)
  {
  if (str->position >= str->length)
    {
    return 0;
    }
  *ch = str->data[str->position];
  str->position += read;
  return 1;
  }

static void next_char(schemerlicht_stream* str)
  {
  ++str->position;
  }

typedef struct read_token_state
  {
  int line_nr;
  int column_nr;
  } read_token_state;

static int read_token(token* tok, schemerlicht_context* ctxt, schemerlicht_string* buff, schemerlicht_stream* str, read_token_state* state)
  {
  schemerlicht_assert(buff->string_length == 0);

  int is_a_symbol = 0;
  int is_a_character = 0;

  char s;
  int valid_chars_remaining = get_char(&s, str, 0);
  while (valid_chars_remaining)
    {
    if (ignore_character(s))
      {
      if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
        {        
        return 1;
        }      
      is_a_character = 0;
      while (ignore_character(s) && valid_chars_remaining)
        {
        if (s == '\n')
          {
          ++state->line_nr;
          state->column_nr = 0;
          }
        ++str->position;
        valid_chars_remaining = get_char(&s, str, 0);
        ++state->column_nr;
        }
      if (!valid_chars_remaining)
        break;
      }

    const schemerlicht_memsize str_position = str->position;
    if (!is_a_character) // any special sign can appear as a character, e.g. #\(
      {
      switch (s)
        {
        case '(':
        {
        if (is_a_symbol && buff->string_length == 0) // #(
          {
          ++str->position;
          is_a_symbol = 0;
          *tok = make_token_cstr(ctxt, SCHEMERLICHT_T_LEFT_ROUND_BRACKET, state->line_nr, state->column_nr, "#(");
          ++state->column_nr;
          return 1;
          }
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {          
          return 1;
          }
        ++str->position;
        *tok = make_token_cstr(ctxt, SCHEMERLICHT_T_LEFT_ROUND_BRACKET, state->line_nr, state->column_nr, "(");
        ++state->column_nr;
        return 1;
        }
        case ')':
        {
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {          
          return 1;
          }
        ++str->position;
        *tok = make_token_cstr(ctxt, SCHEMERLICHT_T_RIGHT_ROUND_BRACKET, state->line_nr, state->column_nr, ")");
        ++state->column_nr;
        return 1;
        }
        case '[':
        {
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {          
          return 1;
          }
        ++str->position;
        *tok = make_token_cstr(ctxt, SCHEMERLICHT_T_LEFT_SQUARE_BRACKET, state->line_nr, state->column_nr, "[");
        ++state->column_nr;
        return 1;
        }
        case ']':
        {
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {          
          return 1;
          }
        ++str->position;
        *tok = make_token_cstr(ctxt, SCHEMERLICHT_T_RIGHT_SQUARE_BRACKET, state->line_nr, state->column_nr, "]");
        ++state->column_nr;
        return 1;
        }
        case '#':
        {
        if (is_a_symbol)
          break;
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {          
          return 1;
          }
        ++str->position;
        char t;
        int valid_peek = get_char(&t, str, 0); // peek
        if (valid_peek && t == ';') //treat as comment
          {
          while (valid_chars_remaining && s != '\n') // comment, so skip till end of the line
            valid_chars_remaining = get_char(&s, str, 1);
          valid_chars_remaining = get_char(&s, str, 0);
          ++state->line_nr;
          state->column_nr = 1;
          }
        else if (valid_peek && t == '|') //treat as multiline comment
          {
          valid_chars_remaining = get_char(&s, str, 1);
          valid_chars_remaining = get_char(&s, str, 1);
          ++state->column_nr;
          int end_of_comment_found = 0;
          while (!end_of_comment_found)
            {
            while (valid_chars_remaining && s != '|')
              {
              if (s == '\n')
                {
                ++state->line_nr;
                state->column_nr = 0;
                }
              valid_chars_remaining = get_char(&s, str, 1);
              ++state->column_nr;
              }
            if (!valid_chars_remaining)
              {
              end_of_comment_found = 1;
              }
            else
              {
              valid_chars_remaining = get_char(&s, str, 1);
              if (valid_chars_remaining && s == '#')
                end_of_comment_found = 1;
              }
            }
          valid_chars_remaining = get_char(&s, str, 0);
          ++state->column_nr;
          }
        else
          {
          is_a_symbol = 1;
          valid_chars_remaining = get_char(&s, str, 0);
          ++state->column_nr;
          }
        break;
        }
        case ';':
        {
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {          
          return 1;
          }
        ++str->position;
        while (valid_chars_remaining && s != '\n') // comment, so skip till end of the line
          valid_chars_remaining = get_char(&s, str, 1);
        valid_chars_remaining = get_char(&s, str, 0);
        ++state->line_nr;
        state->column_nr = 1;
        break;
        }
        case '\'':
        {
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {          
          return 1;
          }
        ++str->position;
        *tok = make_token_cstr(ctxt, SCHEMERLICHT_T_QUOTE, state->line_nr, state->column_nr, "'");
        ++state->column_nr;
        return 1;
        }
        case '`':
        {
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {
          return 1;
          }
        ++str->position;
        *tok = make_token_cstr(ctxt, SCHEMERLICHT_T_BACKQUOTE, state->line_nr, state->column_nr, "`");
        ++state->column_nr;
        return 1;
        }
        case ',':
        {
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {          
          return 1;
          }
        ++str->position;
        char t;
        int valid_peek = get_char(&t, str, 0); // peek
        if (valid_peek && t == '@')
          {
          *tok = make_token_cstr(ctxt, SCHEMERLICHT_T_UNQUOTE_SPLICING, state->line_nr, state->column_nr, ",@");
          ++str->position;
          //valid_chars_remaining = get_char(&s, str, 0); // not necessary
          state->column_nr += 2;
          return 1;
          }
        else
          {
          *tok = make_token_cstr(ctxt, SCHEMERLICHT_T_UNQUOTE, state->line_nr, state->column_nr, ",");
          ++state->column_nr;
          return 1;
          }
        break;
        }
        case '"':
        {
        if (treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol))
          {          
          return 1;
          }
        ++str->position;
        int temp_column_nr = state->column_nr;
        int temp_line_nr = state->line_nr;
        schemerlicht_string tmp;
        schemerlicht_string_init(ctxt, &tmp, "\"");
        valid_chars_remaining = get_char(&s, str, 1);
        ++state->column_nr;
        while (valid_chars_remaining && s != '"')
          {
          schemerlicht_string_push_back(ctxt, &tmp, s);
          if (s == '\n')
            {
            ++state->line_nr;
            state->column_nr = 0;
            }
          if (s == '\\') // escape syntax
            {
            valid_chars_remaining = get_char(&s, str, 1);
            if (!valid_chars_remaining)
              {
              *tok = make_token(ctxt, SCHEMERLICHT_T_BAD, temp_line_nr, temp_column_nr, &tmp);
              schemerlicht_string_destroy(ctxt, &tmp);
              return 1;
              }
            schemerlicht_string_push_back(ctxt, &tmp, s);
            }
          valid_chars_remaining = get_char(&s, str, 1);
          ++state->column_nr;
          }
        if (valid_chars_remaining)
          {
          schemerlicht_string_push_back(ctxt, &tmp, s);
          //valid_chars_remaining = get_char(&s, str, 1);
          //++state->column_nr;
          }
        replace_escape_chars(&tmp);
        *tok = make_token(ctxt, SCHEMERLICHT_T_STRING, temp_line_nr, temp_column_nr, &tmp);
        schemerlicht_string_destroy(ctxt, &tmp);
        return 1;
        }
        } // switch (*s)
      }

    if (str_position == str->position && valid_chars_remaining)
      {
      schemerlicht_string_push_back(ctxt, buff, s);
      ++str->position;
      valid_chars_remaining = get_char(&s, str, 0);
      ++state->column_nr;
      }

    is_a_character = (is_a_symbol && buff->string_length == 1 && buff->string_ptr[0] == '\\');
    }  
  return treat_buffer_token(ctxt, buff, tok, state->line_nr, state->column_nr, &is_a_symbol);
  }

schemerlicht_vector tokenize(schemerlicht_context* ctxt, schemerlicht_stream* str)
  {
  schemerlicht_vector tokens;
  schemerlicht_vector_init(ctxt, &tokens, token);

  schemerlicht_string buff;
  schemerlicht_string_init(ctxt, &buff, "");

  read_token_state state;
  state.line_nr = 1;
  state.column_nr = 1;

  token tok;
  while (read_token(&tok, ctxt, &buff, str, &state))
    {
    schemerlicht_vector_push_back(ctxt, &tokens, tok, token);
    }

  schemerlicht_string_destroy(ctxt, &buff);
  return tokens;
  }

schemerlicht_vector tokenize_old(schemerlicht_context* ctxt, schemerlicht_stream* str)
  {
  schemerlicht_vector tokens;
  schemerlicht_vector_init(ctxt, &tokens, token);

  schemerlicht_string buff;
  schemerlicht_string_init(ctxt, &buff, "");

  int is_a_symbol = 0;
  int is_a_character = 0;

  int line_nr = 1;
  int column_nr = 1;

  char s;
  int valid_chars_remaining = get_char(&s, str, 1);
  while (valid_chars_remaining)
    {
    if (ignore_character(s))
      {
      treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
      is_a_character = 0;
      while (ignore_character(s) && valid_chars_remaining)
        {
        if (s == '\n')
          {
          ++line_nr;
          column_nr = 0;
          }
        valid_chars_remaining = get_char(&s, str, 1);
        ++column_nr;
        }
      if (!valid_chars_remaining)
        break;
      }

    const schemerlicht_memsize str_position = str->position;
    if (!is_a_character) // any special sign can appear as a character, e.g. #\(
      {
      switch (s)
        {
        case '(':
        {
        if (is_a_symbol && buff.string_length == 0) // #(
          {
          is_a_symbol = 0;
          token t = make_token_cstr(ctxt, SCHEMERLICHT_T_LEFT_ROUND_BRACKET, line_nr, column_nr, "#(");
          schemerlicht_vector_push_back(ctxt, &tokens, t, token);
          valid_chars_remaining = get_char(&s, str, 1);
          ++column_nr;
          break;
          }
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_LEFT_ROUND_BRACKET, line_nr, column_nr, "(");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        valid_chars_remaining = get_char(&s, str, 1);
        ++column_nr;
        break;
        }
        case ')':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_RIGHT_ROUND_BRACKET, line_nr, column_nr, ")");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        valid_chars_remaining = get_char(&s, str, 1);
        ++column_nr;
        break;
        }
        case '[':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_LEFT_SQUARE_BRACKET, line_nr, column_nr, "[");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        valid_chars_remaining = get_char(&s, str, 1);
        ++column_nr;
        break;
        }
        case ']':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_RIGHT_SQUARE_BRACKET, line_nr, column_nr, "]");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        valid_chars_remaining = get_char(&s, str, 1);
        ++column_nr;
        break;
        }
        case '#':
        {
        if (is_a_symbol)
          break;
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        char t;
        int valid_peek = get_char(&t, str, 0); // peek
        if (valid_peek && t == ';') //treat as comment
          {
          while (valid_chars_remaining && s != '\n') // comment, so skip till end of the line
            valid_chars_remaining = get_char(&s, str, 1);
          valid_chars_remaining = get_char(&s, str, 1);
          ++line_nr;
          column_nr = 1;
          }
        else if (valid_peek && t == '|') //treat as multiline comment
          {
          valid_chars_remaining = get_char(&s, str, 1);
          valid_chars_remaining = get_char(&s, str, 1);
          ++column_nr;
          int end_of_comment_found = 0;
          while (!end_of_comment_found)
            {
            while (valid_chars_remaining && s != '|')
              {
              if (s == '\n')
                {
                ++line_nr;
                column_nr = 0;
                }
              valid_chars_remaining = get_char(&s, str, 1);
              ++column_nr;
              }
            if (!valid_chars_remaining)
              {
              end_of_comment_found = 1;
              }
            else
              {
              valid_chars_remaining = get_char(&s, str, 1);
              if (valid_chars_remaining && s == '#')
                end_of_comment_found = 1;
              }
            }
          valid_chars_remaining = get_char(&s, str, 1);
          ++column_nr;
          }
        else
          {
          is_a_symbol = 1;
          valid_chars_remaining = get_char(&s, str, 1);
          ++column_nr;
          }
        break;
        }
        case ';':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        while (valid_chars_remaining && s != '\n') // comment, so skip till end of the line
          valid_chars_remaining = get_char(&s, str, 1);
        valid_chars_remaining = get_char(&s, str, 1);
        ++line_nr;
        column_nr = 1;
        break;
        }
        case '\'':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_QUOTE, line_nr, column_nr, "'");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        valid_chars_remaining = get_char(&s, str, 1);
        ++column_nr;
        break;
        }
        case '`':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        token t = make_token_cstr(ctxt, SCHEMERLICHT_T_BACKQUOTE, line_nr, column_nr, "`");
        schemerlicht_vector_push_back(ctxt, &tokens, t, token);
        valid_chars_remaining = get_char(&s, str, 1);
        ++column_nr;
        break;
        }
        case ',':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        char t;
        int valid_peek = get_char(&t, str, 0); // peek
        if (valid_peek && t == '@')
          {
          token tok = make_token_cstr(ctxt, SCHEMERLICHT_T_UNQUOTE_SPLICING, line_nr, column_nr, ",@");
          schemerlicht_vector_push_back(ctxt, &tokens, tok, token);
          valid_chars_remaining = get_char(&s, str, 1);
          ++column_nr;
          }
        else
          {
          token tok = make_token_cstr(ctxt, SCHEMERLICHT_T_UNQUOTE, line_nr, column_nr, ",");
          schemerlicht_vector_push_back(ctxt, &tokens, tok, token);
          }
        valid_chars_remaining = get_char(&s, str, 1);
        ++column_nr;
        break;
        }
        case '"':
        {
        treat_buffer(ctxt, &buff, &tokens, line_nr, column_nr, &is_a_symbol);
        int temp_column_nr = column_nr;
        int temp_line_nr = line_nr;
        schemerlicht_string tmp;
        schemerlicht_string_init(ctxt, &tmp, "\"");
        valid_chars_remaining = get_char(&s, str, 1);
        ++column_nr;
        while (valid_chars_remaining && s != '"')
          {
          schemerlicht_string_push_back(ctxt, &tmp, s);
          if (s == '\n')
            {
            ++line_nr;
            column_nr = 0;
            }
          if (s == '\\') // escape syntax
            {
            valid_chars_remaining = get_char(&s, str, 1);
            if (!valid_chars_remaining)
              {
              token tok = make_token(ctxt, SCHEMERLICHT_T_BAD, temp_line_nr, temp_column_nr, &tmp);
              schemerlicht_vector_push_back(ctxt, &tokens, tok, token);
              schemerlicht_string_destroy(ctxt, &tmp);
              break;
              }
            schemerlicht_string_push_back(ctxt, &tmp, s);
            }
          valid_chars_remaining = get_char(&s, str, 1);
          ++column_nr;
          }
        if (valid_chars_remaining)
          {
          schemerlicht_string_push_back(ctxt, &tmp, s);
          valid_chars_remaining = get_char(&s, str, 1);
          ++column_nr;
          }
        replace_escape_chars(&tmp);
        token tok = make_token(ctxt, SCHEMERLICHT_T_STRING, temp_line_nr, temp_column_nr, &tmp);
        schemerlicht_vector_push_back(ctxt, &tokens, tok, token);
        schemerlicht_string_destroy(ctxt, &tmp);
        break;
        }
        } // switch (*s)
      }

    if (str_position == str->position && valid_chars_remaining)
      {
      schemerlicht_string_push_back(ctxt, &buff, s);
      valid_chars_remaining = get_char(&s, str, 1);
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