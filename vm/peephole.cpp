#include "peephole.h"

#include <set>
#include <cassert>

VM_BEGIN

namespace
  {
  
  /*
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
  */

  std::vector<VM::vmcode::instruction>::iterator peephole_mov(std::vector<VM::vmcode::instruction>::iterator it, std::vector<VM::vmcode::instruction>& vec)
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
      case VM::vmcode::AND8:
      case VM::vmcode::OR:
      case VM::vmcode::CMP:
      case VM::vmcode::CMP8:      
      case VM::vmcode::CMPEQPD:
      case VM::vmcode::CMPLTPD:
      case VM::vmcode::CMPLEPD:
      case VM::vmcode::TEST:
      case VM::vmcode::TEST8:
      case VM::vmcode::MOV:
      case VM::vmcode::MOV8:
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

  void peephole(std::vector<VM::vmcode::instruction>& vec)
    {
    auto it = vec.begin();
    while (it != vec.end())
      {
      it = peephole_mov(it, vec);
      it = remove_duplicates(it, vec);
      it = peephole_superoperators(it, vec);
      ++it;
      }
    }
  } // namespace
  
void peephole_optimization(VM::vmcode& code)
  {
  
  for (auto& list : code.get_instructions_list())
    {
    peephole(list);
    peephole(list);
    peephole(list);
    }
  
  }
  
VM_END
