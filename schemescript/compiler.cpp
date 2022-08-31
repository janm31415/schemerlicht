#include "compiler.h"
#include "compile_error.h"
#include "context_defs.h"
#include "asm_aux.h"
#include "inlines.h"
#include "macro_data.h"
#include "preprocess.h"
#include "primitives.h"
#include "types.h"
#include "globals.h"
#include "primitives.h"
#include "inlines.h"
#include "cinput_data.h"
#include <sstream>

COMPILER_BEGIN

using namespace VM;


function_map generate_function_map()
  {
  function_map fm;
  fm.insert(std::pair<std::string, fun_ptr>("+", &compile_add));
  fm.insert(std::pair<std::string, fun_ptr>("-", &compile_sub));
  fm.insert(std::pair<std::string, fun_ptr>("*", &compile_mul));
  fm.insert(std::pair<std::string, fun_ptr>("/", &compile_div));
  fm.insert(std::pair<std::string, fun_ptr>("=", &compile_equal));
  fm.insert(std::pair<std::string, fun_ptr>("!=", &compile_not_equal));
  fm.insert(std::pair<std::string, fun_ptr>("<", &compile_less));
  fm.insert(std::pair<std::string, fun_ptr>("<=", &compile_leq));
  fm.insert(std::pair<std::string, fun_ptr>(">", &compile_greater));
  fm.insert(std::pair<std::string, fun_ptr>(">=", &compile_geq));
  fm.insert(std::pair<std::string, fun_ptr>("%allocate-symbol", &compile_allocate_symbol));
  fm.insert(std::pair<std::string, fun_ptr>("%slot-ref", &compile_slot_ref));
  fm.insert(std::pair<std::string, fun_ptr>("%slot-set!", &compile_slot_set));
  fm.insert(std::pair<std::string, fun_ptr>("%undefined", &compile_undefined));
  fm.insert(std::pair<std::string, fun_ptr>("%quiet-undefined", &compile_scheme_quiet_undefined));
  fm.insert(std::pair<std::string, fun_ptr>("add1", &compile_add1));
  fm.insert(std::pair<std::string, fun_ptr>("%apply", &compile_apply));
  fm.insert(std::pair<std::string, fun_ptr>("arithmetic-shift", &compile_arithmetic_shift));
  fm.insert(std::pair<std::string, fun_ptr>("assoc", &compile_assoc));
  fm.insert(std::pair<std::string, fun_ptr>("assq", &compile_assq));
  fm.insert(std::pair<std::string, fun_ptr>("assv", &compile_assv));
  fm.insert(std::pair<std::string, fun_ptr>("bitwise-and", &compile_bitwise_and));
  fm.insert(std::pair<std::string, fun_ptr>("bitwise-not", &compile_bitwise_not));
  fm.insert(std::pair<std::string, fun_ptr>("bitwise-or", &compile_bitwise_or));
  fm.insert(std::pair<std::string, fun_ptr>("bitwise-xor", &compile_bitwise_xor));
  fm.insert(std::pair<std::string, fun_ptr>("boolean?", &compile_is_boolean));
  fm.insert(std::pair<std::string, fun_ptr>("car", &compile_car));
  fm.insert(std::pair<std::string, fun_ptr>("cdr", &compile_cdr));
  fm.insert(std::pair<std::string, fun_ptr>("char?", &compile_is_char));
  fm.insert(std::pair<std::string, fun_ptr>("char=?", &compile_char_equal));
  fm.insert(std::pair<std::string, fun_ptr>("char<?", &compile_char_less));
  fm.insert(std::pair<std::string, fun_ptr>("char>?", &compile_char_greater));
  fm.insert(std::pair<std::string, fun_ptr>("char<=?", &compile_char_leq));
  fm.insert(std::pair<std::string, fun_ptr>("char>=?", &compile_char_geq));
  fm.insert(std::pair<std::string, fun_ptr>("char->fixnum", &compile_char_to_fixnum));
  fm.insert(std::pair<std::string, fun_ptr>("close-file", &compile_close_file));
  fm.insert(std::pair<std::string, fun_ptr>("closure", &compile_closure));
  fm.insert(std::pair<std::string, fun_ptr>("closure?", &compile_is_closure));
  fm.insert(std::pair<std::string, fun_ptr>("closure-ref", &compile_closure_ref));
  fm.insert(std::pair<std::string, fun_ptr>("cons", &compile_cons));
  fm.insert(std::pair<std::string, fun_ptr>("compare-strings", &compile_compare_strings));
  fm.insert(std::pair<std::string, fun_ptr>("compare-strings-ci", &compile_compare_strings_ci));
  fm.insert(std::pair<std::string, fun_ptr>("current-seconds", &compile_current_seconds));
  fm.insert(std::pair<std::string, fun_ptr>("current-milliseconds", &compile_current_milliseconds));
  fm.insert(std::pair<std::string, fun_ptr>("eof-object?", &compile_is_eof_object));
  fm.insert(std::pair<std::string, fun_ptr>("eq?", &compile_is_eq));
  fm.insert(std::pair<std::string, fun_ptr>("eqv?", &compile_is_eqv));
  fm.insert(std::pair<std::string, fun_ptr>("%eqv?", &compile_is_eqv_structurally));
  fm.insert(std::pair<std::string, fun_ptr>("equal?", &compile_is_equal));
  fm.insert(std::pair<std::string, fun_ptr>("%error", &compile_error));
  fm.insert(std::pair<std::string, fun_ptr>("%eval", &compile_eval));
  fm.insert(std::pair<std::string, fun_ptr>("file-exists?", &compile_file_exists));
  fm.insert(std::pair<std::string, fun_ptr>("fixnum?", &compile_is_fixnum));
  fm.insert(std::pair<std::string, fun_ptr>("fixnum->char", &compile_fixnum_to_char));
  fm.insert(std::pair<std::string, fun_ptr>("fixnum->flonum", &compile_fixnum_to_flonum));
  fm.insert(std::pair<std::string, fun_ptr>("fixnum-expt", &compile_fixnum_expt));
  fm.insert(std::pair<std::string, fun_ptr>("flonum?", &compile_is_flonum));
  fm.insert(std::pair<std::string, fun_ptr>("flonum->fixnum", &compile_flonum_to_fixnum));
  fm.insert(std::pair<std::string, fun_ptr>("flonum-expt", &compile_flonum_expt));
  fm.insert(std::pair<std::string, fun_ptr>("%flush-output-port", &compile_flush_output_port));
  fm.insert(std::pair<std::string, fun_ptr>("fx=?", &compile_fx_equal));
  fm.insert(std::pair<std::string, fun_ptr>("fx<?", &compile_fx_less));
  fm.insert(std::pair<std::string, fun_ptr>("fx>?", &compile_fx_greater));
  fm.insert(std::pair<std::string, fun_ptr>("fx<=?", &compile_fx_leq));
  fm.insert(std::pair<std::string, fun_ptr>("fx>=?", &compile_fx_geq));
  fm.insert(std::pair<std::string, fun_ptr>("fx+", &compile_fx_add));
  fm.insert(std::pair<std::string, fun_ptr>("fx-", &compile_fx_sub));
  fm.insert(std::pair<std::string, fun_ptr>("fx*", &compile_fx_mul));
  fm.insert(std::pair<std::string, fun_ptr>("fx/", &compile_fx_div));
  fm.insert(std::pair<std::string, fun_ptr>("fxadd1", &compile_fx_add1));
  fm.insert(std::pair<std::string, fun_ptr>("fxsub1", &compile_fx_sub1));
  fm.insert(std::pair<std::string, fun_ptr>("fxzero?", &compile_fx_is_zero));
  fm.insert(std::pair<std::string, fun_ptr>("getenv", &compile_getenv));
  fm.insert(std::pair<std::string, fun_ptr>("halt", &compile_halt));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-sign", &compile_ieee754_sign));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-exponent", &compile_ieee754_exponent));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-mantissa", &compile_ieee754_mantissa));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-sin", &compile_ieee754_sin));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-cos", &compile_ieee754_cos));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-tan", &compile_ieee754_tan));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-asin", &compile_ieee754_asin));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-acos", &compile_ieee754_acos));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-atan1", &compile_ieee754_atan1));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-log", &compile_ieee754_log));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-round", &compile_ieee754_round));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-sqrt", &compile_ieee754_sqrt));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-truncate", &compile_ieee754_truncate));
  fm.insert(std::pair<std::string, fun_ptr>("ieee754-pi", &compile_ieee754_pi));
  fm.insert(std::pair<std::string, fun_ptr>("input-port?", &compile_is_input_port));
  fm.insert(std::pair<std::string, fun_ptr>("load", &compile_load));
  fm.insert(std::pair<std::string, fun_ptr>("length", &compile_length));
  fm.insert(std::pair<std::string, fun_ptr>("list", &compile_list));
  fm.insert(std::pair<std::string, fun_ptr>("make-port", &compile_make_port));
  fm.insert(std::pair<std::string, fun_ptr>("%make-promise", &compile_make_promise));
  fm.insert(std::pair<std::string, fun_ptr>("make-string", &compile_make_string));
  fm.insert(std::pair<std::string, fun_ptr>("make-vector", &compile_make_vector));
  fm.insert(std::pair<std::string, fun_ptr>("max", &compile_max));
  fm.insert(std::pair<std::string, fun_ptr>("member", &compile_member));
  fm.insert(std::pair<std::string, fun_ptr>("memq", &compile_memq));
  fm.insert(std::pair<std::string, fun_ptr>("memv", &compile_memv));
  fm.insert(std::pair<std::string, fun_ptr>("min", &compile_min));
  fm.insert(std::pair<std::string, fun_ptr>("not", &compile_not));
  fm.insert(std::pair<std::string, fun_ptr>("null?", &compile_is_nil));
  fm.insert(std::pair<std::string, fun_ptr>("num2str", &compile_num2str));
  fm.insert(std::pair<std::string, fun_ptr>("open-file", &compile_open_file));
  fm.insert(std::pair<std::string, fun_ptr>("output-port?", &compile_is_output_port));
  fm.insert(std::pair<std::string, fun_ptr>("%peek-char", &compile_peek_char));
  fm.insert(std::pair<std::string, fun_ptr>("pair?", &compile_is_pair));
  fm.insert(std::pair<std::string, fun_ptr>("port?", &compile_is_port));
  fm.insert(std::pair<std::string, fun_ptr>("procedure?", &compile_is_procedure));
  fm.insert(std::pair<std::string, fun_ptr>("promise?", &compile_is_promise));
  fm.insert(std::pair<std::string, fun_ptr>("putenv", &compile_putenv));
  fm.insert(std::pair<std::string, fun_ptr>("quotient", &compile_quotient));
  fm.insert(std::pair<std::string, fun_ptr>("%read-char", &compile_read_char));
  fm.insert(std::pair<std::string, fun_ptr>("reclaim", &compile_reclaim));
  fm.insert(std::pair<std::string, fun_ptr>("reclaim-garbage", &compile_reclaim_garbage));
  fm.insert(std::pair<std::string, fun_ptr>("remainder", &compile_remainder));
  fm.insert(std::pair<std::string, fun_ptr>("set-car!", &compile_set_car));
  fm.insert(std::pair<std::string, fun_ptr>("set-cdr!", &compile_set_cdr));
  fm.insert(std::pair<std::string, fun_ptr>("str2num", &compile_str2num));
  fm.insert(std::pair<std::string, fun_ptr>("string", &compile_string));
  fm.insert(std::pair<std::string, fun_ptr>("string-hash", &compile_string_hash));
  fm.insert(std::pair<std::string, fun_ptr>("string?", &compile_is_string));
  fm.insert(std::pair<std::string, fun_ptr>("string-append1", &compile_string_append1));
  fm.insert(std::pair<std::string, fun_ptr>("string-copy", &compile_string_copy));
  fm.insert(std::pair<std::string, fun_ptr>("string-fill!", &compile_string_fill));
  fm.insert(std::pair<std::string, fun_ptr>("string-length", &compile_string_length));
  fm.insert(std::pair<std::string, fun_ptr>("string-ref", &compile_string_ref));
  fm.insert(std::pair<std::string, fun_ptr>("string-set!", &compile_string_set));
  fm.insert(std::pair<std::string, fun_ptr>("substring", &compile_substring));
  fm.insert(std::pair<std::string, fun_ptr>("symbol->string", &compile_symbol_to_string));
  fm.insert(std::pair<std::string, fun_ptr>("symbol?", &compile_is_symbol));
  fm.insert(std::pair<std::string, fun_ptr>("sub1", &compile_sub1));
  fm.insert(std::pair<std::string, fun_ptr>("vector", &compile_vector));
  fm.insert(std::pair<std::string, fun_ptr>("vector?", &compile_is_vector));
  fm.insert(std::pair<std::string, fun_ptr>("vector-fill!", &compile_vector_fill));
  fm.insert(std::pair<std::string, fun_ptr>("vector-length", &compile_vector_length));
  fm.insert(std::pair<std::string, fun_ptr>("vector-ref", &compile_vector_ref));
  fm.insert(std::pair<std::string, fun_ptr>("vector-set!", &compile_vector_set));
  fm.insert(std::pair<std::string, fun_ptr>("%write-char", &compile_write_char));
  fm.insert(std::pair<std::string, fun_ptr>("%write-string", &compile_write_string));
  fm.insert(std::pair<std::string, fun_ptr>("zero?", &compile_is_zero));
  return fm;
  }
  
namespace
  {
  struct registered_functions
    {
    const function_map* primitives;
    const function_map* inlined_primitives;
    const std::map<std::string, external_function>* externals;
    };

  inline bool is_inlined_primitive(const std::string& prim_name)
    {
    if (prim_name.size() > 2)
      {
      return (prim_name[0] == '#' && prim_name[1] == '#');
      }
    return false;
    }

  function_map generate_inlined_primitives()
    {
    function_map fm;
    /*
    primitive names with a prefix ## are the inlined versions.
    */
    fm.insert(std::pair<std::string, fun_ptr>("##remainder", &inline_remainder));
    fm.insert(std::pair<std::string, fun_ptr>("##quotient", &inline_quotient));
    fm.insert(std::pair<std::string, fun_ptr>("##arithmetic-shift", &inline_arithmetic_shift));
    fm.insert(std::pair<std::string, fun_ptr>("##%undefined", &inline_undefined));
    fm.insert(std::pair<std::string, fun_ptr>("##%quiet-undefined", &inline_scheme_quiet_undefined));
    fm.insert(std::pair<std::string, fun_ptr>("##vector-length", &inline_vector_length));
    fm.insert(std::pair<std::string, fun_ptr>("##fixnum->flonum", &inline_fixnum_to_flonum));
    fm.insert(std::pair<std::string, fun_ptr>("##flonum->fixnum", &inline_flonum_to_fixnum));
    fm.insert(std::pair<std::string, fun_ptr>("##fixnum->char", &inline_fixnum_to_char));
    fm.insert(std::pair<std::string, fun_ptr>("##char->fixnum", &inline_char_to_fixnum));
    fm.insert(std::pair<std::string, fun_ptr>("##char<?", &inline_fx_less));
    fm.insert(std::pair<std::string, fun_ptr>("##char<=?", &inline_fx_leq));
    fm.insert(std::pair<std::string, fun_ptr>("##char>?", &inline_fx_greater));
    fm.insert(std::pair<std::string, fun_ptr>("##char>=?", &inline_fx_geq));
    fm.insert(std::pair<std::string, fun_ptr>("##char=?", &inline_fx_equal));
    fm.insert(std::pair<std::string, fun_ptr>("##bitwise-and", &inline_bitwise_and));
    fm.insert(std::pair<std::string, fun_ptr>("##bitwise-not", &inline_bitwise_not));
    fm.insert(std::pair<std::string, fun_ptr>("##bitwise-or", &inline_bitwise_or));
    fm.insert(std::pair<std::string, fun_ptr>("##bitwise-xor", &inline_bitwise_xor));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-sign", &inline_ieee754_sign));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-mantissa", &inline_ieee754_mantissa));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-exponent", &inline_ieee754_exponent));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-pi", &inline_ieee754_pi));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxsin", &inline_ieee754_fxsin));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxcos", &inline_ieee754_fxcos));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxtan", &inline_ieee754_fxtan));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxasin", &inline_ieee754_fxasin));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxacos", &inline_ieee754_fxacos));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxatan1", &inline_ieee754_fxatan1));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxlog", &inline_ieee754_fxlog));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxround", &inline_ieee754_fxround));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxtruncate", &inline_ieee754_fxtruncate));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fxsqrt", &inline_ieee754_fxsqrt));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-flsin", &inline_ieee754_flsin));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-flcos", &inline_ieee754_flcos));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fltan", &inline_ieee754_fltan));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-flasin", &inline_ieee754_flasin));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-flacos", &inline_ieee754_flacos));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-flatan1", &inline_ieee754_flatan1));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fllog", &inline_ieee754_fllog));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-flround", &inline_ieee754_flround));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-fltruncate", &inline_ieee754_fltruncate));
    fm.insert(std::pair<std::string, fun_ptr>("##ieee754-flsqrt", &inline_ieee754_flsqrt));
    fm.insert(std::pair<std::string, fun_ptr>("##memq", &inline_memq));
    fm.insert(std::pair<std::string, fun_ptr>("##assq", &inline_assq));
    fm.insert(std::pair<std::string, fun_ptr>("##not", &inline_not));
    fm.insert(std::pair<std::string, fun_ptr>("##cons", &inline_cons));
    fm.insert(std::pair<std::string, fun_ptr>("##car", &inline_car));
    fm.insert(std::pair<std::string, fun_ptr>("##cdr", &inline_cdr));
    fm.insert(std::pair<std::string, fun_ptr>("##set-car!", &inline_set_car));
    fm.insert(std::pair<std::string, fun_ptr>("##set-cdr!", &inline_set_cdr));
    fm.insert(std::pair<std::string, fun_ptr>("##eq?", &inline_eq));
    fm.insert(std::pair<std::string, fun_ptr>("##fxadd1", &inline_fx_add1));
    fm.insert(std::pair<std::string, fun_ptr>("##fxsub1", &inline_fx_sub1));
    fm.insert(std::pair<std::string, fun_ptr>("##fxmax", &inline_fx_max));
    fm.insert(std::pair<std::string, fun_ptr>("##fxmin", &inline_fx_min));
    fm.insert(std::pair<std::string, fun_ptr>("##fxzero?", &inline_fx_is_zero));
    fm.insert(std::pair<std::string, fun_ptr>("##fx+", &inline_fx_add));
    fm.insert(std::pair<std::string, fun_ptr>("##fx-", &inline_fx_sub));
    fm.insert(std::pair<std::string, fun_ptr>("##fx*", &inline_fx_mul));
    fm.insert(std::pair<std::string, fun_ptr>("##fx/", &inline_fx_div));
    fm.insert(std::pair<std::string, fun_ptr>("##fx<?", &inline_fx_less));
    fm.insert(std::pair<std::string, fun_ptr>("##fx<=?", &inline_fx_leq));
    fm.insert(std::pair<std::string, fun_ptr>("##fx>?", &inline_fx_greater));
    fm.insert(std::pair<std::string, fun_ptr>("##fx>=?", &inline_fx_geq));
    fm.insert(std::pair<std::string, fun_ptr>("##fx=?", &inline_fx_equal));
    fm.insert(std::pair<std::string, fun_ptr>("##fx!=?", &inline_fx_not_equal));
    fm.insert(std::pair<std::string, fun_ptr>("##fladd1", &inline_fl_add1));
    fm.insert(std::pair<std::string, fun_ptr>("##flsub1", &inline_fl_sub1));
    fm.insert(std::pair<std::string, fun_ptr>("##flmax", &inline_fl_max));
    fm.insert(std::pair<std::string, fun_ptr>("##flmin", &inline_fl_min));
    fm.insert(std::pair<std::string, fun_ptr>("##flzero?", &inline_fl_is_zero));
    fm.insert(std::pair<std::string, fun_ptr>("##fl+", &inline_fl_add));
    fm.insert(std::pair<std::string, fun_ptr>("##fl*", &inline_fl_mul));
    fm.insert(std::pair<std::string, fun_ptr>("##fl/", &inline_fl_div));
    fm.insert(std::pair<std::string, fun_ptr>("##fl-", &inline_fl_sub));
    fm.insert(std::pair<std::string, fun_ptr>("##fl<?", &inline_fl_less));
    fm.insert(std::pair<std::string, fun_ptr>("##fl<=?", &inline_fl_leq));
    fm.insert(std::pair<std::string, fun_ptr>("##fl>?", &inline_fl_greater));
    fm.insert(std::pair<std::string, fun_ptr>("##fl>=?", &inline_fl_geq));
    fm.insert(std::pair<std::string, fun_ptr>("##fl=?", &inline_fl_equal));
    fm.insert(std::pair<std::string, fun_ptr>("##fl!=?", &inline_fl_not_equal));
    fm.insert(std::pair<std::string, fun_ptr>("##fixnum?", &inline_is_fixnum));
    fm.insert(std::pair<std::string, fun_ptr>("##flonum?", &inline_is_flonum));
    fm.insert(std::pair<std::string, fun_ptr>("##pair?", &inline_is_pair));
    fm.insert(std::pair<std::string, fun_ptr>("##vector?", &inline_is_vector));
    fm.insert(std::pair<std::string, fun_ptr>("##string?", &inline_is_string));
    fm.insert(std::pair<std::string, fun_ptr>("##symbol?", &inline_is_symbol));
    fm.insert(std::pair<std::string, fun_ptr>("##closure?", &inline_is_closure));
    fm.insert(std::pair<std::string, fun_ptr>("##procedure?", &inline_is_procedure));
    fm.insert(std::pair<std::string, fun_ptr>("##boolean?", &inline_is_boolean));
    fm.insert(std::pair<std::string, fun_ptr>("##null?", &inline_is_nil));
    fm.insert(std::pair<std::string, fun_ptr>("##eof-object?", &inline_is_eof_object));
    fm.insert(std::pair<std::string, fun_ptr>("##char?", &inline_is_char));
    fm.insert(std::pair<std::string, fun_ptr>("##promise?", &inline_is_promise));
    fm.insert(std::pair<std::string, fun_ptr>("##port?", &inline_is_port));
    fm.insert(std::pair<std::string, fun_ptr>("##input-port?", &inline_is_input_port));
    fm.insert(std::pair<std::string, fun_ptr>("##output-port?", &inline_is_output_port));
    return fm;
    }
    
  inline void compile_expression(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const Expression& expr, const primitive_map& pm, const compiler_options& options, vmcode::operand target = vmcode::RAX, bool expire_registers = true);

  struct get_scan_index_helper
    {
    uint64_t si;

    template <class F>
    void operator()(const F& i)
      {
      si = i.scan_index;
      }

    void operator()(const Literal& i)
      {
      get_scan_index_helper gsih;
      std::visit(gsih, i);
      si = gsih.si;
      }
    };

  inline uint64_t get_scan_index(const Expression& expr)
    {
    get_scan_index_helper gsih;
    std::visit(gsih, expr);
    return gsih.si;
    }

  void register_allocation_expire_old_intervals(uint64_t time_point, compile_data& data)
    {
    if (time_point < data.first_ra_map_time_point_to_elapse) // first quick check to see whether there are some registers to free
      return;
    uint64_t first_ra_map_time_point_to_elapse = (uint64_t)-1;
    auto it = data.ra_map.begin();
    while (it != data.ra_map.end())
      {
      reg_alloc_data rad = it->first;
      std::string var_name = it->second;
      if (rad.live_range.last <= time_point) // can be removed
        {
        if (rad.type == reg_alloc_data::t_register)
          data.ra->make_register_available(rad.reg);
        else
          data.ra->make_local_available(rad.local_id);
        it = data.ra_map.erase(it);
        }
      else
        {
        first_ra_map_time_point_to_elapse = std::min<uint64_t>(first_ra_map_time_point_to_elapse, rad.live_range.last);
        ++it;
        }
      }
    data.first_ra_map_time_point_to_elapse = first_ra_map_time_point_to_elapse;
    }

  inline bool target_is_valid(vmcode::operand target)
    {
    if (target == vmcode::R15)
      return false;
    if (target == vmcode::R13)
      return false;
    return true;
    }

  inline bool expression_can_be_targetted(const Expression& expr)
    {
    return std::holds_alternative<Variable>(expr) || std::holds_alternative<Literal>(expr);
    //return std::holds_alternative<Literal>(expr);
    }

  inline void compile_nop(vmcode& code, vmcode::operand target)
    {
    assert(target_is_valid(target));
    code.add(vmcode::MOV, target, vmcode::NUMBER, scheme_undefined);
    }

  inline void compile_fixnum(registered_functions&, environment_map&, repl_data&, compile_data&, vmcode& code, const Fixnum& f, const compiler_options&, vmcode::operand target)
    {
    assert(target_is_valid(target));
    code.add(vmcode::MOV, target, vmcode::NUMBER, int2fixnum(f.value));
    }

  inline void compile_flonum(registered_functions&, environment_map&, repl_data&, compile_data&, vmcode& code, const Flonum& f, const compiler_options& ops, vmcode::operand target)
    {
    assert(target_is_valid(target));
    if (ops.safe_primitives && ops.safe_flonums)
      {
      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2);
      check_heap(code, re_flonum_heap_overflow);
      }
    uint64_t header = make_block_header(1, T_FLONUM);
    uint64_t v = *reinterpret_cast<const uint64_t*>(&f.value);
    code.add(vmcode::MOV, vmcode::R15, ALLOC);
    code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
    code.add(vmcode::MOV, MEM_ALLOC, vmcode::NUMBER, header);
    code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::NUMBER, v);
    code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
    code.add(vmcode::MOV, target, vmcode::R15);
    }

  inline void compile_true(registered_functions&, environment_map&, repl_data&, compile_data&, vmcode& code, const compiler_options&, vmcode::operand target)
    {
    assert(target_is_valid(target));
    code.add(vmcode::MOV, target, vmcode::NUMBER, bool_t);
    }

  inline void compile_false(registered_functions&, environment_map&, repl_data&, compile_data&, vmcode& code, const compiler_options&, vmcode::operand target)
    {
    assert(target_is_valid(target));
    code.add(vmcode::MOV, target, vmcode::NUMBER, bool_f);
    }

  inline void compile_nil(registered_functions&, environment_map&, repl_data&, compile_data&, vmcode& code, const compiler_options&, vmcode::operand target)
    {
    assert(target_is_valid(target));
    code.add(vmcode::MOV, target, vmcode::NUMBER, nil);
    }

  inline void compile_character(registered_functions&, environment_map&, repl_data&, compile_data&, vmcode& code, const Character& c, const compiler_options&, vmcode::operand target)
    {
    assert(target_is_valid(target));
    int64_t i = int64_t(c.value) << 8;
    i |= char_tag;
    code.add(vmcode::MOV, target, vmcode::NUMBER, i);
    }

  inline void compile_string(registered_functions&, environment_map&, repl_data&, compile_data&, vmcode& code, const String& s, const compiler_options& ops, vmcode::operand target)
    {
    assert(target_is_valid(target));
    std::string str = s.value;
    int nr_of_args = (int)str.length();

    if (ops.safe_primitives)
      {
      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, ((nr_of_args >> 3) + 2));
      check_heap(code, re_string_heap_overflow);
      }

    code.add(vmcode::MOV, vmcode::R15, ALLOC);
    code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);


    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
    code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, ((nr_of_args >> 3) + 1));
    code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
    code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

    for (int i = 0; i < nr_of_args; ++i)
      {
      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)str[i]);
      code.add(vmcode::MOV8, MEM_ALLOC, vmcode::RAX);
      code.add(vmcode::INC, ALLOC);
      }
    code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
    int end = ((nr_of_args >> 3) + 1) << 3;
    for (int i = nr_of_args; i < end; ++i)
      {
      code.add(vmcode::MOV8, MEM_ALLOC, vmcode::RAX);
      code.add(vmcode::INC, ALLOC);
      }

    code.add(vmcode::MOV, target, vmcode::R15);
    }

  inline void compile_symbol(registered_functions&, environment_map&, repl_data&, compile_data&, vmcode& code, const Symbol& s, const compiler_options& ops, vmcode::operand target)
    {
    assert(target_is_valid(target));
    std::string str = s.value;
    int nr_of_args = (int)str.length();

    if (ops.safe_primitives)
      {
      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, ((nr_of_args >> 3) + 2));
      check_heap(code, re_symbol_heap_overflow);
      }

    code.add(vmcode::MOV, vmcode::R15, ALLOC);
    code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);


    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)symbol_tag << (uint64_t)block_shift);
    code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, ((nr_of_args >> 3) + 1));
    code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
    code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

    for (int i = 0; i < nr_of_args; ++i)
      {
      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)str[i]);
      code.add(vmcode::MOV8, MEM_ALLOC, vmcode::RAX);
      code.add(vmcode::INC, ALLOC);
      }
    code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
    int end = ((nr_of_args >> 3) + 1) << 3;
    for (int i = nr_of_args; i < end; ++i)
      {
      code.add(vmcode::MOV8, MEM_ALLOC, vmcode::RAX);
      code.add(vmcode::INC, ALLOC);
      }

    code.add(vmcode::MOV, target, vmcode::R15);
    }

  inline void compile_literal(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const Literal& lit, const compiler_options& options, vmcode::operand target)
    {
    assert(target_is_valid(target));
    if (std::holds_alternative<Fixnum>(lit))
      {
      compile_fixnum(fns, env, rd, data, code, std::get<Fixnum>(lit), options, target);
      }
    else if (std::holds_alternative<True>(lit))
      {
      compile_true(fns, env, rd, data, code, options, target);
      }
    else if (std::holds_alternative<False>(lit))
      {
      compile_false(fns, env, rd, data, code, options, target);
      }
    else if (std::holds_alternative<Nil>(lit))
      {
      compile_nil(fns, env, rd, data, code, options, target);
      }
    else if (std::holds_alternative<Character>(lit))
      {
      compile_character(fns, env, rd, data, code, std::get<Character>(lit), options, target);
      }
    else if (std::holds_alternative<Flonum>(lit))
      {
      compile_flonum(fns, env, rd, data, code, std::get<Flonum>(lit), options, target);
      }
    else if (std::holds_alternative<String>(lit))
      {
      compile_string(fns, env, rd, data, code, std::get<String>(lit), options, target);
      }
    else if (std::holds_alternative<Symbol>(lit))
      {
      compile_symbol(fns, env, rd, data, code, std::get<Symbol>(lit), options, target);
      }
    else
      throw_error(not_implemented);
    }
    

  void compile_if(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& cd, vmcode& code, const If& i, const primitive_map& pm, const compiler_options& ops)
    {
    assert(i.arguments.size() == 3);
    if (i.arguments.size() != 3)
      throw_error(i.line_nr, i.column_nr, i.filename, invalid_number_of_arguments);
    compile_expression(fns, env, rd, cd, code, i.arguments.front(), pm, ops);
    uint64_t lab1 = label++;
    uint64_t lab2 = label++;
    code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, bool_f);
    code.add(vmcode::JE, label_to_string(lab1));
    compile_expression(fns, env, rd, cd, code, i.arguments[1], pm, ops);
    code.add(vmcode::JMP, label_to_string(lab2));
    code.add(vmcode::LABEL, label_to_string(lab1));
    compile_expression(fns, env, rd, cd, code, i.arguments[2], pm, ops);
    code.add(vmcode::LABEL, label_to_string(lab2));
    }

  void compile_funcall(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& cd, vmcode& code, const FunCall& fun, const primitive_map& pm, const compiler_options& ops)
    {
    const auto& arg_reg = get_argument_registers();

    for (size_t i = 0; i < fun.arguments.size(); ++i)
      {
      std::stringstream str;
      str << i + 1;
      code.add(vmcode::COMMENT, "push function arg " + str.str() + " to stack");
      compile_expression(fns, env, rd, cd, code, fun.arguments[i], pm, ops);
      //code.add(vmcode::PUSH, vmcode::RAX);
      push(code, vmcode::RAX);
      }

    code.add(vmcode::COMMENT, "compute function");
    compile_expression(fns, env, rd, cd, code, fun.fun.front(), pm, ops); // the function itself should be evaluated after its arguments. Not clear why, but bugs otherwise.

    auto no_closure = label_to_string(label++);
    std::string error;
    code.add(vmcode::COMMENT, "check whether function is actually closure or primitive object");
    jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R15, no_closure);
    if (ops.safe_primitives)
      {
      error = label_to_string(label++);
      code.add(vmcode::MOV, vmcode::R11, vmcode::RAX);
      code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
      jump_if_arg_does_not_point_to_closure(code, vmcode::R11, vmcode::R15, error);
      }
    code.add(vmcode::COMMENT, "save function in rcx as self");
    code.add(vmcode::MOV, arg_reg[0], vmcode::RAX);
    for (int i = (int)fun.arguments.size() - 1; i >= 0; --i)
      {
      int j = i + 1;
      std::stringstream str;
      str << j;
      code.add(vmcode::COMMENT, "get function arg " + str.str() + " from stack");
      if (j >= arg_reg.size())
        {
        uint32_t local_id = (uint32_t)(j - (int)arg_reg.size());
        //code.add(vmcode::POP, vmcode::RAX);
        pop(code, vmcode::RAX);
        if (local_id >= cd.local_stack_size)
          throw_error(too_many_locals);
        save_to_local(code, local_id);
        }
      else
        {
        const auto& reg = arg_reg[j];
        //code.add(vmcode::POP, reg);
        pop(code, reg);
        }
      }

    // now is the moment to call garbage collection
    // all arguments are in the registers or locals
    // there are no other local variables due to cps conversion
    if (ops.garbage_collection)
      {
      std::string garbage_error = label_to_string(label++);
      std::string continue_label = label_to_string(label++);
      code.add(vmcode::CMP, ALLOC, LIMIT);
      code.add(vmcode::JLE, continue_label);
      code.add(vmcode::CMP, ALLOC, FROM_SPACE_END);
      code.add(vmcode::JGE, garbage_error);
      code.add(vmcode::COMMENT, "jump to reclaim-garbage");

      auto pm_it = pm.find("reclaim-garbage");
      if (pm_it == pm.end())
        throw_error(primitive_unknown, "reclaim-garbage");

      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, pm_it->second.address);
      code.add(vmcode::MOV, CONTINUE, vmcode::LABELADDRESS, continue_label);
      code.add(vmcode::JMP, vmcode::RAX);
      error_label(code, garbage_error, re_heap_full);
      code.add(vmcode::LABEL, continue_label);
      }

    code.add(vmcode::COMMENT, "number of arguments in r11");
    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, fun.arguments.size() + 1); // can be removed possibly, if reclaim keeps r11 untouched
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RAX, CELLS(1));
    code.add(vmcode::COMMENT, "jump to closure label");
    code.add(vmcode::JMP, vmcode::R15);

    code.add(vmcode::LABEL, no_closure);
    // RAX contains address to primitive procedure
    if (ops.safe_primitives)
      {
      code.add(vmcode::MOV, vmcode::R15, vmcode::RAX);
      code.add(vmcode::AND, vmcode::R15, vmcode::NUMBER, procedure_mask);
      code.add(vmcode::CMP, vmcode::R15, vmcode::NUMBER, procedure_tag);
      code.add(vmcode::JNE, error);
      }

    code.add(vmcode::MOV, vmcode::R15, vmcode::RAX);
    code.add(vmcode::AND, vmcode::R15, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);

    // skip first argument, as this is a continuation
    for (int i = (int)fun.arguments.size() - 1; i >= 1; --i)
      {
      int j = i - 1;
      if (j >= arg_reg.size())
        {
        uint32_t local_id = (uint32_t)(j - (int)arg_reg.size());
        //code.add(vmcode::POP, vmcode::RAX);
        pop(code, vmcode::RAX);
        if (local_id >= cd.local_stack_size)
          throw_error(too_many_locals);
        save_to_local(code, local_id, vmcode::RAX, vmcode::R11);
        }
      else
        {
        const auto& reg = arg_reg[j];
        //code.add(vmcode::POP, reg);
        pop(code, reg);
        }
      }

    auto continue_primitive = label_to_string(label++);
    code.add(vmcode::MOV, CONTINUE, vmcode::LABELADDRESS, continue_primitive);
    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, fun.arguments.size() - 1);
    code.add(vmcode::JMP, vmcode::R15); // call primitive
    code.add(vmcode::LABEL, continue_primitive);

    code.add(vmcode::COMMENT, "pop continuation object after prim object call");
    //code.add(vmcode::POP, vmcode::RCX); // get closure
    pop(code, vmcode::RCX);
    code.add(vmcode::MOV, vmcode::RDX, vmcode::RAX);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);
    if (ops.safe_primitives)
      {
      jump_if_arg_is_not_block(code, vmcode::RAX, vmcode::R11, error);
      }
    code.add(vmcode::AND, vmcode::RAX, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
    if (ops.safe_primitives)
      {
      jump_if_arg_does_not_point_to_closure(code, vmcode::RAX, vmcode::R11, error);
      }
    code.add(vmcode::MOV, vmcode::R15, vmcode::MEM_RAX, CELLS(1));
    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, 2);
    code.add(vmcode::COMMENT, "call continuation object after prim object call");
    code.add(vmcode::JMP, vmcode::R15);

    if (ops.safe_primitives)
      {
      error_label(code, error, re_closure_expected);
      }
    }

  void compile_variable_arity_rest_arg(vmcode& code, const compiler_options& ops, size_t arg_pos)
    {
    std::string not_empty = label_to_string(label++);
    std::string done = label_to_string(label++);
    std::string done2 = label_to_string(label++);
    std::string repeat = label_to_string(label++);
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0);
    code.add(vmcode::JG, not_empty);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, nil);
    code.add(vmcode::JMP, CONTINUE);
    code.add(vmcode::LABEL, not_empty);
    code.add(vmcode::PUSH, vmcode::RCX);
    code.add(vmcode::PUSH, vmcode::RDX);
    if (ops.safe_primitives)
      {
      code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
      code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
      code.add(vmcode::ADD, vmcode::RAX, vmcode::R11); // shl 1 and add for * 3
      check_heap(code, re_list_heap_overflow);
      }
    code.add(vmcode::MOV, vmcode::R15, ALLOC);
    code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);

    uint64_t header = make_block_header(2, T_PAIR);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);

    code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);

    if (arg_pos < get_argument_registers().size())
      {
      code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), get_argument_registers()[arg_pos]);
      }
    else
      {
      load_local(code, arg_pos - get_argument_registers().size(), vmcode::RDX, vmcode::RCX);
      code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RDX);
      }
    code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 1);
    code.add(vmcode::JE, done);

    int max_iter = 6;
    for (int i = 1; i < max_iter; ++i)
      {
      code.add(vmcode::MOV, vmcode::RCX, ALLOC);
      code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
      code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
      code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
      code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
      code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);

      if ((arg_pos + i) < get_argument_registers().size())
        {
        code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), get_argument_registers()[arg_pos + i]);
        }
      else
        {
        load_local(code, arg_pos + i - get_argument_registers().size(), vmcode::RDX, vmcode::RCX);
        code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RDX);
        }
      code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, i + 1);
      code.add(vmcode::JE, done);
      }

    code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, max_iter);

    code.add(vmcode::PUSH, vmcode::RSI);

    code.add(vmcode::MOV, vmcode::RDX, LOCAL);
    code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS((arg_pos + max_iter - get_argument_registers().size())));
    code.add(vmcode::LABEL, repeat);
    code.add(vmcode::MOV, vmcode::RSI, vmcode::MEM_RDX);

    code.add(vmcode::MOV, vmcode::RCX, ALLOC);
    code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(3));
    code.add(vmcode::OR, vmcode::RCX, vmcode::NUMBER, block_tag);
    code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RCX);
    code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
    code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
    code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RSI);
    code.add(vmcode::ADD, vmcode::RDX, vmcode::NUMBER, CELLS(1));
    code.add(vmcode::DEC, vmcode::R11);
    code.add(vmcode::JE, done2);
    code.add(vmcode::JMP, repeat);

    code.add(vmcode::LABEL, done2);
    code.add(vmcode::POP, vmcode::RSI);

    code.add(vmcode::LABEL, done);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, nil);
    code.add(vmcode::MOV, MEM_ALLOC, CELLS(2), vmcode::RAX);
    code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(3));
    code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
    code.add(vmcode::POP, vmcode::RDX);
    code.add(vmcode::POP, vmcode::RCX);
    code.add(vmcode::JMP, CONTINUE);
    }

  void compile_lambda(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& cd, vmcode& code, const Lambda& lam, const primitive_map& pm, const compiler_options& ops)
    {
    compile_data new_cd = create_compile_data(cd.heap_size, cd.globals_stack, cd.ra->number_of_locals(), cd.p_ctxt);
    new_cd.halt_label = cd.halt_label;
    new_cd.ra->make_all_available();
    code.push();
    auto lab = label_to_string(label++);
    code.add(vmcode::LABEL_ALIGNED, lab);
    environment_map new_env = std::make_shared<environment<environment_entry>>(env);
    for (size_t i = 0; i < lam.variables.size(); ++i)
      {
      const std::string& var_name = lam.variables[i];
      if (i < get_argument_registers().size())
        {
        environment_entry e;
        e.st = environment_entry::st_register;
        e.pos = (uint64_t)get_argument_registers()[i];
        e.live_range = lam.live_ranges[i];
        new_env->push(var_name, e);
        new_cd.ra->make_register_unavailable(get_argument_registers()[i]);
        new_cd.ra_map[make_reg_alloc_data(reg_alloc_data::t_register, e.pos, lam.live_ranges[i])] = var_name;
        new_cd.first_ra_map_time_point_to_elapse = std::min<uint64_t>(new_cd.first_ra_map_time_point_to_elapse, lam.live_ranges[i].last);
        }
      else
        {
        size_t j = i - get_argument_registers().size();
        environment_entry e;
        e.st = environment_entry::st_local;
        e.pos = (uint64_t)j;
        e.live_range = lam.live_ranges[i];
        new_env->push(var_name, e);
        new_cd.ra->make_local_unavailable((uint32_t)j);
        new_cd.ra_map[make_reg_alloc_data(reg_alloc_data::t_local, e.pos, lam.live_ranges[i])] = var_name;
        new_cd.first_ra_map_time_point_to_elapse = std::min<uint64_t>(new_cd.first_ra_map_time_point_to_elapse, lam.live_ranges[i].last);
        }
      }
    std::string error;
    if (ops.safe_primitives)
      {
      error = label_to_string(label++);
      uint64_t nr_of_args_necessary = lam.variables.size();
      if (lam.variable_arity && nr_of_args_necessary)
        --nr_of_args_necessary;
      code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, nr_of_args_necessary);
      code.add(vmcode::JL, error);
      }
    if (lam.variable_arity)
      {
      auto cont = label_to_string(label++);
      code.add(vmcode::MOV, CONTINUE, vmcode::LABELADDRESS, cont);
      size_t arg_pos = lam.variables.size() - 1;

      // R11 = self + cps + real vars + rest where rest counts as 1 => rest = R11 - self - cps - real vars = R11 - (lam.variables.size() - 1)
      code.add(vmcode::SUB, vmcode::R11, vmcode::NUMBER, lam.variables.size() - 1);
      compile_variable_arity_rest_arg(code, ops, arg_pos);
      code.add(vmcode::LABEL, cont);
      if (arg_pos < get_argument_registers().size())
        {
        code.add(vmcode::MOV, get_argument_registers()[arg_pos], vmcode::RAX);
        }
      else
        {
        size_t j = arg_pos - get_argument_registers().size();
        save_to_local(code, j);
        }
      }
    if (lam.body.size() != 1)
      throw_error(lam.line_nr, lam.column_nr, lam.filename, bad_syntax);
    compile_expression(fns, new_env, rd, new_cd, code, lam.body.front(), pm, ops);
    code.add(vmcode::RET);
    if (ops.safe_primitives)
      {
      error_label(code, error, re_lambda_invalid_number_of_arguments);
      }
    code.pop();
    code.add(vmcode::MOV, vmcode::RAX, vmcode::LABELADDRESS, lab);
    //code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, procedure_tag);
    }

  void compile_set(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& cd, vmcode& code, const Set& s, const primitive_map& pm, const compiler_options& ops)
    {
    if (s.originates_from_define || s.originates_from_quote)
      {
      compile_expression(fns, env, rd, cd, code, s.value.front(), pm, ops);

      environment_entry e;
      if (!env->find(e, s.name))
        throw_error(s.line_nr, s.column_nr, s.filename, invalid_variable_name);

      code.add(vmcode::MOV, vmcode::R15, GLOBALS);
      code.add(vmcode::MOV, vmcode::MEM_R15, e.pos, vmcode::RAX);

      // We don't need to update the environment with the new global variable or quote, as this is
      // already done by the passes global_define_env and quote_conversion.
      // We do this in the passes so that function names are already known in the body of a recursive function.
      }
    else //set! in global namespace, so we are changing the value of a global variable
      {
      environment_entry e;
      if (!env->find(e, s.name))
        throw_error(s.line_nr, s.column_nr, s.filename, invalid_variable_name);

      //set! in global namespace
      compile_expression(fns, env, rd, cd, code, s.value.front(), pm, ops);

      code.add(vmcode::MOV, vmcode::R15, GLOBALS);
      code.add(vmcode::MOV, vmcode::MEM_R15, e.pos, vmcode::RAX);

      }
    }

  void add_global_variable_to_debug_info(vmcode& code, uint64_t pos, const compiler_options& ops)
    {
    if (ops.keep_variable_stack)
      {
      code.add(vmcode::COMMENT, "Start debug info");
      code.add(vmcode::COMMENT, "\tCycling the call stack");
      for (int i = SCHEME_VARIABLE_DEBUG_STACK_SIZE - 2; i >= 0; --i) // move items on the stack further down
        {
        code.add(vmcode::MOV, vmcode::R15, LAST_GLOBAL_VARIABLE_USED + CELLS(i));
        code.add(vmcode::MOV, LAST_GLOBAL_VARIABLE_USED + CELLS(i + 1), vmcode::R15);
        }
      code.add(vmcode::COMMENT, "\tAdding the latest item to the call stack");
      code.add(vmcode::MOV, LAST_GLOBAL_VARIABLE_USED, vmcode::NUMBER, pos); // push last item on the stack
      code.add(vmcode::COMMENT, "Stop debug info");
      }
    }

  void compile_variable(registered_functions&, environment_map& env, repl_data& rd, compile_data& cd, vmcode& code, const Variable& var, const compiler_options& ops, vmcode::operand target)
    {
    assert(target_is_valid(target));
    environment_entry e;
    if (!env->find(e, var.name))
      {
      /*
      This variable was not found in the environment. However: it is possible that this variable will be defined by a load at runtime.
      Therefore we now allocate a global position for this variable. It might be filled in later by load.
      */

      std::string name_before_alpha_conversion = get_variable_name_before_alpha(var.name);

      rd.alpha_conversion_env->push(name_before_alpha_conversion, var.name);
      environment_entry ne;
      ne.st = environment_entry::st_global;
      ne.pos = rd.global_index * 8;
      ++(rd.global_index);
      env->push_outer(var.name, ne);
      uint64_t* addr = cd.p_ctxt->globals + (ne.pos >> 3);
      *addr = unresolved_tag; // This is a new address, previously equal to unalloc_tag. To avoid that gc stops here when cleaning, we change its value to unresolved_tag.

      code.add(vmcode::MOV, vmcode::R15, GLOBALS);
      code.add(vmcode::MOV, target, vmcode::MEM_R15, ne.pos);
      add_global_variable_to_debug_info(code, ne.pos, ops);
      }
    else
      {
      switch (e.st)
        {
        case environment_entry::st_local:
          load_local(code, e.pos, target, vmcode::R15); break;
        case environment_entry::st_register:
          if (target != (vmcode::operand)e.pos)
            code.add(vmcode::MOV, target, (vmcode::operand)e.pos); break;
        case environment_entry::st_global:
          code.add(vmcode::MOV, vmcode::R15, GLOBALS);
          code.add(vmcode::MOV, target, vmcode::MEM_R15, e.pos);
          add_global_variable_to_debug_info(code, e.pos, ops);
          break;
        }
      }
    }

  std::vector<vmcode::operand> get_inlined_args()
    {
    std::vector<vmcode::operand> vec;
    vec.push_back(vmcode::RAX);
    vec.push_back(vmcode::RBX);
    //vec.push_back(vmcode::R11);
    return vec;
    }

  void compile_prim_call_inlined(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const PrimitiveCall& prim, const primitive_map& pm, const compiler_options& options)
    {
    assert(is_inlined_primitive(prim.primitive_name));

    static std::vector<vmcode::operand> args = get_inlined_args();

    int nr_prim_args = (int)prim.arguments.size();

    assert(nr_prim_args <= args.size());

    if (nr_prim_args == 1)
      {
      compile_expression(fns, env, rd, data, code, prim.arguments.back(), pm, options);
      }
    else if (nr_prim_args > 1)
      {
      bool expressions_can_be_targetted = options.fast_expression_targetting;
      for (const auto& expr : prim.arguments)
        expressions_can_be_targetted &= expression_can_be_targetted(expr);

      if (expressions_can_be_targetted)
        {
        for (int i = nr_prim_args - 1; i >= 0; --i)
          compile_expression(fns, env, rd, data, code, prim.arguments[i], pm, options, args[i], false); // since we are going backwards, we cannot expire intervals. It is safe to go backwards, as the expressions are simple
        }
      else
        {
        if (!prim.arguments.empty())
          {
          for (size_t i = 0; i < nr_prim_args - 1; ++i)
            {
            compile_expression(fns, env, rd, data, code, prim.arguments[i], pm, options);
            //code.add(vmcode::PUSH, vmcode::RAX);
            push(code, vmcode::RAX);
            }
          compile_expression(fns, env, rd, data, code, prim.arguments.back(), pm, options);
          code.add(vmcode::MOV, args[nr_prim_args - 1], vmcode::RAX);
          for (int i = nr_prim_args - 2; i >= 0; --i)
            {
            //code.add(vmcode::POP, args[i]);
            pop(code, args[i]);
            }
          }
        }
      }

    auto inlined_pm_it = fns.inlined_primitives->find(prim.primitive_name);
    if (inlined_pm_it == fns.inlined_primitives->end())
      throw_error(prim.line_nr, prim.column_nr, prim.filename, primitive_unknown, prim.primitive_name);
    code.add(vmcode::COMMENT, "inlining " + inlined_pm_it->first);

    inlined_pm_it->second(code, options);
    }

  void compile_prim_call_not_inlined(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const PrimitiveCall& prim, const primitive_map& pm, const compiler_options& options)
    {
    assert(!is_inlined_primitive(prim.primitive_name));

    const auto& arg_reg = get_argument_registers();
    int nr_prim_args = (int)prim.arguments.size();
    /*
    First we check whether the registers and locals for arguments are free.
    If they are not free we first push them onto the stack, and pop them at the end.
    */
    std::vector<uint32_t> locals_that_were_not_free;
    std::vector<vmcode::operand> registers_that_were_not_free;
    for (int i = 0; i < nr_prim_args; ++i)
      {
      if (i >= (int)arg_reg.size())
        {
        uint32_t local_id = (uint32_t)(i - (int)arg_reg.size());
        if (local_id >= data.local_stack_size)
          throw_error(too_many_locals);
        if (!data.ra->is_free_local(local_id))
          {
          locals_that_were_not_free.push_back(local_id);
          load_local(code, local_id);
          //code.add(vmcode::PUSH, vmcode::RAX); //possibly one call here? todo
          push(code, vmcode::RAX);
          }
        }
      else
        {
        const auto& reg = arg_reg[i];
        if (!data.ra->is_free_register(reg))
          {
          registers_that_were_not_free.push_back(reg);
          //code.add(vmcode::PUSH, reg);
          push(code, reg);
          }
        }
      }

    bool expressions_can_be_targetted = options.fast_expression_targetting;
    for (const auto& expr : prim.arguments)
      expressions_can_be_targetted &= expression_can_be_targetted(expr);
    expressions_can_be_targetted &= (prim.arguments.size() <= arg_reg.size());
    if (expressions_can_be_targetted && !prim.arguments.empty()) // check whether no overlap with variables
      {
      for (int i = 0; i < nr_prim_args; ++i)
        {
        const auto& arg = prim.arguments[i];
        if (std::holds_alternative<Variable>(arg))
          {
          const Variable& var = std::get<Variable>(arg);
          environment_entry e;
          if (env->find(e, var.name) && (e.st == environment_entry::st_register))
            {
            if (e.pos < (uint64_t)arg_reg[i])
              {
              expressions_can_be_targetted = false;
              }
            }
          }
        }
      }

    if (expressions_can_be_targetted)
      {
      // we choose this order of evaluation as it has most likelyhood to not interact with variables saved in register (I think)
      for (int i = 0; i < prim.arguments.size(); ++i)
        compile_expression(fns, env, rd, data, code, prim.arguments[i], pm, options, arg_reg[i], false);
      }
    else
      {
      if (nr_prim_args > 0)
        {
        for (int i = 0; i < nr_prim_args - 1; ++i)
          {
          compile_expression(fns, env, rd, data, code, prim.arguments[i], pm, options);
          //code.add(vmcode::PUSH, vmcode::RAX);
          push(code, vmcode::RAX);
          }
        compile_expression(fns, env, rd, data, code, prim.arguments.back(), pm, options);
        int i = (int)prim.arguments.size() - 1;
        if (i >= arg_reg.size())
          {
          uint32_t local_id = (uint32_t)(i - (int)arg_reg.size());
          if (local_id >= data.local_stack_size)
            throw_error(too_many_locals);
          save_to_local(code, local_id);
          }
        else
          {
          const auto& reg = arg_reg[i];
          code.add(vmcode::MOV, reg, vmcode::RAX);
          }
        }

      for (int i = nr_prim_args - 2; i >= 0; --i)
        {
        if (i >= arg_reg.size())
          {
          uint32_t local_id = (uint32_t)(i - (int)arg_reg.size());
          if (local_id >= data.local_stack_size)
            throw_error(too_many_locals);
          //code.add(vmcode::POP, vmcode::RAX);
          pop(code, vmcode::RAX);
          save_to_local(code, local_id);
          }
        else
          {
          const auto& reg = arg_reg[i];
          //code.add(vmcode::POP, reg);
          pop(code, reg);
          }
        }
      }

    // here call primitive

    std::string continue_label = label_to_string(label++);

    if (prim.primitive_name == "halt")
      {
      if (options.garbage_collection)
        {
        code.add(vmcode::COMMENT, "clean all locals for gc");
        //Here we clear all our registers, so that they can be cleaned up by the gc
        code.add(vmcode::MOV, vmcode::R15, LOCAL);
        code.add(vmcode::MOV, vmcode::R11, vmcode::R15);
        code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, CELLS(data.local_stack_size));
        auto repeat_clean_locals = label_to_string(label++);
        code.add(vmcode::LABEL, repeat_clean_locals);
        code.add(vmcode::MOV, vmcode::MEM_R15, vmcode::NUMBER, unalloc_tag);
        code.add(vmcode::ADD, vmcode::R15, vmcode::NUMBER, 8);
        code.add(vmcode::CMP, vmcode::R15, vmcode::R11);
        code.add(vmcode::JL, repeat_clean_locals);
        code.add(vmcode::COMMENT, "clean all registers for gc");
        code.add(vmcode::MOV, vmcode::RDX, vmcode::NUMBER, unalloc_tag);
        code.add(vmcode::MOV, vmcode::RSI, vmcode::NUMBER, unalloc_tag);
        code.add(vmcode::MOV, vmcode::RDI, vmcode::NUMBER, unalloc_tag);
        code.add(vmcode::MOV, vmcode::R8, vmcode::NUMBER, unalloc_tag);
        code.add(vmcode::MOV, vmcode::R9, vmcode::NUMBER, unalloc_tag);
        code.add(vmcode::MOV, vmcode::R12, vmcode::NUMBER, unalloc_tag);
        code.add(vmcode::MOV, vmcode::R14, vmcode::NUMBER, unalloc_tag);
        std::string garbage_error = label_to_string(label++);
        std::string continue_label2 = label_to_string(label++);
        code.add(vmcode::CMP, ALLOC, LIMIT);
        code.add(vmcode::JLE, continue_label2);
        code.add(vmcode::CMP, ALLOC, FROM_SPACE_END);
        code.add(vmcode::JGE, garbage_error);
        code.add(vmcode::COMMENT, "jump to reclaim-garbage");
        auto pm_it = pm.find("reclaim-garbage");
        if (pm_it == pm.end())
          throw_error(primitive_unknown, "reclaim-garbage");
        code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, pm_it->second.address);
        code.add(vmcode::MOV, CONTINUE, vmcode::LABELADDRESS, continue_label2);
        code.add(vmcode::JMP, vmcode::RAX);
        error_label(code, garbage_error, re_heap_full);
        code.add(vmcode::LABEL, continue_label2);

        }
      code.add(vmcode::MOV, CONTINUE, vmcode::LABELADDRESS, data.halt_label);
      }
    else
      code.add(vmcode::MOV, CONTINUE, vmcode::LABELADDRESS, continue_label);

    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, prim.arguments.size());

    auto pm_it = pm.find(prim.primitive_name);
    if (pm_it == pm.end())
      throw_error(prim.line_nr, prim.column_nr, prim.filename, primitive_unknown, prim.primitive_name);
    code.add(vmcode::COMMENT, "jump to " + pm_it->first);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, pm_it->second.address);
    code.add(vmcode::JMP, vmcode::RAX);

    code.add(vmcode::LABEL, continue_label);

    /*
    Restore the registers and locals that were not free in reverse order
    */
    for (auto it = locals_that_were_not_free.rbegin(); it != locals_that_were_not_free.rend(); ++it)
      {
      uint32_t local_id = *it;
      if (local_id >= data.local_stack_size)
        throw_error(too_many_locals);
      //code.add(vmcode::POP, vmcode::R11);
      pop(code, vmcode::R11);
      save_to_local(code, local_id, vmcode::R11, vmcode::R15);
      }
    for (auto it = registers_that_were_not_free.rbegin(); it != registers_that_were_not_free.rend(); ++it)
      {
      //code.add(vmcode::POP, *it);
      pop(code, *it);
      }
    }

  inline void compile_prim_call(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const PrimitiveCall& prim, const primitive_map& pm, const compiler_options& options)
    {
    if (is_inlined_primitive(prim.primitive_name))
      compile_prim_call_inlined(fns, env, rd, data, code, prim, pm, options);
    else
      compile_prim_call_not_inlined(fns, env, rd, data, code, prim, pm, options);
    }

  inline void compile_prim_object(registered_functions&, environment_map& env, repl_data&, compile_data&, vmcode& code, const PrimitiveCall& prim, const primitive_map&, const compiler_options& ops)
    {
    environment_entry e;
    if (!env->find(e, prim.primitive_name))
      {
      throw_error(prim.line_nr, prim.column_nr, prim.filename, primitive_unknown, prim.primitive_name);
      }
    code.add(vmcode::MOV, vmcode::R15, GLOBALS);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_R15, e.pos);
    add_global_variable_to_debug_info(code, e.pos, ops);
    }

  inline void compile_prim(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const PrimitiveCall& prim, const primitive_map& pm, const compiler_options& options)
    {
    /*
    if (prim.as_object)
      compile_prim_object(fns, env, rd, data, code, prim, pm, options);
    else
      compile_prim_call(fns, env, rd, data, code, prim, pm, options);
    */
    if (prim.as_object)
      {
      environment_entry e;
      if (!env->find(e, prim.primitive_name))
        {
        throw_error(prim.line_nr, prim.column_nr, prim.filename, primitive_unknown, prim.primitive_name);
        }
      code.add(vmcode::MOV, vmcode::R15, GLOBALS);
      code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_R15, e.pos);
      add_global_variable_to_debug_info(code, e.pos, options);
      }
    else
      {
      if (is_inlined_primitive(prim.primitive_name))
        compile_prim_call_inlined(fns, env, rd, data, code, prim, pm, options);
      else
        compile_prim_call_not_inlined(fns, env, rd, data, code, prim, pm, options);
      }
    }

  std::vector<vmcode::operand> get_windows_calling_registers()
    {
    std::vector<vmcode::operand> calling_registers;
    calling_registers.push_back(vmcode::RCX);
    calling_registers.push_back(vmcode::RDX);
    calling_registers.push_back(vmcode::R8);
    calling_registers.push_back(vmcode::R9);
    return calling_registers;
    }

  std::vector<vmcode::operand> get_floating_point_registers()
    {
    std::vector<vmcode::operand> calling_registers;
    calling_registers.push_back(vmcode::XMM0);
    calling_registers.push_back(vmcode::XMM1);
    calling_registers.push_back(vmcode::XMM2);
    calling_registers.push_back(vmcode::XMM3);
    return calling_registers;
    }

  void compile_foreign_call(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const ForeignCall& foreign, const primitive_map& pm, const compiler_options& ops)
    {
    std::string error;
    if (ops.safe_primitives)
      error = label_to_string(label++);


    static std::vector<vmcode::operand> arg_reg = get_windows_calling_registers();
    static std::vector<vmcode::operand> arg_float_reg = get_floating_point_registers();


    if (foreign.arguments.size() > arg_reg.size())
      throw_error(foreign.line_nr, foreign.column_nr, foreign.filename, not_implemented);

    //auto ext_it = std::find_if(fns.externals->begin(), fns.externals->end(), [&](const external_function& f) { return f.name == foreign.foreign_name; });
    auto ext_it = fns.externals->find(foreign.foreign_name);
    if (ext_it == fns.externals->end())
      throw_error(foreign.line_nr, foreign.column_nr, foreign.filename, foreign_call_unknown, foreign.foreign_name);
    const external_function& ext = ext_it->second;

    if (ext.arguments.size() < foreign.arguments.size())
      throw_error(foreign.line_nr, foreign.column_nr, foreign.filename, invalid_number_of_arguments, foreign.foreign_name);

    /*
    First we check whether the registers are free.
    If they are not free we first push them onto the stack, and pop them at the end.
    */
    std::vector<vmcode::operand> registers_that_were_not_free;
    for (size_t i = 0; i < foreign.arguments.size(); ++i)
      {
      const auto& reg = arg_reg[i];
      if (!data.ra->is_free_register(reg))
        {
        registers_that_were_not_free.push_back(reg);
        //code.add(vmcode::PUSH, reg);
        push(code, reg);
        }
      }

    for (size_t i = 0; i < foreign.arguments.size(); ++i)
      {
      compile_expression(fns, env, rd, data, code, foreign.arguments[i], pm, ops);
      //code.add(vmcode::PUSH, vmcode::RAX);
      push(code, vmcode::RAX);
      }

    for (int i = (int)foreign.arguments.size() - 1; i >= 0; --i)
      {
      switch (ext.arguments[i])
        {
        case external_function::T_BOOL:
        {

        auto reg = arg_reg[i];
        pop(code, reg);
        code.add(vmcode::CMP, reg, vmcode::NUMBER, bool_f);
        code.add(vmcode::SETNE, vmcode::RAX);
        code.add(vmcode::MOV, reg, vmcode::RAX);
        break;
        }
        case external_function::T_INT64:
        {

        auto reg = arg_reg[i];
        //code.add(vmcode::POP, reg);
        pop(code, reg);
        if (ops.safe_primitives)
          {
          code.add(vmcode::TEST, reg, vmcode::NUMBER, 1);
          code.add(vmcode::JNE, error);
          }
        code.add(vmcode::SHR, reg, vmcode::NUMBER, 1);
        break;
        }
        case external_function::T_SCM:
        {
        auto reg = arg_reg[i];
        //code.add(vmcode::POP, reg);
        pop(code, reg);
        break;
        }
        case external_function::T_DOUBLE:
        {
        auto reg = arg_float_reg[i];
        //code.add(vmcode::POP, vmcode::R11);
        pop(code, vmcode::R11);
        if (ops.safe_primitives)
          jump_if_arg_is_not_block(code, vmcode::R11, vmcode::R15, error);
        code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
        if (ops.safe_primitives)
          jump_if_arg_does_not_point_to_flonum(code, vmcode::R11, vmcode::R15, error);
        code.add(vmcode::MOV, reg, vmcode::MEM_R11, CELLS(1));
        break;
        }
        case external_function::T_CHAR_POINTER:
        {
        auto reg = arg_reg[i];
        //code.add(vmcode::POP, vmcode::R11);
        pop(code, vmcode::R11);
        if (ops.safe_primitives)
          jump_if_arg_is_not_block(code, vmcode::R11, vmcode::R15, error);
        code.add(vmcode::AND, vmcode::R11, vmcode::NUMBER, 0xFFFFFFFFFFFFFFF8);
        if (ops.safe_primitives)
          jump_if_arg_does_not_point_to_string(code, vmcode::R11, vmcode::R15, error);
        code.add(vmcode::ADD, vmcode::R11, vmcode::NUMBER, CELLS(1));
        code.add(vmcode::MOV, reg, vmcode::R11);
        break;
        }
        default: throw_error(foreign.line_nr, foreign.column_nr, foreign.filename, not_implemented);
        }
      }


    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, ext.address);
    code.add(vmcode::CALLEXTERNAL, vmcode::R11);

    switch (ext.return_type)
      {
      case external_function::T_BOOL:
      {
      code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 3);
      code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, bool_f);
      break;
      }
      case external_function::T_INT64:
      {
      code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
      break;
      }
      case external_function::T_SCM:
      {
      break;
      }
      case external_function::T_DOUBLE:
      {
      if (ops.safe_primitives && ops.safe_flonums)
        {
        code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 2);
        check_heap(code, re_flonum_heap_overflow);
        }
      uint64_t header = make_block_header(1, T_FLONUM);
      code.add(vmcode::MOV, vmcode::R11, ALLOC);
      code.add(vmcode::OR, vmcode::R11, vmcode::NUMBER, block_tag);
      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
      code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
      code.add(vmcode::MOV, vmcode::RAX, vmcode::XMM0);
      code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RAX);
      code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
      code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
      break;
      }
      case external_function::T_CHAR_POINTER:
      {
      auto done = label_to_string(label++);
      auto repeat = label_to_string(label++);
      code.add(vmcode::PUSH, vmcode::RCX);
      code.add(vmcode::MOV, vmcode::RCX, vmcode::RAX); // char pointer in rcx
      code.add(vmcode::MOV, vmcode::R15, vmcode::RAX); // char pointer in r15
      code.add(vmcode::XOR, vmcode::R11, vmcode::R11); // r11 is counter for string length, initialize to 0
      code.add(vmcode::LABEL, repeat);
      code.add(vmcode::MOV8, vmcode::RAX, vmcode::MEM_R15); // get first character in al
      code.add(vmcode::CMP8, vmcode::RAX, vmcode::NUMBER, 0); // is zero?
      code.add(vmcode::JE, done);
      code.add(vmcode::INC, vmcode::R15);
      code.add(vmcode::INC, vmcode::R11);
      code.add(vmcode::JMP, repeat);
      code.add(vmcode::LABEL, done);

      if (ops.safe_primitives)
        {
        code.add(vmcode::MOV, vmcode::RAX, vmcode::R11);
        code.add(vmcode::SHR, vmcode::RAX, vmcode::NUMBER, 3);
        code.add(vmcode::ADD, vmcode::RAX, vmcode::NUMBER, 2);
        check_heap(code, re_string_heap_overflow);
        }

      code.add(vmcode::SHR, vmcode::R11, vmcode::NUMBER, 3);
      code.add(vmcode::INC, vmcode::R11);

      code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)string_tag << (uint64_t)block_shift);
      code.add(vmcode::OR, vmcode::RAX, vmcode::R11); // header is now correct in rax

      code.add(vmcode::MOV, vmcode::R15, ALLOC);
      code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);

      code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX); // save header
      code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));

      auto copy_done = label_to_string(label++);
      auto copy_repeat = label_to_string(label++);

      code.add(vmcode::LABEL, copy_repeat);
      code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RCX);
      code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);
      code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(1));
      code.add(vmcode::CMP, vmcode::R11, vmcode::NUMBER, 0); // is zero?
      code.add(vmcode::JE, copy_done);
      code.add(vmcode::DEC, vmcode::R11);
      code.add(vmcode::ADD, vmcode::RCX, vmcode::NUMBER, CELLS(1));
      code.add(vmcode::JMP, copy_repeat);
      code.add(vmcode::LABEL, copy_done);

      code.add(vmcode::MOV, vmcode::RAX, vmcode::R15);
      code.add(vmcode::POP, vmcode::RCX);
      break;
      }
      case external_function::T_VOID:
      {
      code.add(vmcode::XOR, vmcode::RAX, vmcode::RAX);
      break;
      }
      default: throw_error(foreign.line_nr, foreign.column_nr, foreign.filename, not_implemented);
      }
    /*
    Restore the registers that were not free in reverse order
    */
    for (auto it = registers_that_were_not_free.rbegin(); it != registers_that_were_not_free.rend(); ++it)
      {
      //code.add(vmcode::POP, *it);
      pop(code, *it);
      }

    if (ops.safe_primitives)
      {
      auto skip_error = label_to_string(label++);
      code.add(vmcode::JMP, skip_error);
      error_label(code, error, re_foreign_call_contract_violation);
      code.add(vmcode::LABEL, skip_error);
      }
    }

  void compile_let(registered_functions& fns, environment_map env, repl_data& rd, compile_data& data, vmcode& code, const Let& let, const primitive_map& pm, const compiler_options& options)
    {
    const Let* p_let = &let;
    while (p_let) // for nested let statements we avoid extra function calls to avoid stack usage. Nested lets can occur if you called remove_single_begin during preprocessing.
      {
      auto new_env = std::make_shared<environment<environment_entry>>(env);
      for (int i = 0; i < p_let->bindings.size(); ++i)
        {
        compile_expression(fns, env, rd, data, code, p_let->bindings[i].second, pm, options);
        environment_entry e;
        e.live_range = p_let->live_ranges[i];
        if (data.ra->free_register_available())
          {
          e.st = environment_entry::st_register;
          e.pos = (uint64_t)data.ra->get_next_available_register();
          code.add(vmcode::MOV, (vmcode::operand)e.pos, vmcode::RAX);
          data.ra_map[make_reg_alloc_data(reg_alloc_data::t_register, e.pos, p_let->live_ranges[i])] = p_let->bindings[i].first;
          data.first_ra_map_time_point_to_elapse = std::min<uint64_t>(data.first_ra_map_time_point_to_elapse, p_let->live_ranges[i].last);
          }
        else if (data.ra->free_local_available())
          {
          e.st = environment_entry::st_local;
          e.pos = (uint64_t)data.ra->get_next_available_local();
          save_to_local(code, e.pos);
          data.ra_map[make_reg_alloc_data(reg_alloc_data::t_local, e.pos, p_let->live_ranges[i])] = p_let->bindings[i].first;
          data.first_ra_map_time_point_to_elapse = std::min<uint64_t>(data.first_ra_map_time_point_to_elapse, p_let->live_ranges[i].last);
          }
        else
          {
          throw std::runtime_error("no local storage available anymore");
          }

        new_env->push(p_let->bindings[i].first, e);
        }
      if (std::holds_alternative<Let>(p_let->body.front())) // if this is a nested let
        {
        p_let = &std::get<Let>(p_let->body.front());
        env = new_env;
        }
      else
        {
        compile_expression(fns, new_env, rd, data, code, p_let->body.front(), pm, options);
        p_let = nullptr;
        }
      }
    }

  void compile_begin(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const Begin& beg, const primitive_map& pm, const compiler_options& options)
    {
    for (const auto& expr : beg.arguments)
      {
      compile_expression(fns, env, rd, data, code, expr, pm, options);
      }
    }
    
    
  inline void compile_expression(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const Expression& expr, const primitive_map& pm, const compiler_options& options, vmcode::operand target, bool expire_registers)
    {
    assert(target_is_valid(target));
    if (std::holds_alternative<Literal>(expr))
      {
      compile_literal(fns, env, rd, data, code, std::get<Literal>(expr), options, target);
      }
    else if (std::holds_alternative<Begin>(expr))
      {
      compile_begin(fns, env, rd, data, code, std::get<Begin>(expr), pm, options);
      }
    else if (std::holds_alternative<Let>(expr))
      {
      compile_let(fns, env, rd, data, code, std::get<Let>(expr), pm, options);
      }
    else if (std::holds_alternative<PrimitiveCall>(expr))
      {
      compile_prim(fns, env, rd, data, code, std::get<PrimitiveCall>(expr), pm, options);
      }
    else if (std::holds_alternative<ForeignCall>(expr))
      {
      compile_foreign_call(fns, env, rd, data, code, std::get<ForeignCall>(expr), pm, options);
      }
    else if (std::holds_alternative<Variable>(expr))
      {
      compile_variable(fns, env, rd, data, code, std::get<Variable>(expr), options, target);
      }
    else if (std::holds_alternative<If>(expr))
      {
      compile_if(fns, env, rd, data, code, std::get<If>(expr), pm, options);
      }
    else if (std::holds_alternative<Set>(expr))
      {
      compile_set(fns, env, rd, data, code, std::get<Set>(expr), pm, options);
      }
    else if (std::holds_alternative<Lambda>(expr))
      {
      compile_lambda(fns, env, rd, data, code, std::get<Lambda>(expr), pm, options);
      }
    else if (std::holds_alternative<FunCall>(expr))
      {
      compile_funcall(fns, env, rd, data, code, std::get<FunCall>(expr), pm, options);
      }
    else if (std::holds_alternative<Nop>(expr))
      {
      compile_nop(code, target);
      }
    else
      throw_error(not_implemented);
    if (expire_registers)
      register_allocation_expire_old_intervals(get_scan_index(expr), data);
    }
    
  void compile_program(registered_functions& fns, environment_map& env, repl_data& rd, compile_data& data, vmcode& code, const Program& prog, const primitive_map& pm, const compiler_options& options)
    {
    for (const auto& expr : prog.expressions)
      {
      if (std::holds_alternative<Begin>(expr))
        {
        for (const auto& expr2 : std::get<Begin>(expr).arguments)
          {
          data.halt_label = label_to_string(label++);
          compile_expression(fns, env, rd, data, code, expr2, pm, options);
          code.add(vmcode::LABEL, data.halt_label);
          data.ra->make_all_available();
          data.ra_map.clear();
          data.first_ra_map_time_point_to_elapse = (uint64_t)-1;
          }
        }
      else
        {
        data.halt_label = label_to_string(label++);
        compile_expression(fns, env, rd, data, code, expr, pm, options);
        code.add(vmcode::LABEL, data.halt_label);
        }
      }
    code.add(vmcode::JMP, "L_finish");
    }
  
  
  void compile_cinput_parameters(cinput_data& cinput, environment_map& env, vmcode& code)
    {
    for (int j = 0; j < (int)cinput.parameters.size(); ++j)
      {
      if (cinput.parameters[j].second == cinput_data::cin_int)
        {
        std::string name = cinput.parameters[j].first;

        environment_entry e;
        if (!env->find(e, name) || e.st != environment_entry::st_global)
          throw_error(invalid_c_input_syntax);
        code.add(vmcode::MOV, vmcode::RAX, GLOBALS);
        if (j == 0)
          {
          code.add(vmcode::SHL, vmcode::RDX, vmcode::NUMBER, 1);
          code.add(vmcode::MOV, vmcode::MEM_RAX, e.pos, vmcode::RDX);
          }
        else if (j == 1)
          {
          code.add(vmcode::SHL, vmcode::R8, vmcode::NUMBER, 1);
          code.add(vmcode::MOV, vmcode::MEM_RAX, e.pos, vmcode::R8);
          }
        else if (j == 2)
          {
          code.add(vmcode::SHL, vmcode::R9, vmcode::NUMBER, 1);
          code.add(vmcode::MOV, vmcode::MEM_RAX, e.pos, vmcode::R9);
          }
        else
          {
          int addr = j - 3;
          code.add(vmcode::MOV, vmcode::RCX, vmcode::MEM_RSP, (40 + addr * 8));
          code.add(vmcode::SHL, vmcode::RCX, vmcode::NUMBER, 1);
          code.add(vmcode::MOV, vmcode::MEM_RAX, e.pos, vmcode::RCX);
          }
        }
      else if (cinput.parameters[j].second == cinput_data::cin_double)
        {
        std::string name = cinput.parameters[j].first;

        environment_entry e;
        if (!env->find(e, name) || e.st != environment_entry::st_global)
          throw_error(invalid_c_input_syntax);

        uint64_t header = make_block_header(1, T_FLONUM);
        code.add(vmcode::MOV, vmcode::R15, ALLOC);
        code.add(vmcode::OR, vmcode::R15, vmcode::NUMBER, block_tag);
        code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, header);
        code.add(vmcode::MOV, MEM_ALLOC, vmcode::RAX);

        if (j == 0)
          code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM1);
        else if (j == 1)
          code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM2);
        else if (j == 2)
          code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::XMM3);
        else
          {
          int addr = j - 3;
          code.add(vmcode::MOV, vmcode::RAX, vmcode::MEM_RSP, (40 + addr * 8));
          code.add(vmcode::MOV, MEM_ALLOC, CELLS(1), vmcode::RAX);
          }

        code.add(vmcode::ADD, ALLOC, vmcode::NUMBER, CELLS(2));
        code.add(vmcode::MOV, vmcode::RAX, GLOBALS);
        code.add(vmcode::MOV, vmcode::MEM_RAX, e.pos, vmcode::R15);
        }
      }
    }
  
  }
  
  

void compile(environment_map& env, repl_data& rd, macro_data& md, context& ctxt, VM::vmcode& code, Program& prog, const primitive_map& pm, const std::map<std::string, external_function>& external_functions, const compiler_options& options)
  {
  static function_map prims = generate_function_map();
  static function_map inlined_prims = generate_inlined_primitives();
  registered_functions fns;
  fns.primitives = &prims;
  fns.inlined_primitives = &inlined_prims;
  fns.externals = &external_functions;
  
  cinput_data cinput;
  
  preprocess(env, rd, md, ctxt, cinput, prog, pm, options);
  label = 0;

  compile_data data = create_compile_data(ctxt.total_heap_size, ctxt.globals_end - ctxt.globals, (uint32_t)ctxt.number_of_locals, &ctxt);
  
  data.ra->make_all_available();
  data.ra_map.clear();
  data.first_ra_map_time_point_to_elapse = (uint64_t)-1;
  
  if (rd.global_index > data.globals_stack) // too many globals declared
    throw_error(too_many_globals);
    
    
  if constexpr (rsp_offset)
    code.add(VM::vmcode::SUB, VM::vmcode::RSP, VM::vmcode::NUMBER, rsp_offset);
    
  code.add(vmcode::MOV, CONTEXT, CALLING_CONVENTION_INT_PAR_1);
  
  /*
  Save the current content of the registers in the context
  */
  store_registers(code);
  
  code.add(vmcode::MOV, ERROR, vmcode::LABELADDRESS, "L_error");
  code.add(vmcode::MOV, STACK_REGISTER, STACK); // get the stack location from the context and put it in the dedicated register
  code.add(vmcode::MOV, STACK_SAVE, STACK_REGISTER);  // save the current stack position. At the end of this method we'll restore STACK to STACK_SAVE, as scheme
                                                       // does not pop every stack position that was pushed due to continuation passing style.

  code.add(vmcode::MOV, ALLOC, ALLOC_SAVED);
  
  compile_cinput_parameters(cinput, env, code);
  
  /*
  Make registers point to unalloc_tag, so that gc cannot crash due to old register content
  */
  code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, unalloc_tag);
  code.add(vmcode::MOV, vmcode::RDX, vmcode::NUMBER, unalloc_tag);
  code.add(vmcode::MOV, vmcode::RSI, vmcode::NUMBER, unalloc_tag);
  code.add(vmcode::MOV, vmcode::RDI, vmcode::NUMBER, unalloc_tag);
  code.add(vmcode::MOV, vmcode::R8, vmcode::NUMBER, unalloc_tag);
  code.add(vmcode::MOV, vmcode::R9, vmcode::NUMBER, unalloc_tag);
  code.add(vmcode::MOV, vmcode::R12, vmcode::NUMBER, unalloc_tag);
  code.add(vmcode::MOV, vmcode::R14, vmcode::NUMBER, unalloc_tag);
  
  /*Call the main procedure*/
  code.add(vmcode::JMP, "L_scheme_entry");

  code.add(vmcode::LABEL, "L_error");

  code.add(vmcode::LABEL, "L_finish");

  code.add(vmcode::MOV, ALLOC_SAVED, ALLOC);

  code.add(vmcode::MOV, STACK_REGISTER, STACK_SAVE); // restore the scheme stack to its saved position
  code.add(vmcode::MOV, STACK, STACK_REGISTER);
  
  
  /*Restore the registers to their original state*/
  load_registers(code);
  
  if constexpr (rsp_offset)
    code.add(VM::vmcode::ADD, VM::vmcode::RSP, VM::vmcode::NUMBER, rsp_offset);
  code.add(VM::vmcode::RET);
  
  /*Compile the main procedure*/
  code.push();
  code.add(vmcode::GLOBAL, "L_scheme_entry");
  compile_program(fns, env, rd, data, code, prog, pm, options);
  if (options.do_cps_conversion)
    {
    code.add(vmcode::COMMENT, "error: invalid program termination");
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, (uint64_t)re_invalid_program_termination);
    code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 8);
    code.add(vmcode::OR, vmcode::RAX, vmcode::NUMBER, error_tag);
    code.add(vmcode::JMP, "L_error");
    }
  else
    code.add(vmcode::JMP, "L_finish");
  code.pop();
  }

COMPILER_END
