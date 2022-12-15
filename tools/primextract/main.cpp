#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

void read_file(std::ostream& ostr, const char* filename)
  {
  std::ifstream t(filename);
  ostr << t.rdbuf();
  }

void write_to_file(const std::string& filename, const std::string& content, const std::string& folder)
  {
  std::string fn = folder + filename + std::string(".h");
  std::ofstream t(fn);
  t << content;
  }

void replace_in_string(std::string& s, const std::string& substring_to_be_replaced, const std::string& replace_by_this_string)
  {
  std::string::size_type pos = 0;
  while ((pos = s.find(substring_to_be_replaced, pos)) != std::string::npos)
    {
    s.replace(pos, substring_to_be_replaced.size(), replace_by_this_string);
    pos += replace_by_this_string.size();
    }
  }

std::string get_prim_enum(const std::string& prim_name)
  {
  if (prim_name == std::string("halt"))
    {
    return std::string("SCHEMERLICHT_HALT");
    }
  else if (prim_name == std::string("equal"))
    {
    return std::string("SCHEMERLICHT_EQUAL");
    }
  else if (prim_name == std::string("string_less"))
    {
    return std::string("SCHEMERLICHT_STRING_LESS");
    }
  else if (prim_name == std::string("string_greater"))
    {
    return std::string("SCHEMERLICHT_STRING_GREATER");
    }
  else if (prim_name == std::string("string_ci_less"))
    {
    return std::string("SCHEMERLICHT_STRING_CI_LESS");
    }
  else if (prim_name == std::string("string_ci_greater"))
    {
    return std::string("SCHEMERLICHT_STRING_CI_GREATER");
    }
  return std::string();
  }

bool ignore_character(const char& ch)
  {
  return (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r');
  }

enum class parse_action
  {
  FINDING_PRIMITIVE,
  FINDING_BEGINNING_BRACE,
  FINDING_ENDING_BRACE
  };

struct parse_state
  {
  int count_brace;
  parse_action action;
  const char* s;
  std::string current_prim_name;
  std::string current_prim_enum;
  const char* first_brace;
  };

void treat_buffer(std::string& buff, parse_state& state)
  {
  switch (state.action)
    {
    case parse_action::FINDING_PRIMITIVE:
    {
    std::size_t pos = buff.find("schemerlicht_primitive_");
    if (pos != std::string::npos && state.count_brace == 0)
      {
      std::string prim_name = buff.substr(23);
      state.current_prim_name = prim_name;
      state.action = parse_action::FINDING_BEGINNING_BRACE;
      }
    break;
    }
    case parse_action::FINDING_BEGINNING_BRACE:
    {
    break;
    }
    case parse_action::FINDING_ENDING_BRACE:
    {
    std::size_t pos = buff.find("SCHEMERLICHT_");
    if (pos != std::string::npos && state.count_brace == 1 && state.current_prim_enum.empty())
      {
      state.current_prim_enum = buff;
      }
    break;
    }
    }
  buff.clear();
  }

void parse_file(const std::string& str, const std::string& folder)
  {
  std::string word;
  std::string buff;
  const char* s_end = str.c_str() + str.length();
  parse_state state;
  state.count_brace = 0;
  state.action = parse_action::FINDING_PRIMITIVE;
  state.s = str.c_str();
  std::stringstream large_switch;
  large_switch << "switch (function_id)\n  {\n";
  while (state.s < s_end)
    {
    if (ignore_character(*state.s))
      {
      treat_buffer(buff, state);
      while (ignore_character(*state.s))
        {
        ++state.s;
        }
      }

    const char* s_copy = state.s;
    switch (*state.s)
      {
      case '(':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case ')':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case '[':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case ']':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case '{':
      {
      treat_buffer(buff, state);
      if (state.count_brace == 0 && state.action == parse_action::FINDING_BEGINNING_BRACE)
        {
        state.first_brace = state.s;
        state.action = parse_action::FINDING_ENDING_BRACE;
        }
      ++state.count_brace;
      ++state.s;
      break;
      }
      case '}':
      {
      treat_buffer(buff, state);
      --state.count_brace;
      if (state.count_brace == 0 && state.action == parse_action::FINDING_ENDING_BRACE)
        {
        std::string prim_content(state.first_brace, state.s + 1);
        if (state.current_prim_enum.empty())
          state.current_prim_enum = get_prim_enum(state.current_prim_name);
        std::cout << "Content for primitive " << state.current_prim_name << " with enum " << state.current_prim_enum << std::endl;
        std::cout << prim_content << std::endl;
        write_to_file(state.current_prim_name, prim_content, folder);
        large_switch << "  case " << state.current_prim_enum << ":\n  ";
        //replace_in_string(prim_content, "return;", "goto SCHEMERLICHT_STOP_LABEL;");
        large_switch << prim_content;
        large_switch << "\n  break;\n";
        state.action = parse_action::FINDING_PRIMITIVE;
        state.current_prim_enum.clear();
        state.current_prim_name.clear();
        }
      ++state.s;
      break;
      }
      case '%':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case '+':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case '-':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case '*':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case '/':
      {
      treat_buffer(buff, state);
      const char* t = state.s; ++t;
      if (*t && *t == '/')
        {
        state.s = t;
        while (*state.s && *state.s != '\n') // comment, so skip till end of the line
          ++state.s;
        }
      else if (*t && *t == '*')
        {
        state.s = t;
        ++state.s;
        bool end_of_comment_found = false;
        while (!end_of_comment_found)
          {
          while (*state.s && *state.s != '*')
            ++state.s;
          if (!(*state.s) || (*(++state.s) == '/'))
            end_of_comment_found = true;
          }
        }
      ++state.s;
      break;
      }
      case '>':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case '<':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case '=':
      {

      treat_buffer(buff, state);
      ++state.s;

      break;
      }
      case ';':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case ',':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      case '&':
      {
      treat_buffer(buff, state);
      ++state.s;
      break;
      }
      }
    if (*s_copy && (s_copy == state.s))
      {
      buff += *state.s;
      ++state.s;
      }

    }
  treat_buffer(buff, state);
  large_switch << "  default:\n";
  large_switch << "    schemerlicht_throw(ctxt, SCHEMERLICHT_ERROR_NOT_IMPLEMENTED);\n";
  large_switch << "    break;\n";
  large_switch << "  }\n";
  std::string large_switch_content = large_switch.str();
  write_to_file("large_switch", large_switch_content, folder);
  }

int main(int argc, char** argv)
  {
  if (argc < 3)
    {
    printf("Usage: primextract <primitives.c> <folder>");
    }
  else
    {
    std::string primitives_file(argv[1]);
    std::string folder(argv[2]);
    if (folder.back() != '/' && folder.back() != '\\')
      folder.push_back('/');
    std::stringstream str;
    read_file(str, primitives_file.c_str());
    std::string prim_file = str.str();
    parse_file(prim_file, folder);
    }
  return 0;
  }