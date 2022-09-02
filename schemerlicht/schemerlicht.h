#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"

#include <stdint.h>
#include <ostream>
#include <vector>

COMPILER_BEGIN

class scm_type;

typedef uint8_t* schemerlicht_compiled_function_ptr;

struct schemerlicht_parameters
  {
  COMPILER_SCHEMERLICHT_API schemerlicht_parameters();
  uint64_t heap_size;
  uint64_t globals_stack;
  uint32_t local_stack;
  uint64_t scheme_stack;
  std::ostream* trace;
  std::ostream* stderror;
  std::ostream* stdoutput;
  };

/*
Initializes schemerlicht.
Call func, passing it data and return what func returns.
*/
COMPILER_SCHEMERLICHT_API void* scheme_with_schemerlicht(void* (*func)(void*), void* data = nullptr, schemerlicht_parameters params = schemerlicht_parameters());

COMPILER_SCHEMERLICHT_API void schemerlicht_repl(int argc = 0, char** argv = nullptr);

COMPILER_SCHEMERLICHT_API void schemerlicht_quit();

COMPILER_SCHEMERLICHT_API schemerlicht_compiled_function_ptr schemerlicht_compile(const std::string& scheme_expression);

COMPILER_SCHEMERLICHT_API void* schemerlicht_get_context();

COMPILER_SCHEMERLICHT_API void* schemerlicht_clone_context(void* ctxt);

COMPILER_SCHEMERLICHT_API void schemerlicht_destroy_clone_context(void* ctxt);

template <typename... Args>
uint64_t schemerlicht_run_raw(schemerlicht_compiled_function_ptr fun, void* ctxt, Args... args)
  {
  uint64_t result = 39; // hardcoded scheme_undefined
  if (fun)
    {
    result = fun(ctxt, args...);
    }
  return result;
  }

COMPILER_SCHEMERLICHT_API uint64_t schemerlicht_run_raw(const std::string& scheme_expression);
COMPILER_SCHEMERLICHT_API uint64_t schemerlicht_runf_raw(const std::string& scheme_file);
COMPILER_SCHEMERLICHT_API std::string schemerlicht_raw_to_string(uint64_t scm_value, std::streamsize precision = 6);

COMPILER_SCHEMERLICHT_API void schemerlicht_run(const std::string& scheme_expression);
COMPILER_SCHEMERLICHT_API void schemerlicht_runf(const std::string& scheme_file);

COMPILER_SCHEMERLICHT_API void schemerlicht_show_help();
COMPILER_SCHEMERLICHT_API void schemerlicht_show_expand(const std::string& input);
COMPILER_SCHEMERLICHT_API void schemerlicht_show_assembly(const std::string& input);
COMPILER_SCHEMERLICHT_API void schemerlicht_show_unresolved();
COMPILER_SCHEMERLICHT_API void schemerlicht_show_external_primitives(const std::string& arg);
COMPILER_SCHEMERLICHT_API void schemerlicht_show_memory();
COMPILER_SCHEMERLICHT_API void schemerlicht_show_environment();

COMPILER_SCHEMERLICHT_API std::string schemerlicht_expand(const std::string& scheme_expression);

COMPILER_SCHEMERLICHT_API std::string schemerlicht_last_global_variable_used();

enum external_type
  {
  schemerlicht_bool,
  schemerlicht_char_pointer,
  schemerlicht_double,
  schemerlicht_int64,
  schemerlicht_void,
  schemerlicht_scm
  };

COMPILER_SCHEMERLICHT_API void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, const std::vector<external_type>& arguments, const char* help_text = nullptr);
COMPILER_SCHEMERLICHT_API void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, const char* help_text = nullptr);
COMPILER_SCHEMERLICHT_API void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, external_type arg1, const char* help_text = nullptr);
COMPILER_SCHEMERLICHT_API void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, external_type arg1, external_type arg2, const char* help_text = nullptr);
COMPILER_SCHEMERLICHT_API void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, external_type arg1, external_type arg2, external_type arg3, const char* help_text = nullptr);
COMPILER_SCHEMERLICHT_API void register_external_primitive(const std::string& name, void* func_ptr, external_type return_type, external_type arg1, external_type arg2, external_type arg3, external_type arg4, const char* help_text = nullptr);

class scm_type
  {
  public:
    COMPILER_SCHEMERLICHT_API scm_type();
    COMPILER_SCHEMERLICHT_API scm_type(uint64_t rax);
    COMPILER_SCHEMERLICHT_API ~scm_type();

    COMPILER_SCHEMERLICHT_API operator uint64_t() const { return scm_value; }

    COMPILER_SCHEMERLICHT_API bool is_fixnum() const;
    COMPILER_SCHEMERLICHT_API bool is_bool_true() const;
    COMPILER_SCHEMERLICHT_API bool is_bool_false() const;
    COMPILER_SCHEMERLICHT_API bool is_nil() const;
    COMPILER_SCHEMERLICHT_API bool is_char() const;
    COMPILER_SCHEMERLICHT_API bool is_undefined() const;
    COMPILER_SCHEMERLICHT_API bool is_scheme_quiet_undefined() const;
    COMPILER_SCHEMERLICHT_API bool is_eof() const;
    COMPILER_SCHEMERLICHT_API bool is_procedure() const;
    COMPILER_SCHEMERLICHT_API bool is_error() const;
    COMPILER_SCHEMERLICHT_API bool is_block() const;
    COMPILER_SCHEMERLICHT_API bool is_flonum() const;
    COMPILER_SCHEMERLICHT_API bool is_closure() const;
    COMPILER_SCHEMERLICHT_API bool is_pair() const;
    COMPILER_SCHEMERLICHT_API bool is_vector() const;
    COMPILER_SCHEMERLICHT_API bool is_string() const;
    COMPILER_SCHEMERLICHT_API bool is_symbol() const;
    COMPILER_SCHEMERLICHT_API bool is_port() const;
    COMPILER_SCHEMERLICHT_API bool is_promise() const;

    COMPILER_SCHEMERLICHT_API int64_t get_fixnum() const;
    COMPILER_SCHEMERLICHT_API double get_flonum() const;
    COMPILER_SCHEMERLICHT_API double get_number() const;
    COMPILER_SCHEMERLICHT_API std::string get_string() const;
    COMPILER_SCHEMERLICHT_API std::vector<scm_type> get_vector() const;
    COMPILER_SCHEMERLICHT_API std::pair<scm_type, scm_type> get_pair() const;
    COMPILER_SCHEMERLICHT_API std::vector<scm_type> get_list() const;
    COMPILER_SCHEMERLICHT_API double* flonum_address() const;
    COMPILER_SCHEMERLICHT_API std::string get_closure_name() const;

    COMPILER_SCHEMERLICHT_API uint64_t value() const { return scm_value; }
  private:
    uint64_t scm_value;
  };

COMPILER_SCHEMERLICHT_API scm_type make_fixnum(int64_t value);
COMPILER_SCHEMERLICHT_API scm_type make_nil();
COMPILER_SCHEMERLICHT_API scm_type make_true();
COMPILER_SCHEMERLICHT_API scm_type make_false();
COMPILER_SCHEMERLICHT_API scm_type make_undefined();
COMPILER_SCHEMERLICHT_API scm_type make_scheme_quiet_undefined();
COMPILER_SCHEMERLICHT_API scm_type make_flonum(double value);
COMPILER_SCHEMERLICHT_API scm_type make_pair(scm_type first, scm_type second);
COMPILER_SCHEMERLICHT_API scm_type make_list(const std::vector<scm_type>& lst);
COMPILER_SCHEMERLICHT_API scm_type make_vector(const std::vector<scm_type>& vec);
COMPILER_SCHEMERLICHT_API scm_type make_string(const std::string& s);

COMPILER_SCHEMERLICHT_API void set_prompt(const std::string& prompt_text);
COMPILER_SCHEMERLICHT_API void set_welcome_message(const std::string& welcome_message_text);
COMPILER_SCHEMERLICHT_API void set_help_text(const std::string& help_text);

COMPILER_SCHEMERLICHT_API void save_compiler_data();
COMPILER_SCHEMERLICHT_API void restore_compiler_data();


COMPILER_END
