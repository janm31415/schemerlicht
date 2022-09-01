#include "peephole.h"

#include <set>
#include <cassert>

VM_BEGIN

namespace
  {
  VM::vmcode::operand get_register(VM::vmcode::operand reg)
    {
    switch (reg)
      {
      case VM::vmcode::MEM_RAX: return VM::vmcode::RAX;
      case VM::vmcode::MEM_RBX: return VM::vmcode::RBX;
      case VM::vmcode::MEM_RCX: return VM::vmcode::RCX;
      case VM::vmcode::MEM_RDX: return VM::vmcode::RDX;
      case VM::vmcode::MEM_RDI: return VM::vmcode::RDI;
      case VM::vmcode::MEM_RSI: return VM::vmcode::RSI;
      case VM::vmcode::MEM_RSP: return VM::vmcode::RSP;
      case VM::vmcode::MEM_RBP: return VM::vmcode::RBP;
      case VM::vmcode::MEM_R8: return VM::vmcode::R8;
      case VM::vmcode::MEM_R9: return VM::vmcode::R9;
      case VM::vmcode::MEM_R10: return VM::vmcode::R10;
      case VM::vmcode::MEM_R11: return VM::vmcode::R11;
      case VM::vmcode::MEM_R12: return VM::vmcode::R12;
      case VM::vmcode::MEM_R13: return VM::vmcode::R13;
      case VM::vmcode::MEM_R14: return VM::vmcode::R14;
      case VM::vmcode::MEM_R15: return VM::vmcode::R15;
      }
    return reg;
    }

  VM::vmcode::operand get_memory_register(VM::vmcode::operand reg)
    {
    switch (reg)
      {
      case VM::vmcode::RAX: return VM::vmcode::MEM_RAX;
      case VM::vmcode::RBX: return VM::vmcode::MEM_RBX;
      case VM::vmcode::RCX: return VM::vmcode::MEM_RCX;
      case VM::vmcode::RDX: return VM::vmcode::MEM_RDX;
      case VM::vmcode::RDI: return VM::vmcode::MEM_RDI;
      case VM::vmcode::RSI: return VM::vmcode::MEM_RSI;
      case VM::vmcode::RSP: return VM::vmcode::MEM_RSP;
      case VM::vmcode::RBP: return VM::vmcode::MEM_RBP;
      case VM::vmcode::R8: return VM::vmcode::MEM_R8;
      case VM::vmcode::R9: return VM::vmcode::MEM_R9;
      case VM::vmcode::R10: return VM::vmcode::MEM_R10;
      case VM::vmcode::R11: return VM::vmcode::MEM_R11;
      case VM::vmcode::R12: return VM::vmcode::MEM_R12;
      case VM::vmcode::R13: return VM::vmcode::MEM_R13;
      case VM::vmcode::R14: return VM::vmcode::MEM_R14;
      case VM::vmcode::R15: return VM::vmcode::MEM_R15;
      }
    return reg;
    }

  bool operand_is_temp(VM::vmcode::operand op, const std::vector<vmcode::operand>& temporary_operands)
    {
    return std::find(temporary_operands.begin(), temporary_operands.end(), op) != temporary_operands.end();
    }

  bool operation_does_not_modify_first_operand(VM::vmcode::operation op)
    {
    switch (op)
      {
      case VM::vmcode::CMP:
      case VM::vmcode::TEST:
        return true;
      default: return false;
      }
    }

  bool operation_on_temp_register_can_be_done_on_target_register(VM::vmcode::instruction instr)
    {
    switch (instr.oper)
      {
      case VM::vmcode::XOR:
      case VM::vmcode::XORPD:
        return instr.operand1 == instr.operand2 && instr.operand1_mem == instr.operand2_mem;
      case VM::vmcode::MOV:
      case VM::vmcode::CVTSI2SD:
      case VM::vmcode::CVTTSD2SI:
      case VM::vmcode::SETE:
      case VM::vmcode::SETNE:
      case VM::vmcode::SETL:
      case VM::vmcode::SETG:
      case VM::vmcode::SETLE:
      case VM::vmcode::SETGE:
        return true;
      default:
        return false;
      }
    return false;
    }

  bool is_operation_that_sets_first_register_without_side_effects(VM::vmcode::operation op)
    {
    switch (op)
      {
      case VM::vmcode::MOV:
      case VM::vmcode::CVTSI2SD:
      case VM::vmcode::CVTTSD2SI:
        return true;
      }
    return false;
    }

  std::vector<VM::vmcode::instruction>::iterator peephole_mov_1(std::vector<VM::vmcode::instruction>::iterator it, std::vector<VM::vmcode::instruction>& vec, const std::vector<vmcode::operand>& temporary_operands)
    {
    if (it->oper != VM::vmcode::MOV)
      return it;
    if (it->operand1_mem != 0)
      return it;
    if (!operand_is_temp(it->operand1, temporary_operands))
      return it;
    auto it2 = it;
    ++it2;
    if (it2 == vec.end())
      return it;
    if (it2->operand2_mem == 0 && it2->operand2 == it->operand1)
      {
      // we have the case
      //   mov tmp_reg, expr
      //   op  target_reg, tmp_reg
      // which can be replaced by
      //   op  target_reg, expr
      it2->operand2 = it->operand2;
      it2->operand2_mem = it->operand2_mem;
      it = vec.erase(it);
      }
    else if (it2->operand1_mem == 0 && it2->operand1 == it->operand1)
      {
      // we have the case
      //   mov tmp_reg, expr
      //   op  tmp_reg, other_reg
      // which can be replaced by
      //   op  expr, other_reg
      // if op is an operation that does not change expr.
      if (operation_does_not_modify_first_operand(it2->oper))
        {
        it2->operand1 = it->operand2;
        it2->operand1_mem = it->operand2_mem;
        it = vec.erase(it);
        }
      }
    return it;
    }

  std::vector<VM::vmcode::instruction>::iterator peephole_mov_2(std::vector<VM::vmcode::instruction>::iterator it, std::vector<VM::vmcode::instruction>& vec, const std::vector<vmcode::operand>& temporary_operands)
    {
    if (it->oper != VM::vmcode::MOV)
      return it;
    if (it->operand2_mem != 0)
      return it;
    if (!operand_is_temp(it->operand2, temporary_operands))
      return it;
    if (it == vec.begin())
      return it;
    auto it2 = it;
    --it2;
    if (it2->operand1_mem == 0 && it2->operand1 == it->operand2 && operation_on_temp_register_can_be_done_on_target_register(*it2))
      {
      // we have the case
      //   op  tmp_reg, expr
      //   mov other_reg, tmp_reg
      // which can be replaced by
      //   op  other_reg, expr
      // if op is an operation that does not use the value in tmp_reg.
      it2->operand1_mem = it->operand1_mem;
      it2->operand1 = it->operand1;
      if (it2->operand2_mem == 0 && it2->operand2 == it->operand2)
        {
        it2->operand2_mem = it->operand1_mem;
        it2->operand2 = it->operand1;
        }
      it = vec.erase(it);
      }
    return it;
    }

  std::vector<VM::vmcode::instruction>::iterator peephole_mov_3(std::vector<VM::vmcode::instruction>::iterator it, std::vector<VM::vmcode::instruction>& vec)
    {
    if (it->oper != VM::vmcode::MOV)
      return it;
    auto it2 = it;
    ++it2;
    if (it2 == vec.end())
      return it;
    if (it2->oper == VM::vmcode::MOV && it->operand1 == it2->operand1 && it->operand1_mem == it2->operand1_mem)
      {
      it = vec.erase(it); // two moves that move to the same spot, so remove the first move.
      }
    return it;
    }

  bool operations_duplicate_has_no_effect(VM::vmcode::instruction instr)
    {
    switch (instr.oper)
      {
      case VM::vmcode::AND:
      case VM::vmcode::OR:
      case VM::vmcode::CMP:
      case VM::vmcode::CMPEQPD:
      case VM::vmcode::CMPLTPD:
      case VM::vmcode::CMPLEPD:
      case VM::vmcode::TEST:
      case VM::vmcode::MOV:
      case VM::vmcode::SETE:
      case VM::vmcode::SETNE:
      case VM::vmcode::SETL:
      case VM::vmcode::SETG:
      case VM::vmcode::SETLE:
      case VM::vmcode::SETGE:
      case VM::vmcode::CVTSI2SD:
      case VM::vmcode::CVTTSD2SI:
      case VM::vmcode::CABS:
      case VM::vmcode::JMP:
      case VM::vmcode::JA:
      case VM::vmcode::JB:
      case VM::vmcode::JE:
      case VM::vmcode::JL:
      case VM::vmcode::JLE:
      case VM::vmcode::JG:
      case VM::vmcode::JGE:
      case VM::vmcode::JNE:
        return true;
      default:
        return false;
      }
    return false;
    }

  std::vector<VM::vmcode::instruction>::iterator remove_duplicates(std::vector<VM::vmcode::instruction>::iterator it, std::vector<VM::vmcode::instruction>& vec)
    {
    if (!operations_duplicate_has_no_effect(it->oper))
      return it;
    auto it2 = it;
    ++it2;
    if (it2 == vec.end())
      return it;
    if (it2->oper == it->oper && it2->operand1 == it->operand1 && it2->operand1_mem == it->operand1_mem &&
      it2->operand2 == it->operand2 && it2->operand2_mem == it->operand2_mem &&
      it2->operand3 == it->operand3 && it2->operand3_mem == it->operand3_mem)
      {
      it = vec.erase(it);
      }
    return it;
    }


  std::set<VM::vmcode::operand> get_placeholder_operands(const std::vector<VM::vmcode::instruction>& vec)
    {
    std::set<VM::vmcode::operand> non_placeholders;
    non_placeholders.insert(VM::vmcode::XMM0);
    for (const auto& instr : vec)
      {
      if (instr.operand1 != VM::vmcode::NUMBER && instr.operand1 != VM::vmcode::EMPTY)
        {
        if (get_memory_register(instr.operand1) == instr.operand1 && get_register(instr.operand1) != instr.operand1) // any memory register is not a placeholder
          non_placeholders.insert(get_register(instr.operand1));
        }
      if (!is_operation_that_sets_first_register_without_side_effects(instr.oper))
        {
        non_placeholders.insert(get_register(instr.operand1));
        }
      }
    size_t non_placeholders_size = non_placeholders.size();
    bool repeat = true;
    while (repeat)
      {
      for (const auto& instr : vec)
        {
        if (is_operation_that_sets_first_register_without_side_effects(instr.oper))
          {
          auto it = non_placeholders.find(get_register(instr.operand2));
          if (it != non_placeholders.end())
            non_placeholders.insert(get_register(instr.operand1));
          if (instr.operand3 != VM::vmcode::EMPTY)
            {
            it = non_placeholders.find(get_register(instr.operand3));
            if (it != non_placeholders.end())
              non_placeholders.insert(get_register(instr.operand1));
            }
          }
        }
      repeat = non_placeholders_size != non_placeholders.size();
      non_placeholders_size = non_placeholders.size();
      }
    std::set<VM::vmcode::operand> placeholders;
    for (int i = VM::vmcode::RAX; i <= VM::vmcode::XMM15; ++i)
      {
      auto reg = get_register((VM::vmcode::operand)i);
      auto it = non_placeholders.find(reg);
      if (it == non_placeholders.end())
        placeholders.insert(reg);
      }
    return placeholders;
    }

  std::vector<VM::vmcode::instruction>::iterator transitive_numbers(std::vector<VM::vmcode::instruction>::iterator it, std::vector<VM::vmcode::instruction>& vec, const std::set<VM::vmcode::operand>& placeholder_operands)
    {
    if (it->oper != VM::vmcode::MOV)
      return it;
    if (it->operand2 != VM::vmcode::NUMBER)
      return it;
    auto find_placeholder = placeholder_operands.find(it->operand1);
    if (find_placeholder == placeholder_operands.end())
      return it;
    // we have so far MOV local_register number
    auto local_register = it->operand1;
    uint64_t number_value = it->operand2_mem;
    it = vec.erase(it);
    auto it2 = it;
    bool done = it2 == vec.end();
    while (!done)
      {
      if (it2->oper == VM::vmcode::LABEL || it->oper == VM::vmcode::LABEL_ALIGNED || it->oper == VM::vmcode::GLOBAL)
        {
        done = true;
        }
      else if (is_operation_that_sets_first_register_without_side_effects(it2->oper) && it2->operand1 == local_register)
        {
        done = true;
        }
      if (!done)
        {
        if (it2->operand2 == local_register)
          {
          assert(it2->operand2_mem == 0);
          it2->operand2 = VM::vmcode::NUMBER;
          it2->operand2_mem = number_value;
          }
        if (it2->operand3 == local_register)
          {
          assert(it2->operand3_mem == 0);
          it2->operand3 = VM::vmcode::NUMBER;
          it2->operand3_mem = number_value;
          }
        }
      ++it2;
      if (it2 == vec.end())
        done = true;
      }

    return it;
    }

  std::vector<VM::vmcode::instruction>::iterator peephole_superoperators(std::vector<VM::vmcode::instruction>::iterator it, std::vector<VM::vmcode::instruction>& vec)
    {
    if (it->oper == VM::vmcode::MOV)
      {
      auto it2 = it;
      ++it2;
      if (it2 == vec.end())
        return it;
      if (it2->operand1 != it->operand1 || it2->operand1_mem != it->operand1_mem)
        return it;
      switch (it2->oper)
        {
        case VM::vmcode::ADD:
          it->oper = VM::vmcode::MOVADD;
          it->operand3 = it2->operand2;
          it->operand3_mem = it2->operand2_mem;
          it = vec.erase(it2);
          break;
        case VM::vmcode::SUB:
          it->oper = VM::vmcode::MOVSUB;
          it->operand3 = it2->operand2;
          it->operand3_mem = it2->operand2_mem;
          it = vec.erase(it2);
          break;
        //case VM::vmcode::MUL:
        //  it->oper = VM::vmcode::MOVMUL;
        //  it->operand3 = it2->operand2;
        //  it->operand3_mem = it2->operand2_mem;
        //  it = vec.erase(it2);
        //  break;
        case VM::vmcode::DIV2:
          it->oper = VM::vmcode::MOVDIV;
          it->operand3 = it2->operand2;
          it->operand3_mem = it2->operand2_mem;
          it = vec.erase(it2);
          break;
        case VM::vmcode::SHL:
          it->oper = VM::vmcode::MOVSHL;
          it->operand3 = it2->operand2;
          it->operand3_mem = it2->operand2_mem;
          it = vec.erase(it2);
          break;
        case VM::vmcode::SHR:
          it->oper = VM::vmcode::MOVSHR;
          it->operand3 = it2->operand2;
          it->operand3_mem = it2->operand2_mem;
          it = vec.erase(it2);
          break;
        default:
          break;
        }
      }
    else if (it->oper == VM::vmcode::SHL)
      {
      auto it2 = it;
      ++it2;
      if (it2 == vec.end())
        return it;
      if (it2->operand2 != it->operand1 || it2->operand2_mem != it->operand1_mem)
        return it;
      if (it2->oper == VM::vmcode::ADD)
        {
        it->oper = VM::vmcode::ADDSHL;
        it->operand3 = it->operand2;
        it->operand3_mem = it->operand2_mem;
        it->operand2 = it->operand1;
        it->operand2_mem = it->operand1_mem;
        it->operand1 = it2->operand1;
        it->operand1_mem = it2->operand1_mem;
        it = vec.erase(it2);
        }
      }
    return it;
    }

  void peephole(std::vector<VM::vmcode::instruction>& vec, const std::vector<vmcode::operand>& temporary_operands)
    {
    auto placeholder_operands = get_placeholder_operands(vec);
    auto it = vec.begin();
    while (it != vec.end())
      {
      //it = peephole_mov_1(it, vec, temporary_operands);
      //it = peephole_mov_2(it, vec, temporary_operands);
      it = peephole_mov_3(it, vec);
      it = remove_duplicates(it, vec);
      //it = transitive_numbers(it, vec, placeholder_operands);
      it = peephole_superoperators(it, vec);
      ++it;
      }
    }
  } // namespace
  
void peephole_optimization(VM::vmcode& code, const std::vector<vmcode::operand>& temporary_operands)
  {
  
  for (auto& list : code.get_instructions_list())
    {
    peephole(list, temporary_operands);
    peephole(list, temporary_operands);
    peephole(list, temporary_operands);
    }
  
  }
  
VM_END
