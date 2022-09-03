#pragma once

#include "schemerlicht_api.h"

#include "namespace.h"
#include "vm/vmcode.h"

#include <map>
#include <set>
#include <stdint.h>
#include <vector>


COMPILER_BEGIN

class reg_alloc
  {
  public:
    COMPILER_SCHEMERLICHT_API reg_alloc(const std::vector<VM::vmcode::operand>& usable_registers, uint32_t number_of_locals);
    COMPILER_SCHEMERLICHT_API ~reg_alloc();

    bool free_register_available() const;
    bool free_local_available() const;
    
    VM::vmcode::operand get_next_available_register();
    void make_register_available(VM::vmcode::operand reg);

    uint32_t get_next_available_local();
    void make_local_available(uint32_t val);

    uint8_t number_of_registers();
    uint32_t number_of_locals();

    uint8_t number_of_available_registers();
    uint32_t number_of_available_locals();

    bool is_free_register(VM::vmcode::operand reg);
    bool is_free_local(uint32_t local_id);

    void make_all_available();

    uint32_t number_of_locals() const { return nr_locals; }

    void make_register_unavailable(VM::vmcode::operand reg);
    void make_local_unavailable(uint32_t val);

    const std::vector<VM::vmcode::operand>& registers() const { return available_registers; }

  private:
    std::vector<VM::vmcode::operand> available_registers;
    std::map<VM::vmcode::operand, uint8_t> register_to_index;
    uint32_t nr_locals; 
    std::set<uint32_t> free_locals;
    std::set<uint8_t> free_registers;
  };


COMPILER_END
