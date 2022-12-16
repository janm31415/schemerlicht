#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

void read_file(std::ostream& ostr, const char* filename)
  {
  std::ifstream t(filename);
  ostr << t.rdbuf();
  }

void write_to_file(const std::string& filename, const std::string& content)
  {
  std::ofstream t(filename);
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
  FINDING_ENDING_BRACE,
  FINDING_ENUM,  
  FINDING_ENUM_END,
  DONE
  };

struct parse_state
  {
  int count_brace;
  parse_action action;
  const char* s;
  std::string current_prim_name;
  std::string current_prim_enum;
  const char* first_brace;
  std::vector<std::string> enum_values;
  std::vector<std::pair<std::string, std::string> > primitive_pairs;
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
    case parse_action::FINDING_ENUM:
    {
    if (buff == std::string("enum"))
      {
      state.action = parse_action::FINDING_ENUM_END;
      }
    break;
    }
    case parse_action::FINDING_ENUM_END:
    {
    if (!buff.empty())
      state.enum_values.push_back(buff);
    break;
    }
    case parse_action::DONE:
    {
    break;
    }
    }
  buff.clear();
  }

void parse_file(parse_state& state, const std::string& str, bool header)
  {
  std::string word;
  std::string buff;
  const char* s_end = str.c_str() + str.length();
  state.count_brace = 0;
  if (header)
    state.action = parse_action::FINDING_ENUM;
  else
    state.action = parse_action::FINDING_PRIMITIVE;
  state.s = str.c_str();
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
        state.primitive_pairs.emplace_back(state.current_prim_enum, state.current_prim_name);
        state.action = parse_action::FINDING_PRIMITIVE;
        state.current_prim_enum.clear();
        state.current_prim_name.clear();
        }
      else if (state.action == parse_action::FINDING_ENUM_END)
        {
        state.action = parse_action::DONE;
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
  }

void build_dispatch_file(const parse_state& state, const std::string& outputfile)
  {
  std::stringstream str;
  str << "//This file is autogenerated with primdispatch\n\n";
  str << "typedef void (*prim_fun)(schemerlicht_context*, int, int, int);\n\n";
  str << "static prim_fun prim_fun_dispatch_table[] = {\n";

  for (const std::string& enum_value : state.enum_values)
    {
    auto it = std::find_if(state.primitive_pairs.begin(), state.primitive_pairs.end(), [&](const std::pair<std::string, std::string>& pr)
      {
      return pr.first == enum_value;
      });
    str << "&schemerlicht_primitive_" << it->second << ",\n";
    }

  str << "};";
  std::string filecontent = str.str();
  write_to_file(outputfile, filecontent);
  }

int main(int argc, char** argv)
  {
  if (argc < 3)
    {
    printf("Usage: primdispatch <outputfile> <source folder>");
    printf("outputfile will contain a dispatch for all primitives\n");
    printf("source folder is expected to be the schemerlicht source folder that contains files primitives.h/c\n");
    }
  else
    {
    std::string outputfile(argv[1]);
    std::string folder(argv[2]);
    if (folder.back() != '/' && folder.back() != '\\')
      folder.push_back('/');
    std::stringstream str;
    std::string primitives_header_file = folder + std::string("primitives.h");
    std::string primitives_source_file = folder + std::string("primitives.c");
    read_file(str, primitives_header_file.c_str());
    std::string prim_header_file = str.str();
    str.str("");
    str.clear();
    read_file(str, primitives_source_file.c_str());
    std::string prim_source_file = str.str();
    parse_state state;
    parse_file(state, prim_header_file, true);
    parse_file(state, prim_source_file, false);
    build_dispatch_file(state, outputfile);
    }
  return 0;
  }
