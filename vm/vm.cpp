#include "vm.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <array>

VM_BEGIN

namespace
  {

  bool is_8_bit(uint64_t number)
    {
    int8_t b = int8_t(number);
    return int64_t(b) == (int64_t)number;
    }

  bool is_16_bit(uint64_t number)
    {
    int16_t b = int16_t(number);
    return int64_t(b) == (int64_t)number;
    }

  bool is_32_bit(uint64_t number)
    {
    int32_t b = int32_t(number);
    return int64_t(b) == (int64_t)number;
    }

  int8_t _number_of_operands(const vmcode::operation& op)
    {
    switch (op)
      {
      case vmcode::ADD: return 2;
      case vmcode::ADDSD: return 2;
      case vmcode::AND: return 2;
      case vmcode::CALLEXTERNAL:return 1;
      case vmcode::CALL:return 1;
      case vmcode::COMMENT: return 0;
      case vmcode::CMP: return 2;
      case vmcode::CMPEQPD: return 2;
      case vmcode::CMPLTPD: return 2;
      case vmcode::CMPLEPD: return 2;
      case vmcode::CVTSI2SD: return 2;
      case vmcode::CVTTSD2SI:return 2;
      case vmcode::CSQRT:return 2;
      case vmcode::CSIN:return 2;
      case vmcode::CCOS:return 2;
      case vmcode::CEXP:return 2;
      case vmcode::CLOG:return 2;
      case vmcode::CLOG2:return 2;
      case vmcode::CABS:return 2;
      case vmcode::CTAN:return 2;
      case vmcode::CATAN:return 2;
      case vmcode::CATAN2:return 2;
      case vmcode::CPOW:return 2;
      case vmcode::CMIN:return 2;
      case vmcode::CMAX:return 2;
      case vmcode::SWAP:return 2;
      case vmcode::DEC: return 1;
      case vmcode::DIV: return 1;
      case vmcode::DIV2: return 2;
      case vmcode::DIVSD: return 2;
      case vmcode::EXTERN: return 0;
      case vmcode::GLOBAL:return 0;
      case vmcode::LABEL:return 0;
      case vmcode::LABEL_ALIGNED:return 0;
      case vmcode::IDIV:return 1;
      case vmcode::IDIV2:return 2;
      case vmcode::IMUL:return 2;
      case vmcode::INC: return 1;
      case vmcode::JE: return 1;
      case vmcode::JL: return 1;
      case vmcode::JLE:return 1;
      case vmcode::JA: return 1;
      case vmcode::JB: return 1;
      case vmcode::JG: return 1;
      case vmcode::JGE:return 1;
      case vmcode::JNE:return 1;
      case vmcode::JMP:return 1;
      case vmcode::LEA: return 2;
      case vmcode::MOD: return 2;
      case vmcode::MODSD: return 2;
      case vmcode::MOV: return 2;
      case vmcode::MOVMSKPD: return 2;
      case vmcode::MUL: return 1;
      case vmcode::MULSD:return 2;
      case vmcode::NEG:return 1;
      case vmcode::NOP:return 0;
      case vmcode::OR:return 2;
      case vmcode::POP: return 1;
      case vmcode::PUSH: return 1;
      case vmcode::RET: return 0;
      case vmcode::SAL: return 2;
      case vmcode::SAR: return 2;
      case vmcode::SETE: return 1;
      case vmcode::SETNE:return 1;
      case vmcode::SETL: return 1;
      case vmcode::SETG: return 1;
      case vmcode::SETLE:return 1;
      case vmcode::SETGE:return 1;
      case vmcode::SHL: return 2;
      case vmcode::SHR:return 2;
      case vmcode::SQRTPD:return 2;
      case vmcode::SUB: return 2;
      case vmcode::SUBSD: return 2;
      case vmcode::TEST: return 2;
      case vmcode::UCOMISD: return 2;
      case vmcode::XOR: return 2;
      case vmcode::XORPD: return 2;

      case vmcode::MOVADD: return 3;
      case vmcode::MOVSUB: return 3;
      case vmcode::MOVMUL: return 3;
      case vmcode::MOVDIV: return 3;
      case vmcode::MOVSHL: return 3;
      case vmcode::MOVSHR: return 3;
      case vmcode::ADDSHL: return 3;
      default:
      {
      //std::stringstream str;
      //str << vmcode::operation_to_string(op) << " number of operands unknown!";
      //throw std::logic_error(str.str());
      return -1;
      }
      }
    }

  std::array<int8_t, 128> build_number_of_operands_array()
    {
    std::array<int8_t, 128> arr;
    for (int i = 0; i < 128; ++i)
      {
      arr[i] = _number_of_operands((VM::vmcode::operation)i);
      }
    return arr;
    }

  static std::array<int8_t, 128> nr_oprnds = build_number_of_operands_array();

  int8_t number_of_operands(const vmcode::operation& op)
    {    
    return nr_oprnds[(int)op];
    }

  enum operand_immediate_type
    {
    _VARIABLE,
    _8BIT,
    _32BIT,
    _64BIT
    };

  operand_immediate_type get_operand_immediate_type(const vmcode::operation& op)
    {
    switch (op)
      {
      case vmcode::CALL:
      case vmcode::JE:
      case vmcode::JL:
      case vmcode::JLE:
      case vmcode::JA:
      case vmcode::JB:
      case vmcode::JG:
      case vmcode::JGE:
      case vmcode::JNE:
      case vmcode::JMP:
        return _32BIT;
      default: return _VARIABLE;
      }
    }

  bool ignore_operation_as_bytecode(const vmcode::operation& op)
    {
    switch (op)
      {
      case vmcode::LABEL: return true;
      case vmcode::LABEL_ALIGNED: return true;
      case vmcode::GLOBAL: return true;
      case vmcode::COMMENT: return true;
      default: return false;
      }
    }

  void get_memory_size_type(uint8_t& opmem, bool& save_mem_size, const vmcode::operation& op, const vmcode::operand& oprnd, uint64_t oprnd_mem)
    {
    save_mem_size = false;
    opmem = 0;
    switch (oprnd)
      {
      case  vmcode::EMPTY:
      case  vmcode::RAX:
      case  vmcode::RBX:
      case  vmcode::RCX:
      case  vmcode::RDX:
      case  vmcode::RSI:
      case  vmcode::RDI:
      case  vmcode::RSP:
      case  vmcode::RBP:
      case  vmcode::R8:
      case  vmcode::R9:
      case  vmcode::R10:
      case  vmcode::R11:
      case  vmcode::R12:
      case  vmcode::R13:
      case  vmcode::R14:
      case  vmcode::R15:
      case  vmcode::XMM0:
      case  vmcode::XMM1:
      case  vmcode::XMM2:
      case  vmcode::XMM3:
      case  vmcode::XMM4:
      case  vmcode::XMM5:
      case  vmcode::XMM6:
      case  vmcode::XMM7:
      case  vmcode::XMM8:
      case  vmcode::XMM9:
      case  vmcode::XMM10:
      case  vmcode::XMM11:
      case  vmcode::XMM12:
      case  vmcode::XMM13:
      case  vmcode::XMM14:
      case  vmcode::XMM15:
        return;
      case vmcode::NUMBER:
      {
      auto memtype = get_operand_immediate_type(op);
      if (memtype == _32BIT)
        {
        opmem = 3; return;
        }
      /*
      switch (memtype)
        {
        case _VARIABLE: break;
        case _8BIT: opmem = 1; return;
        case _32BIT: opmem = 3; return;
        case _64BIT: opmem = 4; return;
        }
      */
      break;
      }
      default: break;
      }
    if (is_8_bit(oprnd_mem))
      opmem = 1;
    else if (is_16_bit(oprnd_mem))
      {
      save_mem_size = true;
      opmem = 2;
      }
    else if (is_32_bit(oprnd_mem))
      {
      save_mem_size = true;
      opmem = 3;
      }
    else
      {
      save_mem_size = true;
      opmem = 4;
      }
    if (oprnd == vmcode::LABELADDRESS)
      {
      save_mem_size = true;
      opmem = 4;
      }
    }

  uint8_t _get_memory_size_type_fast(const vmcode::operand& oprnd)
    {
    switch (oprnd)
      {
      case  vmcode::EMPTY:
      case  vmcode::RAX:
      case  vmcode::RBX:
      case  vmcode::RCX:
      case  vmcode::RDX:
      case  vmcode::RSI:
      case  vmcode::RDI:
      case  vmcode::RSP:
      case  vmcode::RBP:
      case  vmcode::R8:
      case  vmcode::R9:
      case  vmcode::R10:
      case  vmcode::R11:
      case  vmcode::R12:
      case  vmcode::R13:
      case  vmcode::R14:
      case  vmcode::R15:
      case  vmcode::XMM0:
      case  vmcode::XMM1:
      case  vmcode::XMM2:
      case  vmcode::XMM3:
      case  vmcode::XMM4:
      case  vmcode::XMM5:
      case  vmcode::XMM6:
      case  vmcode::XMM7:
      case  vmcode::XMM8:
      case  vmcode::XMM9:
      case  vmcode::XMM10:
      case  vmcode::XMM11:
      case  vmcode::XMM12:
      case  vmcode::XMM13:
      case  vmcode::XMM14:
      case  vmcode::XMM15:
        return 0;
      case vmcode::NUMBER:
      {      
      return 1;
      }
      case vmcode::LABELADDRESS:
      {
      return 4;      
      }
      default:
        break;      
      }
    return 1;
    }

  std::array<uint8_t, (int)vmcode::LABELADDRESS+1> _build_memory_size_type_fast_array_1()
    {
    std::array<uint8_t, (int)vmcode::LABELADDRESS + 1> arr;
    for (int i = 0; i <= (int)vmcode::LABELADDRESS; ++i)
      {
      arr[i] = _get_memory_size_type_fast((vmcode::operand)i);
      }
    return arr;
    }

  std::array<uint8_t, 128> _build_memory_size_type_fast_array_2()
    {
    std::array<uint8_t, 128> arr;
    for (int i = 0; i <128; ++i)
      {
      auto memtype = get_operand_immediate_type((vmcode::operation)i);
      arr[i] = memtype == _32BIT ? 3 : 1;
      }
    return arr;
    }

  static std::array<uint8_t, (int)vmcode::LABELADDRESS + 1> arr1 = _build_memory_size_type_fast_array_1();
  static std::array<uint8_t, 128> arr2 = _build_memory_size_type_fast_array_2();

  inline uint8_t get_memory_size_type_fast(const vmcode::operation& op, const vmcode::operand& oprnd)
    {
    return arr1[(int)oprnd]*arr2[(int)op];    
    }

  /*
  Byte settings for normal operators:
  byte 1: operation opcode: equal to (int)vmcode::operation value of instr.oper
  byte 2: first operand: equal to (int)vmcode::operand of instr.operand1
  byte 3: second operand: equal to (int)vmcode::operand of instr.operand2
  byte 4: information on operand1_mem and operand2_mem
          First four bits equal to: 0 => instr.operand1_mem equals zero
                                  : 1 => instr.operand1_mem needs 8 bits
                                  : 2 => instr.operand1_mem needs 16 bits
                                  : 3 => instr.operand1_mem needs 32 bits
                                  : 4 => instr.operand1_mem needs 64 bits
          Last four bits equal to : 0 => instr.operand2_mem equals zero
                                  : 1 => instr.operand2_mem needs 8 bits
                                  : 2 => instr.operand2_mem needs 16 bits
                                  : 3 => instr.operand2_mem needs 32 bits
                                  : 4 => instr.operand2_mem needs 64 bits
  byte 5+: instr.operand1_mem using as many bytes as warranted by byte 4, followed by instr.operand2_mem using as many bytes as warranted by byte4.

  Byte settings for super operators (i.e. 3 operands):
  byte 1: operation opcode: equal to (int)vmcode::operation value of instr.oper
  byte 2: first operand: equal to (int)vmcode::operand of instr.operand1
  byte 3: second operand: equal to (int)vmcode::operand of instr.operand2
  byte 4: third operand: equal to (int)vmcode::operand of instr.operand3
  byte 5: information on operand1_mem and operand2_mem
          First two bits equal to : 0 => instr.operand1_mem equals zero
                                  : 1 => instr.operand1_mem needs 8 bits
                                  : 2 => instr.operand1_mem needs 32 bits
                                  : 3 => instr.operand1_mem needs 64 bits
          Bits three to five equal to : 0 => instr.operand2_mem equals zero
                                      : 1 => instr.operand2_mem needs 8 bits
                                      : 2 => instr.operand2_mem needs 16 bits
                                      : 3 => instr.operand2_mem needs 32 bits
                                      : 4 => instr.operand2_mem needs 64 bits
          Last three bits equal to    : 0 => instr.operand3_mem equals zero
                                      : 1 => instr.operand3_mem needs 8 bits
                                      : 2 => instr.operand3_mem needs 16 bits
                                      : 3 => instr.operand3_mem needs 32 bits
                                      : 4 => instr.operand3_mem needs 64 bits
  byte 6+: instr.operand1_mem using as many bytes as warranted by byte 5, followed by instr.operand2_mem using as many bytes as warranted by byte 5,
           flollowed by instr.operand3_mem using as many bytes as warranted by byte 5.
  */
  uint64_t fill_vm_bytecode(const vmcode::instruction& instr, uint8_t* opcode_stream)
    {
    uint64_t sz = 0;
    if (ignore_operation_as_bytecode(instr.oper))
      return sz;
    opcode_stream[sz++] = (uint8_t)instr.oper;
    uint8_t op1mem = 0;
    uint8_t op2mem = 0;
    uint8_t op3mem = 0;
    int nr_ops = number_of_operands(instr.oper);
    assert(nr_ops >= 0);
    if (nr_ops == 1)
      {
      bool savemem = true;
      get_memory_size_type(op1mem, savemem, instr.oper, instr.operand1, instr.operand1_mem);
      if (savemem)
        {
        opcode_stream[sz++] = (uint8_t)instr.operand1;
        opcode_stream[sz++] = op1mem;
        }
      else
        {
        opcode_stream[sz++] = (uint8_t)instr.operand1 | operand_has_8bit_mem;
        }
      }
    else if (nr_ops == 2)
      {
      bool savemem1 = true;
      bool savemem2 = true;
      get_memory_size_type(op1mem, savemem1, instr.oper, instr.operand1, instr.operand1_mem);
      get_memory_size_type(op2mem, savemem2, instr.oper, instr.operand2, instr.operand2_mem);
      if (savemem1 || savemem2)
        {
        opcode_stream[sz++] = (uint8_t)instr.operand1;
        opcode_stream[sz++] = (uint8_t)instr.operand2;
        opcode_stream[sz++] = (uint8_t)(op2mem << 4) | op1mem;
        }
      else
        {
        opcode_stream[sz++] = (uint8_t)instr.operand1 | operand_has_8bit_mem;
        opcode_stream[sz++] = (uint8_t)instr.operand2 | operand_has_8bit_mem;
        }
      }
    else if (nr_ops == 3)
      {      
      bool savemem1 = true;
      bool savemem2 = true;
      bool savemem3 = true;
      get_memory_size_type(op1mem, savemem1, instr.oper, instr.operand1, instr.operand1_mem);
      get_memory_size_type(op2mem, savemem2, instr.oper, instr.operand2, instr.operand2_mem);
      get_memory_size_type(op3mem, savemem3, instr.oper, instr.operand3, instr.operand3_mem);
      if (savemem1 || savemem2 || savemem3)
        {
        // see bit layout for memory in explanation above for super operators
        if (op1mem == 2) // 16bit
          op1mem = 3; // make 32 bit
        uint8_t op1memstorage = op1mem;
        if (op1memstorage >= 3)
          --op1memstorage;
        opcode_stream[sz++] = (uint8_t)instr.operand1;
        opcode_stream[sz++] = (uint8_t)instr.operand2;
        opcode_stream[sz++] = (uint8_t)instr.operand3;
        opcode_stream[sz++] = (uint8_t)(op3mem << 5) | (uint8_t)(op2mem << 2) | op1memstorage;
        }
      else
        {
        opcode_stream[sz++] = (uint8_t)instr.operand1 | operand_has_8bit_mem;
        opcode_stream[sz++] = (uint8_t)instr.operand2 | operand_has_8bit_mem;
        opcode_stream[sz++] = (uint8_t)instr.operand3 | operand_has_8bit_mem;
        }
      }
    switch (op1mem)
      {
      case 1: opcode_stream[sz++] = (uint8_t)instr.operand1_mem; break;
      case 2: *(reinterpret_cast<uint16_t*>(opcode_stream + sz)) = (uint16_t)instr.operand1_mem; sz += 2; break;
      case 3: *(reinterpret_cast<uint32_t*>(opcode_stream + sz)) = (uint32_t)instr.operand1_mem; sz += 4; break;
      case 4: *(reinterpret_cast<uint64_t*>(opcode_stream + sz)) = (uint64_t)instr.operand1_mem; sz += 8; break;
      default: break;
      }
    switch (op2mem)
      {
      case 1: opcode_stream[sz++] = (uint8_t)instr.operand2_mem; break;
      case 2: *(reinterpret_cast<uint16_t*>(opcode_stream + sz)) = (uint16_t)instr.operand2_mem; sz += 2; break;
      case 3: *(reinterpret_cast<uint32_t*>(opcode_stream + sz)) = (uint32_t)instr.operand2_mem; sz += 4; break;
      case 4: *(reinterpret_cast<uint64_t*>(opcode_stream + sz)) = (uint64_t)instr.operand2_mem; sz += 8; break;
      default: break;
      }
    switch (op3mem)
      {
      case 1: opcode_stream[sz++] = (uint8_t)instr.operand3_mem; break;
      case 2: *(reinterpret_cast<uint16_t*>(opcode_stream + sz)) = (uint16_t)instr.operand3_mem; sz += 2; break;
      case 3: *(reinterpret_cast<uint32_t*>(opcode_stream + sz)) = (uint32_t)instr.operand3_mem; sz += 4; break;
      case 4: *(reinterpret_cast<uint64_t*>(opcode_stream + sz)) = (uint64_t)instr.operand3_mem; sz += 8; break;
      default: break;
      }
    return sz;
    }


  void first_pass(first_pass_data& data, vmcode& code, const std::map<std::string, uint64_t>& externals)
    {
    uint8_t buffer[255];
    data.size = 0;
    data.label_to_address.clear();
    for (auto it = code.get_instructions_list().begin(); it != code.get_instructions_list().end(); ++it)
      {
      std::vector<std::pair<size_t, int>> nops_to_add;
      for (size_t i = 0; i < it->size(); ++i)
        {
        auto instr = (*it)[i];
        switch (instr.oper)
          {
          case vmcode::CALLEXTERNAL:
          {
          auto it2 = externals.find(instr.text);
          if (it2 != externals.end())
            {
            instr.oper = vmcode::MOV;
            instr.operand1 = vmcode::RAX;
            instr.operand2 = vmcode::NUMBER;
            instr.operand2_mem = it2->second;
            data.size += fill_vm_bytecode(instr, buffer);
            instr.oper = vmcode::CALLEXTERNAL;
            instr.operand1 = vmcode::RAX;
            instr.operand2 = vmcode::EMPTY;
            data.size += fill_vm_bytecode(instr, buffer);
            }
          else
            {
            //if (instr.operand1 == vmcode::EMPTY || instr.operand1 == vmcode::NUMBER)
            //  throw std::logic_error("CALLEXTERNAL used for non external call");
            data.size += fill_vm_bytecode(instr, buffer);
            }
          break;
          }
          case vmcode::CALL:
          {
          auto it2 = externals.find(instr.text);
          if (it2 != externals.end())
            {
            instr.oper = vmcode::MOV;
            instr.operand1 = vmcode::RAX;
            instr.operand2 = vmcode::NUMBER;
            instr.operand2_mem = it2->second;
            data.size += fill_vm_bytecode(instr, buffer);
            instr.oper = vmcode::CALL;
            instr.operand1 = vmcode::RAX;
            instr.operand2 = vmcode::EMPTY;
            data.size += fill_vm_bytecode(instr, buffer);
            }
          else
            {
            if (instr.operand1 == vmcode::EMPTY)
              {
              instr.operand1 = vmcode::NUMBER;
              instr.operand1_mem = 0x11111111;
              }
            data.size += fill_vm_bytecode(instr, buffer);
            }
          break;
          }
          case vmcode::JMP:
          case vmcode::JE:
          case vmcode::JL:
          case vmcode::JLE:
          case vmcode::JG:
          case vmcode::JA:
          case vmcode::JB:
          case vmcode::JGE:
          case vmcode::JNE:
          {
          if (instr.operand1 == vmcode::EMPTY)
            {
            instr.operand1 = vmcode::NUMBER;
            instr.operand1_mem = 0x11111111;
            }
          data.size += fill_vm_bytecode(instr, buffer);
          break;
          }
          case vmcode::LABEL:
            data.label_to_address[instr.text] = data.size; break;
          case vmcode::LABEL_ALIGNED:
            if (data.size & 7)
              {
              int nr_of_nops = 8 - (data.size & 7);
              data.size += nr_of_nops;
              nops_to_add.emplace_back(i, nr_of_nops);
              }
            data.label_to_address[instr.text] = data.size; break;
          case vmcode::GLOBAL:
            if (data.size & 7)
              {
              int nr_of_nops = 8 - (data.size & 7);
              data.size += nr_of_nops;
              nops_to_add.emplace_back(i, nr_of_nops);
              }
            data.label_to_address[instr.text] = data.size; break;
          case vmcode::EXTERN:
          {
          auto it2 = externals.find(instr.text);
          if (it2 == externals.end())
            throw std::logic_error("error: external is not defined");
          data.external_to_address[instr.text] = it2->second;
          break;
          }
          default:
            data.size += fill_vm_bytecode(instr, buffer); break;
          }
        }
      size_t nops_offset = 0;
      for (auto nops : nops_to_add)
        {
        std::vector<vmcode::instruction> nops_instructions(nops.second, vmcode::instruction(vmcode::NOP));
        it->insert(it->begin() + nops_offset + nops.first, nops_instructions.begin(), nops_instructions.end());
        nops_offset += nops.second;
        }
      }
    }


  uint8_t* second_pass(uint8_t* func, const first_pass_data& data, const vmcode& code)
    {
    uint64_t address_start = (uint64_t)(reinterpret_cast<uint64_t*>(func));
    uint8_t* start = func;
    for (auto it = code.get_instructions_list().begin(); it != code.get_instructions_list().end(); ++it)
      {
      for (vmcode::instruction instr : *it)
        {
        if (instr.operand1 == vmcode::LABELADDRESS)
          {
          auto it2 = data.label_to_address.find(instr.text);
          if (it2 == data.label_to_address.end())
            throw std::logic_error("error: label is not defined");
          instr.operand1_mem = address_start + it2->second;
          }
        if (instr.operand2 == vmcode::LABELADDRESS)
          {
          auto it2 = data.label_to_address.find(instr.text);
          if (it2 == data.label_to_address.end())
            throw std::logic_error("error: label is not defined");
          instr.operand2_mem = address_start + it2->second;
          }
        switch (instr.oper)
          {
          case vmcode::CALL:
          {
          if (instr.operand1 != vmcode::EMPTY)
            break;
          auto it_label = data.label_to_address.find(instr.text);
          auto it_external = data.external_to_address.find(instr.text);
          if (it_external != data.external_to_address.end())
            {
            vmcode::instruction extra_instr(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, it_external->second);
            func += fill_vm_bytecode(extra_instr, func);
            instr.operand1 = vmcode::RAX;
            }
          else if (it_label != data.label_to_address.end())
            {
            int64_t address = (int64_t)it_label->second;
            int64_t current = (int64_t)(func - start);
            instr.operand1 = vmcode::NUMBER;
            instr.operand1_mem = (int64_t(address - current));
            }
          else
            throw std::logic_error("second_pass error: call target does not exist");
          break;
          }
          case vmcode::JE:
          case vmcode::JL:
          case vmcode::JLE:
          case vmcode::JG:
          case vmcode::JA:
          case vmcode::JB:
          case vmcode::JGE:
          case vmcode::JNE:
          {
          if (instr.operand1 != vmcode::EMPTY)
            break;
          auto it2 = data.label_to_address.find(instr.text);
          if (it2 == data.label_to_address.end())
            throw std::logic_error("second_pass error: label does not exist");
          int64_t address = (int64_t)it2->second;
          int64_t current = (int64_t)(func - start);
          instr.operand1 = vmcode::NUMBER;
          instr.operand1_mem = (int64_t(address - current));
          break;
          }
          case vmcode::JMP:
          {
          if (instr.operand1 != vmcode::EMPTY)
            break;
          auto it2 = data.label_to_address.find(instr.text);
          if (it2 == data.label_to_address.end())
            throw std::logic_error("second_pass error: label does not exist");
          int64_t address = (int64_t)it2->second;
          int64_t current = (int64_t)(func - start);
          instr.operand1 = vmcode::NUMBER;
          instr.operand1_mem = (int64_t(address - current));
          break;
          }
          }
        func += fill_vm_bytecode(instr, func);
        }
      }
    while ((uint64_t)(func - start) < data.size)
      {
      vmcode::instruction dummy(vmcode::NOP);
      func += fill_vm_bytecode(dummy, func);
      }
    return func;
    }
  }

void* vm_bytecode(uint64_t& size, first_pass_data& d, vmcode& code, const std::map<std::string, uint64_t>& externals)
  {
  d.external_to_address.clear();
  d.label_to_address.clear();
  first_pass(d, code, externals);

  uint8_t* compiled_func = new uint8_t[d.size + d.data_size];

  if (!compiled_func)
    throw std::runtime_error("Could not allocate virtual memory");

  uint8_t* func_end = second_pass((uint8_t*)compiled_func, d, code);

  uint64_t size_used = func_end - (uint8_t*)compiled_func;

  if (size_used != d.size)
    {
    throw std::logic_error("error: error in size computation.");
    }

  size = d.size + d.data_size;

  return (void*)compiled_func;
  }

void* vm_bytecode(uint64_t& size, vmcode& code, const std::map<std::string, uint64_t>& externals)
  {
  first_pass_data d;
  return vm_bytecode(size, d, code, externals);
  }

void* vm_bytecode(uint64_t& size, first_pass_data& d, vmcode& code)
  {
  std::map<std::string, uint64_t> externals;
  return vm_bytecode(size, d, code, externals);
  }

void* vm_bytecode(uint64_t& size, vmcode& code)
  {
  std::map<std::string, uint64_t> externals;
  return vm_bytecode(size, code, externals);
  }

void free_bytecode(void* f, uint64_t size)
  {
  (void*)size;
  delete[](uint8_t*)f;
  }


/*
Byte settings for normal operators:
byte 1: operation opcode: equal to (int)vmcode::operation value of instr.oper
byte 2: first operand: equal to (int)vmcode::operand of instr.operand1
byte 3: second operand: equal to (int)vmcode::operand of instr.operand2
byte 4: information on operand1_mem and operand2_mem
        First four bits equal to: 0 => instr.operand1_mem equals zero
                                : 1 => instr.operand1_mem needs 8 bits
                                : 2 => instr.operand1_mem needs 16 bits
                                : 3 => instr.operand1_mem needs 32 bits
                                : 4 => instr.operand1_mem needs 64 bits
        Last four bits equal to : 0 => instr.operand2_mem equals zero
                                : 1 => instr.operand2_mem needs 8 bits
                                : 2 => instr.operand2_mem needs 16 bits
                                : 3 => instr.operand2_mem needs 32 bits
                                : 4 => instr.operand2_mem needs 64 bits
byte 5+: instr.operand1_mem using as many bytes as warranted by byte 4, followed by instr.operand2_mem using as many bytes as warranted by byte4.

Byte settings for super operators (i.e. 3 operands):
byte 1: operation opcode: equal to (int)vmcode::operation value of instr.oper
byte 2: first operand: equal to (int)vmcode::operand of instr.operand1
byte 3: second operand: equal to (int)vmcode::operand of instr.operand2
byte 4: third operand: equal to (int)vmcode::operand of instr.operand3
byte 5: information on operand1_mem and operand2_mem
        First two bits equal to : 0 => instr.operand1_mem equals zero
                                : 1 => instr.operand1_mem needs 8 bits
                                : 2 => instr.operand1_mem needs 32 bits
                                : 3 => instr.operand1_mem needs 64 bits
        Bits three to five equal to : 0 => instr.operand2_mem equals zero
                                    : 1 => instr.operand2_mem needs 8 bits
                                    : 2 => instr.operand2_mem needs 16 bits
                                    : 3 => instr.operand2_mem needs 32 bits
                                    : 4 => instr.operand2_mem needs 64 bits
        Last three bits equal to    : 0 => instr.operand3_mem equals zero
                                    : 1 => instr.operand3_mem needs 8 bits
                                    : 2 => instr.operand3_mem needs 16 bits
                                    : 3 => instr.operand3_mem needs 32 bits
                                    : 4 => instr.operand3_mem needs 64 bits
byte 6+: instr.operand1_mem using as many bytes as warranted by byte 5, followed by instr.operand2_mem using as many bytes as warranted by byte 5,
         flollowed by instr.operand3_mem using as many bytes as warranted by byte 5.
*/
uint64_t disassemble_bytecode(vmcode::operation& op,
  vmcode::operand& operand1,
  vmcode::operand& operand2,
  vmcode::operand& operand3,
  uint64_t& operand1_mem,
  uint64_t& operand2_mem,
  uint64_t& operand3_mem,
  const uint8_t* bytecode)
  {
  operand1 = vmcode::EMPTY;
  operand2 = vmcode::EMPTY;
  operand3 = vmcode::EMPTY;
  operand1_mem = 0;
  operand2_mem = 0;
  operand3_mem = 0;
  uint64_t sz = 0;
  uint8_t op1mem = 0;
  uint8_t op2mem = 0;
  uint8_t op3mem = 0;
  op = (vmcode::operation)bytecode[sz++];
  int nr_ops = number_of_operands(op);
  assert(nr_ops >= 0);
  if (nr_ops == 0)
    return sz;
  else if (nr_ops == 1)
    {
    uint8_t op1 = bytecode[sz++];
    if ((op1 & operand_has_8bit_mem) == 0)
      {
      op1mem = bytecode[sz++];
      operand1 = (vmcode::operand)op1;
      }
    else
      {
      op1 &= ~operand_has_8bit_mem;
      operand1 = (vmcode::operand)op1;
      op1mem = get_memory_size_type_fast(op, operand1);
      }
    }
  else if (nr_ops == 2)
    {
    assert(nr_ops == 2);
    uint8_t op1 = bytecode[sz++];
    uint8_t op2 = bytecode[sz++];
    if ((op1 & operand_has_8bit_mem) == 0)
      {
      op1mem = bytecode[sz] & 15;
      op2mem = bytecode[sz] >> 4;
      operand1 = (vmcode::operand)op1;
      operand2 = (vmcode::operand)op2;
      ++sz;
      }
    else
      {
      op1 &= ~operand_has_8bit_mem;
      op2 &= ~operand_has_8bit_mem;
      operand1 = (vmcode::operand)op1;
      operand2 = (vmcode::operand)op2;
      op1mem = get_memory_size_type_fast(op, operand1);
      op2mem = get_memory_size_type_fast(op, operand2);
      }
    }
  else
    {
    assert(nr_ops == 3);
    uint8_t op1 = bytecode[sz++];
    uint8_t op2 = bytecode[sz++];
    uint8_t op3 = bytecode[sz++];
    if ((op1 & operand_has_8bit_mem) == 0)
      {
      /*
      // see bit layout for memory in explanation above for super operators
      if (op1mem == 2) // 16bit
        op1mem = 3; // make 32 bit
      uint8_t op1memstorage = op1mem;
      if (op1memstorage >= 3)
        --op1memstorage;
      */
      op1mem = bytecode[sz] & 3;
      // see bit layout for memory in explanation above for super operators
      if (op1mem >= 2)
        ++op1mem;
      op2mem = (bytecode[sz] >> 2) & 7;
      op3mem = (bytecode[sz] >> 5) & 7;
      operand1 = (vmcode::operand)op1;
      operand2 = (vmcode::operand)op2;
      operand3 = (vmcode::operand)op3;
      ++sz;
      }
    else
      {
      op1 &= ~operand_has_8bit_mem;
      op2 &= ~operand_has_8bit_mem;
      op3 &= ~operand_has_8bit_mem;
      operand1 = (vmcode::operand)op1;
      operand2 = (vmcode::operand)op2;
      operand3 = (vmcode::operand)op3;
      op1mem = get_memory_size_type_fast(op, operand1);
      op2mem = get_memory_size_type_fast(op, operand2);
      op3mem = get_memory_size_type_fast(op, operand3);
      }
    }
  switch (op1mem)
    {
    case 1: operand1_mem = (int8_t)bytecode[sz++]; break;
    case 2: operand1_mem = (int16_t)(*reinterpret_cast<const uint16_t*>(bytecode + sz)); sz += 2; break;
    case 3: operand1_mem = (int32_t)(*reinterpret_cast<const uint32_t*>(bytecode + sz)); sz += 4; break;
    case 4: operand1_mem = *reinterpret_cast<const uint64_t*>(bytecode + sz); sz += 8; break;
    default: operand1_mem = 0; break;
    }
  switch (op2mem)
    {
    case 1: operand2_mem = (int8_t)bytecode[sz++]; break;
    case 2: operand2_mem = (int16_t)(*reinterpret_cast<const uint16_t*>(bytecode + sz)); sz += 2; break;
    case 3: operand2_mem = (int32_t)(*reinterpret_cast<const uint32_t*>(bytecode + sz)); sz += 4; break;
    case 4: operand2_mem = *reinterpret_cast<const uint64_t*>(bytecode + sz); sz += 8; break;
    default: operand2_mem = 0; break;
    }
  switch (op3mem)
    {
    case 1: operand3_mem = (int8_t)bytecode[sz++]; break;
    case 2: operand3_mem = (int16_t)(*reinterpret_cast<const uint16_t*>(bytecode + sz)); sz += 2; break;
    case 3: operand3_mem = (int32_t)(*reinterpret_cast<const uint32_t*>(bytecode + sz)); sz += 4; break;
    case 4: operand3_mem = *reinterpret_cast<const uint64_t*>(bytecode + sz); sz += 8; break;
    default: operand3_mem = 0; break;
    }
  return sz;

  }

namespace
  {
  void reset_registers(registers& regs)
    {
    regs.rbp = (uint64_t)(&regs.stack[0]);
    regs.rsp = (uint64_t)(&regs.stack[vm_stack_size]);
    regs.eflags = 0;
    }
  }

registers::registers()
  {
  reset_registers(*this);
  }

namespace
  {

  uint64_t* get_address_64bit(vmcode::operand oper, uint64_t operand_mem, registers& regs, uint64_t* reserved)
    {
    switch (oper)
      {
      case vmcode::EMPTY: return nullptr;
      case vmcode::RAX: return &regs.rax;
      case vmcode::RBX: return &regs.rbx;
      case vmcode::RCX: return &regs.rcx;
      case vmcode::RDX: return &regs.rdx;
      case vmcode::RDI: return &regs.rdi;
      case vmcode::RSI: return &regs.rsi;
      case vmcode::RSP: return &regs.rsp;
      case vmcode::RBP: return &regs.rbp;
      case vmcode::R8:  return &regs.r8;
      case vmcode::R9:  return &regs.r9;
      case vmcode::R10: return &regs.r10;
      case vmcode::R11: return &regs.r11;
      case vmcode::R12: return &regs.r12;
      case vmcode::R13: return &regs.r13;
      case vmcode::R14: return &regs.r14;
      case vmcode::R15: return &regs.r15;
      case vmcode::XMM0: return (uint64_t*)(&regs.xmm0);
      case vmcode::XMM1: return (uint64_t*)(&regs.xmm1);
      case vmcode::XMM2: return (uint64_t*)(&regs.xmm2);
      case vmcode::XMM3: return (uint64_t*)(&regs.xmm3);
      case vmcode::XMM4: return (uint64_t*)(&regs.xmm4);
      case vmcode::XMM5: return (uint64_t*)(&regs.xmm5);
      case vmcode::XMM6: return (uint64_t*)(&regs.xmm6);
      case vmcode::XMM7: return (uint64_t*)(&regs.xmm7);
      case vmcode::XMM8: return (uint64_t*)(&regs.xmm8);
      case vmcode::XMM9: return (uint64_t*)(&regs.xmm9);
      case vmcode::XMM10:return (uint64_t*)(&regs.xmm10);
      case vmcode::XMM11:return (uint64_t*)(&regs.xmm11);
      case vmcode::XMM12:return (uint64_t*)(&regs.xmm12);
      case vmcode::XMM13:return (uint64_t*)(&regs.xmm13);
      case vmcode::XMM14:return (uint64_t*)(&regs.xmm14);
      case vmcode::XMM15:return (uint64_t*)(&regs.xmm15);
      case vmcode::NUMBER: *reserved = operand_mem; return reserved;
      case vmcode::MEM_RAX: return (uint64_t*)(regs.rax + (int64_t)operand_mem);
      case vmcode::MEM_RBX: return (uint64_t*)(regs.rbx + (int64_t)operand_mem);
      case vmcode::MEM_RCX: return (uint64_t*)(regs.rcx + (int64_t)operand_mem);
      case vmcode::MEM_RDX: return (uint64_t*)(regs.rdx + (int64_t)operand_mem);
      case vmcode::MEM_RDI: return (uint64_t*)(regs.rdi + (int64_t)operand_mem);
      case vmcode::MEM_RSI: return (uint64_t*)(regs.rsi + (int64_t)operand_mem);
      case vmcode::MEM_RSP: return (uint64_t*)(regs.rsp + (int64_t)operand_mem);
      case vmcode::MEM_RBP: return (uint64_t*)(regs.rbp + (int64_t)operand_mem);
      case vmcode::MEM_R8:  return (uint64_t*)(regs.r8 + (int64_t)operand_mem);
      case vmcode::MEM_R9:  return (uint64_t*)(regs.r9 + (int64_t)operand_mem);
      case vmcode::MEM_R10: return (uint64_t*)(regs.r10 + (int64_t)operand_mem);
      case vmcode::MEM_R11: return (uint64_t*)(regs.r11 + (int64_t)operand_mem);
      case vmcode::MEM_R12: return (uint64_t*)(regs.r12 + (int64_t)operand_mem);
      case vmcode::MEM_R13: return (uint64_t*)(regs.r13 + (int64_t)operand_mem);
      case vmcode::MEM_R14: return (uint64_t*)(regs.r14 + (int64_t)operand_mem);
      case vmcode::MEM_R15: return (uint64_t*)(regs.r15 + (int64_t)operand_mem);
      case vmcode::LABELADDRESS: *reserved = operand_mem; return reserved;
      default: return nullptr;
      }
    }

  struct AddOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      left += right;
      }
    };

  struct AndOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      left &= right;
      }
    };

  struct ModOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      left %= right;
      }
    };

  struct MovOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      left = right;
      }
    };

  struct OrOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      left |= right;
      }
    };

  struct ShlOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      left <<= right;
      }
    };

  struct SarOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      int64_t l = (int64_t)left;
      l >>= right;
      left = (uint64_t)l;
      }
    };

  struct ShrOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      left >>= right;
      }
    };

  struct SubOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      left -= right;
      }
    };

  struct XorOper
    {
    static void apply(uint64_t& left, uint64_t right)
      {
      left ^= right;
      }
    };

  struct AddsdOper
    {
    static void apply(double& left, double right)
      {
      left += right;
      }
    };

  struct DivsdOper
    {
    static void apply(double& left, double right)
      {
      left /= right;
      }
    };

  struct ModsdOper
    {
    static void apply(double& left, double right)
      {
      left = fmod(left, right);
      }
    };

  struct MulsdOper
    {
    static void apply(double& left, double right)
      {
      left *= right;
      }
    };

  struct SubsdOper
    {
    static void apply(double& left, double right)
      {
      left -= right;
      }
    };

  struct CSinOper
    {
    static void apply(double& left, double right)
      {
      left = std::sin(right);
      }
    };

  struct CCosOper
    {
    static void apply(double& left, double right)
      {
      left = std::cos(right);
      }
    };

  struct CExpOper
    {
    static void apply(double& left, double right)
      {
      left = std::exp(right);
      }
    };

  struct CLogOper
    {
    static void apply(double& left, double right)
      {
      left = std::log(right);
      }
    };

  struct CLog2Oper
    {
    static void apply(double& left, double right)
      {
      left = std::log2(right);
      }
    };

  struct CAbsOper
    {
    static void apply(double& left, double right)
      {
      left = std::abs(right);
      }
    };

  struct CTanOper
    {
    static void apply(double& left, double right)
      {
      left = std::tan(right);
      }
    };

  struct CAtanOper
    {
    static void apply(double& left, double right)
      {
      left = std::atan(right);
      }
    };

  struct CAtan2Oper
    {
    static void apply(double& left, double right)
      {
      left = std::atan2(left, right);
      }
    };

  struct CPowOper
    {
    static void apply(double& left, double right)
      {
      left = std::pow(left, right);
      }
    };

  struct CMinOper
    {
    static void apply(double& left, double right)
      {
      left = std::min<double>(left, right);
      }
    };

  struct CMaxOper
    {
    static void apply(double& left, double right)
      {
      left = std::max<double>(left, right);
      }
    };

  struct SqrtpdOper
    {
    static void apply(double& left, double right)
      {
      left = std::sqrt(right);
      }
    };

  struct XorpdOper
    {
    static void apply(double& left, double right)
      {
      uint64_t l = *reinterpret_cast<uint64_t*>(&left);
      uint64_t r = *reinterpret_cast<uint64_t*>(&right);
      l ^= r;
      left = *reinterpret_cast<double*>(&l);
      }
    };

  template <class TOper>
  inline void execute_operation(vmcode::operand operand1,
    vmcode::operand operand2,
    uint64_t operand1_mem,
    uint64_t operand2_mem,
    registers& regs)
    {
    uint64_t tmp;
    uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
    assert(oprnd1);
    uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
    assert(oprnd2);
    TOper::apply(*oprnd1, *oprnd2);
    }

  template <class TOper>
  inline void execute_double_operation(vmcode::operand operand1,
    vmcode::operand operand2,
    uint64_t operand1_mem,
    uint64_t operand2_mem,
    registers& regs)
    {
    uint64_t tmp;
    uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
    uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
    assert(oprnd1);
    assert(oprnd2);
    TOper::apply(*reinterpret_cast<double*>(oprnd1), *reinterpret_cast<double*>(oprnd2));
    }

  template <class TOper>
  inline uint64_t execute_operation_const(vmcode::operand operand1,
    vmcode::operand operand2,
    uint64_t operand1_mem,
    uint64_t operand2_mem,
    registers& regs)
    {
    uint64_t tmp;
    uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
    assert(oprnd1);
    uint64_t left = *oprnd1;
    uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
    assert(oprnd2);
    TOper::apply(left, *oprnd2);
    return left;
    }

  inline void get_values(int64_t& left_signed, int64_t& right_signed, uint64_t& left_unsigned, uint64_t right_unsigned, vmcode::operand operand1,
    vmcode::operand operand2,
    uint64_t operand1_mem,
    uint64_t operand2_mem,
    registers& regs)
    {
    uint64_t tmp;
    uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
    assert(oprnd1);
    left_unsigned = *oprnd1;
    left_signed = (int64_t)left_unsigned;
    uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
    assert(oprnd2);
    right_unsigned = *oprnd2;
    right_signed = (int64_t)right_unsigned;
    }

  inline void compare_operation(vmcode::operand operand1,
    vmcode::operand operand2,
    uint64_t operand1_mem,
    uint64_t operand2_mem,
    registers& regs)
    {
    regs.eflags = 0;
    int64_t left_signed = 0;
    int64_t right_signed = 0;
    uint64_t left_unsigned = 0;
    uint64_t right_unsigned = 0;
    get_values(left_signed, right_signed, left_unsigned, right_unsigned, operand1, operand2, operand1_mem, operand2_mem, regs);
    if (left_signed == right_signed)
      regs.eflags |= zero_flag;
    if (left_unsigned < right_unsigned)
      regs.eflags |= carry_flag;
    int64_t temp = left_signed - right_signed;
    if ((temp < left_signed) != (right_signed > 0))
      regs.eflags |= overflow_flag;
    if (temp < 0)
      regs.eflags |= sign_flag;
    }

  void print(vmcode::operation op, vmcode::operand operand1,
    vmcode::operand operand2,
    uint64_t operand1_mem,
    uint64_t operand2_mem)
    {
    vmcode::instruction i;
    i.oper = op;
    i.operand1 = operand1;
    i.operand2 = operand2;
    i.operand1_mem = operand1_mem;
    i.operand2_mem = operand2_mem;
    i.stream(std::cout);
    }

  std::vector<vmcode::operand> get_calling_registers()
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

  uint64_t get_integer_register_value(vmcode::operand reg, const registers& regs)
    {
    switch (reg)
      {
      case vmcode::RDX: return regs.rdx;
      case vmcode::RCX: return regs.rcx;
      case vmcode::R8: return regs.r8;
      case vmcode::R9: return regs.r9;
      default: throw std::runtime_error("Invalid integer register as argument used");
      }
    }

  double get_floating_register_value(vmcode::operand reg, const registers& regs)
    {
    switch (reg)
      {
      case vmcode::XMM0: return regs.xmm0;
      case vmcode::XMM1: return regs.xmm1;
      case vmcode::XMM2: return regs.xmm2;
      case vmcode::XMM3: return regs.xmm3;
      default: throw std::runtime_error("Invalid floating point register as argument used");
      }
    }


  std::vector<vmcode::operand> _get_arguments(const external_function& f)
    {

    static std::vector<vmcode::operand> arg_reg = get_calling_registers();
    static std::vector<vmcode::operand> arg_float_reg = get_floating_point_registers();

    std::vector<vmcode::operand> arg_regs;
    for (size_t i = 0; i < f.arguments.size(); ++i)
      {
      arg_regs.push_back(f.arguments[i] == external_function::T_DOUBLE ? arg_float_reg[i] : arg_reg[i]);
      }

    return arg_regs;
    }

  template <class T>
  T _call_external_0(const external_function& f)
    {
    typedef T(*fun_ptr)();
    fun_ptr fun = (fun_ptr)f.address;
    return fun();
    }

  template <>
  void _call_external_0<void>(const external_function& f)
    {
    typedef void(*fun_ptr)();
    fun_ptr fun = (fun_ptr)f.address;
    fun();
    }

  template <class T, class T1, class V1>
  T _call_external_1(const external_function& f, V1 value)
    {
    typedef T(*fun_ptr)(T1);
    fun_ptr fun = (fun_ptr)f.address;
    return fun((T1)value);
    }

  template <class T1, class V1>
  void _call_external_1_void(const external_function& f, V1 value)
    {
    typedef void(*fun_ptr)(T1);
    fun_ptr fun = (fun_ptr)f.address;
    fun((T1)value);
    }

  template <class T>
  T _call_external_1(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 1);
    switch (f.arguments[0])
      {
      case external_function::T_BOOL: return _call_external_1<T, bool, uint64_t>(f, get_integer_register_value(args[0], regs));
      case external_function::T_CHAR_POINTER: return _call_external_1<T, char*, uint64_t>(f, get_integer_register_value(args[0], regs));
      case external_function::T_DOUBLE: return _call_external_1<T, double, double>(f, get_floating_register_value(args[0], regs));
      case external_function::T_INT64: return _call_external_1<T, int64_t, uint64_t>(f, get_integer_register_value(args[0], regs));
      case external_function::T_VOID: return 0;
      }
    return 0;
    }

  template <>
  void _call_external_1<void>(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 1);
    switch (f.arguments[0])
      {
      case external_function::T_BOOL:  _call_external_1_void<bool, uint64_t>(f, get_integer_register_value(args[0], regs)); break;
      case external_function::T_CHAR_POINTER:  _call_external_1_void<char*, uint64_t>(f, get_integer_register_value(args[0], regs)); break;
      case external_function::T_DOUBLE:  _call_external_1_void<double, double>(f, get_floating_register_value(args[0], regs)); break;
      case external_function::T_INT64:  _call_external_1_void<int64_t, uint64_t>(f, get_integer_register_value(args[0], regs)); break;
      case external_function::T_VOID: break;
      }
    }

  template <class T>
  T get_value(const vmcode::operand& arg, registers& regs)
    {
    return (T)get_integer_register_value(arg, regs);
    }

  template <>
  double get_value<double>(const vmcode::operand& arg, registers& regs)
    {
    return (double)get_floating_register_value(arg, regs);
    }

  template <class T, class T1, class T2>
  T _call_external_2_2(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    typedef T(*fun_ptr)(T1, T2);
    fun_ptr fun = (fun_ptr)f.address;
    return fun(get_value<T1>(args[0], regs), get_value<T2>(args[1], regs));
    }

  template <class T, class T1>
  T _call_external_2_1(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 2);
    switch (f.arguments[1])
      {
      case external_function::T_BOOL: return _call_external_2_2<T, T1, bool>(f, args, regs);
      case external_function::T_CHAR_POINTER: return _call_external_2_2<T, T1, char*>(f, args, regs);
      case external_function::T_DOUBLE: return _call_external_2_2<T, T1, double>(f, args, regs);
      case external_function::T_INT64: return _call_external_2_2<T, T1, int64_t>(f, args, regs);
      case external_function::T_VOID: return 0;
      }
    return 0;
    }

  template < class T1, class T2>
  void _call_external_2_2_void(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    typedef void(*fun_ptr)(T1, T2);
    fun_ptr fun = (fun_ptr)f.address;
    fun(get_value<T1>(args[0], regs), get_value<T2>(args[1], regs));
    }

  template <class T1>
  void _call_external_2_1_void(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 2);
    switch (f.arguments[1])
      {
      case external_function::T_BOOL:  _call_external_2_2_void<T1, bool>(f, args, regs); break;
      case external_function::T_CHAR_POINTER:  _call_external_2_2_void<T1, char*>(f, args, regs); break;
      case external_function::T_DOUBLE:  _call_external_2_2_void<T1, double>(f, args, regs); break;
      case external_function::T_INT64:  _call_external_2_2_void<T1, int64_t>(f, args, regs); break;
      case external_function::T_VOID: break;
      }
    }

  template <class T>
  T _call_external_2(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 2);
    switch (f.arguments[0])
      {
      case external_function::T_BOOL: return _call_external_2_1<T, bool>(f, args, regs);
      case external_function::T_CHAR_POINTER: return _call_external_2_1<T, char*>(f, args, regs);
      case external_function::T_DOUBLE: return _call_external_2_1<T, double>(f, args, regs);
      case external_function::T_INT64: return _call_external_2_1<T, int64_t>(f, args, regs);
      case external_function::T_VOID: return 0;
      }
    return 0;
    }

  template <>
  void _call_external_2<void>(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 2);
    switch (f.arguments[0])
      {
      case external_function::T_BOOL:  _call_external_2_1_void<bool>(f, args, regs); break;
      case external_function::T_CHAR_POINTER:  _call_external_2_1_void<char*>(f, args, regs); break;
      case external_function::T_DOUBLE:  _call_external_2_1_void<double>(f, args, regs); break;
      case external_function::T_INT64:  _call_external_2_1_void<int64_t>(f, args, regs); break;
      case external_function::T_VOID: break;
      }
    }

  template <class T, class T1, class T2, class T3>
  T _call_external_3_3(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    typedef T(*fun_ptr)(T1, T2, T3);
    fun_ptr fun = (fun_ptr)f.address;
    return fun(get_value<T1>(args[0], regs), get_value<T2>(args[1], regs), get_value<T3>(args[2], regs));
    }

  template <class T, class T1, class T2>
  T _call_external_3_2(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 3);
    switch (f.arguments[2])
      {
      case external_function::T_BOOL: return _call_external_3_3<T, T1, T2, bool>(f, args, regs);
      case external_function::T_CHAR_POINTER: return _call_external_3_3<T, T1, T2, char*>(f, args, regs);
      case external_function::T_DOUBLE: return _call_external_3_3<T, T1, T2, double>(f, args, regs);
      case external_function::T_INT64: return _call_external_3_3<T, T1, T2, int64_t>(f, args, regs);
      case external_function::T_VOID: return 0;
      }
    return 0;
    }

  template <class T, class T1>
  T _call_external_3_1(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 3);
    switch (f.arguments[1])
      {
      case external_function::T_BOOL: return _call_external_3_2<T, T1, bool>(f, args, regs);
      case external_function::T_CHAR_POINTER: return _call_external_3_2<T, T1, char*>(f, args, regs);
      case external_function::T_DOUBLE: return _call_external_3_2<T, T1, double>(f, args, regs);
      case external_function::T_INT64: return _call_external_3_2<T, T1, int64_t>(f, args, regs);
      case external_function::T_VOID: return 0;
      }
    return 0;
    }

  template <class T1, class T2, class T3>
  void _call_external_3_3_void(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    typedef void(*fun_ptr)(T1, T2, T3);
    fun_ptr fun = (fun_ptr)f.address;
    fun(get_value<T1>(args[0], regs), get_value<T2>(args[1], regs), get_value<T3>(args[2], regs));
    }

  template <class T1, class T2>
  void _call_external_3_2_void(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 3);
    switch (f.arguments[2])
      {
      case external_function::T_BOOL:  _call_external_3_3_void<T1, T2, bool>(f, args, regs); break;
      case external_function::T_CHAR_POINTER:  _call_external_3_3_void<T1, T2, char*>(f, args, regs); break;
      case external_function::T_DOUBLE:  _call_external_3_3_void<T1, T2, double>(f, args, regs); break;
      case external_function::T_INT64:  _call_external_3_3_void<T1, T2, int64_t>(f, args, regs); break;
      case external_function::T_VOID: break;
      }
    }

  template <class T1>
  void _call_external_3_1_void(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 3);
    switch (f.arguments[1])
      {
      case external_function::T_BOOL:  _call_external_3_2_void<T1, bool>(f, args, regs); break;
      case external_function::T_CHAR_POINTER:  _call_external_3_2_void<T1, char*>(f, args, regs); break;
      case external_function::T_DOUBLE:  _call_external_3_2_void<T1, double>(f, args, regs); break;
      case external_function::T_INT64:  _call_external_3_2_void<T1, int64_t>(f, args, regs); break;
      case external_function::T_VOID: break;
      }
    }

  template <class T>
  T _call_external_3(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 3);
    switch (f.arguments[0])
      {
      case external_function::T_BOOL: return _call_external_3_1<T, bool>(f, args, regs);
      case external_function::T_CHAR_POINTER: return _call_external_3_1<T, char*>(f, args, regs);
      case external_function::T_DOUBLE: return _call_external_3_1<T, double>(f, args, regs);
      case external_function::T_INT64: return _call_external_3_1<T, int64_t>(f, args, regs);
      case external_function::T_VOID: return 0;
      }
    return 0;
    }

  template <>
  void _call_external_3<void>(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 3);
    switch (f.arguments[0])
      {
      case external_function::T_BOOL:  _call_external_3_1_void<bool>(f, args, regs); break;
      case external_function::T_CHAR_POINTER:  _call_external_3_1_void<char*>(f, args, regs); break;
      case external_function::T_DOUBLE:  _call_external_3_1_void<double>(f, args, regs); break;
      case external_function::T_INT64:  _call_external_3_1_void<int64_t>(f, args, regs); break;
      case external_function::T_VOID: break;
      }
    }


  template <class T, class T1, class T2, class T3, class T4>
  T _call_external_4_4(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    typedef T(*fun_ptr)(T1, T2, T3, T4);
    fun_ptr fun = (fun_ptr)f.address;
    return fun(get_value<T1>(args[0], regs), get_value<T2>(args[1], regs), get_value<T3>(args[2], regs), get_value<T4>(args[3], regs));
    }

  template <class T, class T1, class T2, class T3>
  T _call_external_4_3(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 4);
    switch (f.arguments[3])
      {
      case external_function::T_BOOL: return _call_external_4_4<T, T1, T2, T3, bool>(f, args, regs);
      case external_function::T_CHAR_POINTER: return _call_external_4_4<T, T1, T2, T3, char*>(f, args, regs);
      case external_function::T_DOUBLE: return _call_external_4_4<T, T1, T2, T3, double>(f, args, regs);
      case external_function::T_INT64: return _call_external_4_4<T, T1, T2, T3, int64_t>(f, args, regs);
      case external_function::T_VOID: return 0;
      }
    return 0;
    }

  template <class T, class T1, class T2>
  T _call_external_4_2(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 4);
    switch (f.arguments[2])
      {
      case external_function::T_BOOL: return _call_external_4_3<T, T1, T2, bool>(f, args, regs);
      case external_function::T_CHAR_POINTER: return _call_external_4_3<T, T1, T2, char*>(f, args, regs);
      case external_function::T_DOUBLE: return _call_external_4_3<T, T1, T2, double>(f, args, regs);
      case external_function::T_INT64: return _call_external_4_3<T, T1, T2, int64_t>(f, args, regs);
      case external_function::T_VOID: return 0;
      }
    return 0;
    }

  template <class T, class T1>
  T _call_external_4_1(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 4);
    switch (f.arguments[1])
      {
      case external_function::T_BOOL: return _call_external_4_2<T, T1, bool>(f, args, regs);
      case external_function::T_CHAR_POINTER: return _call_external_4_2<T, T1, char*>(f, args, regs);
      case external_function::T_DOUBLE: return _call_external_4_2<T, T1, double>(f, args, regs);
      case external_function::T_INT64: return _call_external_4_2<T, T1, int64_t>(f, args, regs);
      case external_function::T_VOID: return 0;
      }
    return 0;
    }

  template <class T>
  T _call_external_4(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 4);
    switch (f.arguments[0])
      {
      case external_function::T_BOOL: return _call_external_4_1<T, bool>(f, args, regs);
      case external_function::T_CHAR_POINTER: return _call_external_4_1<T, char*>(f, args, regs);
      case external_function::T_DOUBLE: return _call_external_4_1<T, double>(f, args, regs);
      case external_function::T_INT64: return _call_external_4_1<T, int64_t>(f, args, regs);
      case external_function::T_VOID: return 0;
      }
    return 0;
    }


  template <class T1, class T2, class T3, class T4>
  void _call_external_4_4_void(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    typedef void(*fun_ptr)(T1, T2, T3, T4);
    fun_ptr fun = (fun_ptr)f.address;
    fun(get_value<T1>(args[0], regs), get_value<T2>(args[1], regs), get_value<T3>(args[2], regs), get_value<T4>(args[3], regs));
    }

  template <class T1, class T2, class T3>
  void _call_external_4_3_void(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 2);
    switch (f.arguments[3])
      {
      case external_function::T_BOOL:  _call_external_4_4_void<T1, T2, T3, bool>(f, args, regs); break;
      case external_function::T_CHAR_POINTER:  _call_external_4_4_void<T1, T2, T3, char*>(f, args, regs); break;
      case external_function::T_DOUBLE:  _call_external_4_4_void<T1, T2, T3, double>(f, args, regs); break;
      case external_function::T_INT64:  _call_external_4_4_void<T1, T2, T3, int64_t>(f, args, regs); break;
      case external_function::T_VOID: break;
      }
    }

  template <class T1, class T2>
  void _call_external_4_2_void(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 2);
    switch (f.arguments[2])
      {
      case external_function::T_BOOL:  _call_external_4_3_void<T1, T2, bool>(f, args, regs); break;
      case external_function::T_CHAR_POINTER:  _call_external_4_3_void<T1, T2, char*>(f, args, regs); break;
      case external_function::T_DOUBLE:  _call_external_4_3_void<T1, T2, double>(f, args, regs); break;
      case external_function::T_INT64:  _call_external_4_3_void<T1, T2, int64_t>(f, args, regs); break;
      case external_function::T_VOID: break;
      }
    }

  template <class T1>
  void _call_external_4_1_void(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 2);
    switch (f.arguments[1])
      {
      case external_function::T_BOOL:  _call_external_4_2_void<T1, bool>(f, args, regs); break;
      case external_function::T_CHAR_POINTER:  _call_external_4_2_void<T1, char*>(f, args, regs); break;
      case external_function::T_DOUBLE:  _call_external_4_2_void<T1, double>(f, args, regs); break;
      case external_function::T_INT64:  _call_external_4_2_void<T1, int64_t>(f, args, regs); break;
      case external_function::T_VOID: break;
      }
    }

  template <>
  void _call_external_4<void>(const external_function& f, std::vector<vmcode::operand>& args, registers& regs)
    {
    assert(args.size() == 4);
    switch (f.arguments[0])
      {
      case external_function::T_BOOL:  _call_external_4_1_void<bool>(f, args, regs); break;
      case external_function::T_CHAR_POINTER:  _call_external_4_1_void<char*>(f, args, regs); break;
      case external_function::T_DOUBLE:  _call_external_4_1_void<double>(f, args, regs); break;
      case external_function::T_INT64:  _call_external_4_1_void<int64_t>(f, args, regs); break;
      case external_function::T_VOID: break;
      }
    }


  template <class T>
  T _call_external(const external_function& f, registers& regs)
    {
    auto args = _get_arguments(f);
    switch (args.size())
      {
      case 0: return _call_external_0<T>(f);
      case 1: return _call_external_1<T>(f, args, regs);
      case 2: return _call_external_2<T>(f, args, regs);
      case 3: return _call_external_3<T>(f, args, regs);
      case 4: return _call_external_4<T>(f, args, regs);
      default: break;
      }
    return 0;
    }

  template <>
  void _call_external(const external_function& f, registers& regs)
    {
    auto args = _get_arguments(f);
    switch (args.size())
      {
      case 0: _call_external_0<void>(f); break;
      case 1: _call_external_1<void>(f, args, regs); break;
      case 2: _call_external_2<void>(f, args, regs); break;
      case 3: _call_external_3<void>(f, args, regs); break;
      case 4: _call_external_4<void>(f, args, regs); break;
      default: break;
      }
    }

  void call_external(const external_function& f, registers& regs)
    {
    switch (f.return_type)
      {
      case external_function::T_BOOL:
      case external_function::T_CHAR_POINTER:
      case external_function::T_INT64:
      {
      regs.rax = _call_external<uint64_t>(f, regs);
      break;
      }
      case external_function::T_DOUBLE:
      {
      regs.xmm0 = _call_external<double>(f, regs);
      break;
      }
      case external_function::T_VOID:
      {
      _call_external<void>(f, regs);
      break;
      }
      }
    }

  } // namespace
void run_bytecode(const uint8_t* bytecode, uint64_t size, registers& regs, const std::vector<external_function>& externals)
  {
  (void*)size;
  *((uint64_t*)regs.rsp - 1) = 0xffffffffffffffff; // this address means the function call representing this bytecode
  regs.rsp -= 8;
  const uint8_t* bytecode_ptr = bytecode;

  for (;;)
    {
    vmcode::operation op;
    vmcode::operand operand1;
    vmcode::operand operand2;
    vmcode::operand operand3;
    uint64_t operand1_mem;
    uint64_t operand2_mem;
    uint64_t operand3_mem;
    uint64_t sz = disassemble_bytecode(op, operand1, operand2, operand3, operand1_mem, operand2_mem, operand3_mem, bytecode_ptr);

    //print(op, operand1, operand2, operand1_mem, operand2_mem);

    switch (op)
      {
      case vmcode::ADD:
      {
      execute_operation<AddOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::ADDSD:
      {
      execute_double_operation<AddsdOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::AND:
      {
      execute_operation<AndOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CALL:
      {
      if (operand1 == vmcode::NUMBER) // local call
        {
        regs.rsp -= 8;
        *((uint64_t*)regs.rsp) = (uint64_t)(bytecode_ptr + sz); // save address right after call on stack
        uint32_t local_offset = (uint32_t)operand1_mem;
        bytecode_ptr += (int32_t)local_offset;
        sz = 0;
        }
      else // external call
        {
        uint64_t tmp;
        uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
        regs.rsp -= 8;
        *((uint64_t*)regs.rsp) = (uint64_t)(bytecode_ptr + sz); // save address right after call on stack
        bytecode_ptr = (const uint8_t*)(*oprnd1);
        sz = 0;
        //throw std::logic_error("external call not implemented");
        }
      break;
      }
      case vmcode::CALLEXTERNAL:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t address = *oprnd1;
      auto it = std::find_if(externals.begin(), externals.end(), [&](const external_function& f) { return f.address == address; });
      if (it == externals.end())
        throw std::logic_error("Call to unknown external function\n");
      call_external(*it, regs);
      break;
      }
      case vmcode::CMP:
      {
      compare_operation(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CMPEQPD:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      double v1 = *reinterpret_cast<double*>(oprnd1);
      double v2 = *reinterpret_cast<double*>(oprnd2);
      *oprnd1 = (v1 == v2) ? 0xffffffffffffffff : 0;
      break;
      }
      case vmcode::CMPLTPD:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      double v1 = *reinterpret_cast<double*>(oprnd1);
      double v2 = *reinterpret_cast<double*>(oprnd2);
      *oprnd1 = (v1 < v2) ? 0xffffffffffffffff : 0;
      break;
      }
      case vmcode::CMPLEPD:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      double v1 = *reinterpret_cast<double*>(oprnd1);
      double v2 = *reinterpret_cast<double*>(oprnd2);
      *oprnd1 = (v1 <= v2) ? 0xffffffffffffffff : 0;
      break;
      }
      case vmcode::CVTSI2SD:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      double v = (double)((int64_t)*oprnd2);
      *reinterpret_cast<double*>(oprnd1) = v;
      break;
      }
      case vmcode::CVTTSD2SI:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      double v = *reinterpret_cast<double*>(oprnd2);
      *oprnd1 = (int64_t)v;
      break;
      }
      case vmcode::CSQRT:
      {
      execute_double_operation<SqrtpdOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CSIN:
      {
      execute_double_operation<CSinOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CCOS:
      {
      execute_double_operation<CCosOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CEXP:
      {
      execute_double_operation<CExpOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CLOG:
      {
      execute_double_operation<CLogOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CLOG2:
      {
      execute_double_operation<CLog2Oper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CABS:
      {
      execute_double_operation<CAbsOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CTAN:
      {
      execute_double_operation<CTanOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CATAN:
      {
      execute_double_operation<CAtanOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CATAN2:
      {
      execute_double_operation<CAtan2Oper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CPOW:
      {
      execute_double_operation<CPowOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CMIN:
      {
      execute_double_operation<CMinOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::CMAX:
      {
      execute_double_operation<CMaxOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::SWAP:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd1);
      assert(oprnd2);
      uint64_t tmp2 = *oprnd1;
      *oprnd1 = *oprnd2;
      *oprnd2 = tmp2;
      break;
      }
      case vmcode::DEC:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      *oprnd1 -= 1;
      if (*oprnd1)
        regs.eflags &= ~zero_flag;
      else
        regs.eflags |= zero_flag;
      break;
      }
      case vmcode::DIV:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t divider = *oprnd1;
      uint64_t result = regs.rax / divider;
      uint64_t remainder = regs.rax % divider;
      regs.rax = result;
      regs.rdx = remainder;
      break;
      }
      case vmcode::DIV2:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      uint64_t result = *oprnd1 / *oprnd2;
      *oprnd1 = (uint64_t)result;
      break;
      }
      case vmcode::DIVSD:
      {
      execute_double_operation<DivsdOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::IDIV:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      int64_t divider = (int64_t)*oprnd1;
      int64_t result = (int64_t)regs.rax / divider;
      int64_t remainder = (int64_t)regs.rax % divider;
      regs.rax = result;
      regs.rdx = remainder;
      break;
      }
      case vmcode::IDIV2:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      int64_t result = (int64_t)*oprnd1 / (int64_t)*oprnd2;
      *oprnd1 = (uint64_t)result;
      break;
      }
      case vmcode::IMUL:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd1);
      if (oprnd2)
        {
        *oprnd1 *= (int64_t)(*oprnd2);
        }
      else
        {
        int64_t rax = (int64_t)regs.rax;
        rax *= (int64_t)(*oprnd1);
        regs.rax = rax;
        }
      break;
      }
      case vmcode::INC:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      *oprnd1 += 1;
      if (*oprnd1)
        regs.eflags &= ~zero_flag;
      else
        regs.eflags |= zero_flag;

      break;
      }
      case vmcode::JMP:
      {
      if (operand1 == vmcode::NUMBER)
        {
        int32_t local_offset = (int32_t)operand1_mem;
        bytecode_ptr += local_offset;
        sz = 0;
        }
      else
        {
        uint64_t tmp;
        uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
        bytecode_ptr = (const uint8_t*)(*oprnd1);
        sz = 0;
        }
      break;
      }
      case vmcode::JA:
      {
      if (((regs.eflags & zero_flag) | (regs.eflags & carry_flag)) == 0)
        {
        assert(operand1 == vmcode::NUMBER);
        int32_t local_offset = (int32_t)operand1_mem;
        bytecode_ptr += local_offset;
        sz = 0;
        }
      break;
      }
      case vmcode::JB:
      {
      if (regs.eflags & carry_flag)
        {
        assert(operand1 == vmcode::NUMBER);
        int32_t local_offset = (int32_t)operand1_mem;
        bytecode_ptr += local_offset;
        sz = 0;
        }
      break;
      }
      case vmcode::JE:
      {
      if (regs.eflags & zero_flag)
        {
        assert(operand1 == vmcode::NUMBER);
        int32_t local_offset = (int32_t)operand1_mem;
        bytecode_ptr += local_offset;
        sz = 0;
        }
      break;
      }
      case vmcode::JL:
      {
      if (((regs.eflags & sign_flag) ^ (regs.eflags & overflow_flag)))
        {
        assert(operand1 == vmcode::NUMBER);
        int32_t local_offset = (int32_t)operand1_mem;
        bytecode_ptr += local_offset;
        sz = 0;
        }
      break;
      }
      case vmcode::JLE:
      {
      if ((((regs.eflags & sign_flag) ^ (regs.eflags & overflow_flag)) | (regs.eflags & zero_flag)))
        {
        assert(operand1 == vmcode::NUMBER);
        int32_t local_offset = (int32_t)operand1_mem;
        bytecode_ptr += local_offset;
        sz = 0;
        }
      break;
      }
      case vmcode::JG:
      {
      if ((((regs.eflags & sign_flag) ^ (regs.eflags & overflow_flag)) | (regs.eflags & zero_flag)) == 0)
        {
        assert(operand1 == vmcode::NUMBER);
        int32_t local_offset = (int32_t)operand1_mem;
        bytecode_ptr += local_offset;
        sz = 0;
        }
      break;
      }
      case vmcode::JGE:
      {
      if (((regs.eflags & sign_flag) ^ (regs.eflags & overflow_flag)) == 0)
        {
        assert(operand1 == vmcode::NUMBER);
        int32_t local_offset = (int32_t)operand1_mem;
        bytecode_ptr += local_offset;
        sz = 0;
        }
      break;
      }
      case vmcode::JNE:
      {
      if ((regs.eflags & zero_flag) == 0)
        {
        assert(operand1 == vmcode::NUMBER);
        int32_t local_offset = (int32_t)operand1_mem;
        bytecode_ptr += local_offset;
        sz = 0;
        }
      break;
      }
      case vmcode::LEA:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd2);
      *oprnd1 = (uint64_t)oprnd2;
      break;
      }
      case vmcode::MOD:
      {
      execute_operation<ModOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::MODSD:
      {
      execute_double_operation<ModsdOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::MOV:
      {
      execute_operation<MovOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::MOVMSKPD:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      *oprnd1 = (*oprnd2) ? 1 : 0;
      break;
      }
      case vmcode::MUL:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      regs.rax *= (uint64_t)(*oprnd1);
      break;
      }
      case vmcode::MULSD:
      {
      execute_double_operation<MulsdOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::NEG:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      int64_t val = (int64_t)(*oprnd1);
      *oprnd1 = -val;
      break;
      }
      case vmcode::NOP: break;
      case vmcode::OR:
      {
      execute_operation<OrOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::POP:
      {
      uint64_t tmp;
      uint64_t address = *((uint64_t*)regs.rsp);
      regs.rsp += 8;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      *oprnd1 = address;
      break;
      }
      case vmcode::PUSH:
      {
      uint64_t tmp;
      regs.rsp -= 8;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      if (oprnd1)
        *((uint64_t*)regs.rsp) = *oprnd1;
      else if (operand1 == vmcode::NUMBER || operand1 == vmcode::LABELADDRESS)
        *((uint64_t*)regs.rsp) = operand1_mem;
      break;
      }
      case vmcode::RET:
      {
      uint64_t address = *((uint64_t*)regs.rsp);
      regs.rsp += 8; // to check, might need to pop more
      if (address == 0xffffffffffffffff) // we're at the end of this bytecode function call
        {
        reset_registers(regs);
        return;
        }
      bytecode_ptr = (const uint8_t*)address;
      sz = 0;
      break;
      }
      case vmcode::SAL:
      {
      execute_operation<ShlOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::SAR:
      {
      execute_operation<SarOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::SETE:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      if (regs.eflags & zero_flag)
        *oprnd1 = 1;
      else
        *oprnd1 = 0;
      break;
      }
      case vmcode::SETNE:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      if (regs.eflags & zero_flag)
        *oprnd1 = 0;
      else
        *oprnd1 = 1;
      break;
      }
      case vmcode::SETL:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      if (((regs.eflags & sign_flag) ^ (regs.eflags & overflow_flag)))
        *oprnd1 = 1;
      else
        *oprnd1 = 0;
      break;
      }
      case vmcode::SETG:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      if ((((regs.eflags & sign_flag) ^ (regs.eflags & overflow_flag)) | (regs.eflags & zero_flag)) == 0)
        *oprnd1 = 1;
      else
        *oprnd1 = 0;
      break;
      }
      case vmcode::SETLE:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      if ((((regs.eflags & sign_flag) ^ (regs.eflags & overflow_flag)) | (regs.eflags & zero_flag)) == 0)
        *oprnd1 = 0;
      else
        *oprnd1 = 1;
      break;
      }
      case vmcode::SETGE:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      if (((regs.eflags & sign_flag) ^ (regs.eflags & overflow_flag)))
        *oprnd1 = 0;
      else
        *oprnd1 = 1;
      break;
      }
      case vmcode::SHL:
      {
      execute_operation<ShlOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::SHR:
      {
      execute_operation<ShrOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::SQRTPD:
      {
      execute_double_operation<SqrtpdOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::SUB:
      {
      execute_operation<SubOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::SUBSD:
      {
      execute_double_operation<SubsdOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::TEST:
      {
      uint64_t tmp = execute_operation_const<AndOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      if (tmp)
        {
        regs.eflags &= ~zero_flag;
        if ((int64_t)tmp < 0)
          regs.eflags |= sign_flag;
        else
          regs.eflags &= ~sign_flag;
        }
      else
        {
        regs.eflags |= zero_flag;
        regs.eflags &= ~sign_flag;
        }
      break;
      }
      case vmcode::UCOMISD:
      {
      uint64_t tmp;
      double* oprnd1 = (double*)get_address_64bit(operand1, operand1_mem, regs, &tmp);
      double* oprnd2 = (double*)get_address_64bit(operand2, operand2_mem, regs, &tmp);
      if (*oprnd1 != *oprnd1 || *oprnd2 != *oprnd2)
        {
        regs.eflags = zero_flag | carry_flag;
        }
      else if (*oprnd1 < *oprnd2)
        {
        regs.eflags = carry_flag;
        }
      else if (*oprnd2 < *oprnd1)
        {
        regs.eflags = 0;
        }
      else
        {
        regs.eflags = zero_flag;
        }

      break;
      }
      case vmcode::XOR:
      {
      execute_operation<XorOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::XORPD:
      {
      execute_double_operation<XorpdOper>(operand1, operand2, operand1_mem, operand2_mem, regs);
      break;
      }
      case vmcode::MOVADD:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd2);
      uint64_t tmp2;
      uint64_t* oprnd3 = get_address_64bit(operand3, operand3_mem, regs, &tmp2);
      assert(oprnd3);
      *oprnd1 = *oprnd2 + *oprnd3;
      break;
      }
      case vmcode::MOVSUB:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd2);
      uint64_t tmp2;
      uint64_t* oprnd3 = get_address_64bit(operand3, operand3_mem, regs, &tmp2);
      assert(oprnd3);
      *oprnd1 = *oprnd2 - *oprnd3;
      break;
      }
      case vmcode::MOVMUL:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd2);
      uint64_t tmp2;
      uint64_t* oprnd3 = get_address_64bit(operand3, operand3_mem, regs, &tmp2);
      assert(oprnd3);
      *oprnd1 = *oprnd2 * *oprnd3;
      break;
      }
      case vmcode::MOVDIV:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd2);
      uint64_t tmp2;
      uint64_t* oprnd3 = get_address_64bit(operand3, operand3_mem, regs, &tmp2);
      assert(oprnd3);
      *oprnd1 = *oprnd2 / *oprnd3;
      break;
      }
      case vmcode::MOVSHL:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd2);
      uint64_t tmp2;
      uint64_t* oprnd3 = get_address_64bit(operand3, operand3_mem, regs, &tmp2);
      assert(oprnd3);
      *oprnd1 = *oprnd2 << *oprnd3;
      break;
      }
      case vmcode::MOVSHR:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd2);
      uint64_t tmp2;
      uint64_t* oprnd3 = get_address_64bit(operand3, operand3_mem, regs, &tmp2);
      assert(oprnd3);
      *oprnd1 = *oprnd2 >> *oprnd3;
      break;
      }
      case vmcode::ADDSHL:
      {
      uint64_t tmp;
      uint64_t* oprnd1 = get_address_64bit(operand1, operand1_mem, regs, &tmp);
      assert(oprnd1);
      uint64_t* oprnd2 = get_address_64bit(operand2, operand2_mem, regs, &tmp);
      assert(oprnd2);
      uint64_t tmp2;
      uint64_t* oprnd3 = get_address_64bit(operand3, operand3_mem, regs, &tmp2);
      assert(oprnd3);
      *oprnd2 <<= *oprnd3;
      *oprnd1 += *oprnd2;
      break;
      }
      default:
      {
      std::stringstream str;
      str << vmcode::operation_to_string(op) << " is not implemented yet!";
      throw std::logic_error(str.str());
      }
      }
    bytecode_ptr += sz;
    }
  }

VM_END
