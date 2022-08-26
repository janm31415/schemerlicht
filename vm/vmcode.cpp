#include "vmcode.h"
#include <sstream>

VM_BEGIN

namespace
  {
  std::string uchar_to_hex(unsigned char i)
    {
    std::string hex;
    int h1 = (i >> 4) & 0x0f;
    if (h1 < 10)
      hex += '0' + char(h1);
    else
      hex += 'A' + char(h1) - 10;
    int h2 = (i) & 0x0f;
    if (h2 < 10)
      hex += '0' + char(h2);
    else
      hex += 'A' + char(h2) - 10;
    return hex;
    }

  std::string uint64_to_hex(uint64_t i)
    {
    if (i == 0)
      return std::string("0x00");
    unsigned char* ptr = (unsigned char*)&i;
    std::string out = "0x";
    bool skip = true;
    for (int j = 7; j >= 0; --j)
      {
      if (ptr[j])
        skip = false;
      if (!skip)
        out += uchar_to_hex(ptr[j]);
      }
    return out;
    }

  }

vmcode::vmcode()
  {
  instructions_list.emplace_back();
  instructions_list_stack.push_back(instructions_list.begin());
  }

vmcode::~vmcode()
  {
  }

void vmcode::push()
  {
  instructions_list.emplace_back();
  instructions_list_stack.push_back(--instructions_list.end());
  }

void vmcode::pop()
  {
  instructions_list_stack.pop_back();
  }

void vmcode::clear()
  {
  instructions_list_stack.clear();
  instructions_list.clear();
  instructions_list.emplace_back();
  instructions_list_stack.push_back(instructions_list.begin());
  }

const std::list<std::vector<vmcode::instruction>>& vmcode::get_instructions_list() const
  {
  return instructions_list;
  }

std::list<std::vector<vmcode::instruction>>& vmcode::get_instructions_list()
  {
  return instructions_list;
  }

void vmcode::stream(std::ostream& out) const
  {
  for (auto rit = instructions_list.rbegin(); rit != instructions_list.rend(); ++rit)
    {
    for (const auto& ins : *rit)
      {
      ins.stream(out);
      }
    }
  }

vmcode::instruction::instruction() : oper(NOP), operand1(EMPTY), operand2(EMPTY), operand1_mem(0), operand2_mem(0), operand3(EMPTY), operand3_mem(0)
  {
  }

vmcode::instruction::instruction(const std::string& txt) : oper(COMMENT), operand1(EMPTY), operand2(EMPTY), operand1_mem(0), operand2_mem(0), text(txt), operand3(EMPTY), operand3_mem(0)
  {
  }

vmcode::instruction::instruction(operation op) : oper(op), operand1(EMPTY), operand2(EMPTY), operand1_mem(0), operand2_mem(0), operand3(EMPTY), operand3_mem(0)
  {
  }

vmcode::instruction::instruction(operation op, operand op1) : oper(op), operand1(op1), operand2(EMPTY), operand1_mem(0), operand2_mem(0), operand3(EMPTY), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, operand op2) : oper(op), operand1(op1), operand2(op2), operand1_mem(0), operand2_mem(0), operand3(EMPTY), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, operand op2, operand op3) : oper(op), operand1(op1), operand2(op2), operand1_mem(0), operand2_mem(0), operand3(op3), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, uint64_t op1_mem) : oper(op), operand1(op1), operand2(EMPTY), operand1_mem(op1_mem), operand2_mem(0), operand3(EMPTY), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, uint64_t op1_mem, operand op2) : oper(op), operand1(op1), operand2(op2), operand1_mem(op1_mem), operand2_mem(0), operand3(EMPTY), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, uint64_t op1_mem, operand op2, uint64_t op2_mem) : oper(op), operand1(op1), operand2(op2), operand1_mem(op1_mem), operand2_mem(op2_mem), operand3(EMPTY), operand3_mem(0)
  {
  }

vmcode::instruction::instruction(operation op, operand op1, operand op2, uint64_t op2_mem) : oper(op), operand1(op1), operand2(op2), operand1_mem(0), operand2_mem(op2_mem), operand3(EMPTY), operand3_mem(0)
  {
  }

vmcode::instruction::instruction(operation op, const std::string& txt) : oper(op), operand1(EMPTY), operand2(EMPTY), operand1_mem(0), operand2_mem(0), text(txt), operand3(EMPTY), operand3_mem(0)
  {
  }

vmcode::instruction::instruction(operation op, operand op1, const std::string& txt) : oper(op), operand1(op1), operand2(EMPTY), operand1_mem(0), operand2_mem(0), text(txt), operand3(EMPTY), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, operand op2, const std::string& txt) : oper(op), operand1(op1), operand2(op2), operand1_mem(0), operand2_mem(0), text(txt), operand3(EMPTY), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, uint64_t op1_mem, const std::string& txt) : oper(op), operand1(op1), operand2(EMPTY), operand1_mem(op1_mem), operand2_mem(0), text(txt), operand3(EMPTY), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, uint64_t op1_mem, operand op2, const std::string& txt) : oper(op), operand1(op1), operand2(op2), operand1_mem(op1_mem), operand2_mem(0), text(txt), operand3(EMPTY), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, uint64_t op1_mem, operand op2, uint64_t op2_mem, const std::string& txt) : oper(op), operand1(op1), operand2(op2), operand1_mem(op1_mem), operand2_mem(op2_mem), text(txt), operand3(EMPTY), operand3_mem(0)
  {

  }

vmcode::instruction::instruction(operation op, operand op1, operand op2, uint64_t op2_mem, const std::string& txt) : oper(op), operand1(op1), operand2(op2), operand1_mem(0), operand2_mem(op2_mem), text(txt), operand3(EMPTY), operand3_mem(0)
  {

  }

namespace
  {
  std::string _to_string(uint64_t mem)
    {
    std::stringstream str;
    str << mem;
    return str.str();
    }

  std::string _to_string_signed(int64_t mem)
    {
    std::stringstream str;
    str << mem;
    return str.str();
    }

  std::string _operation2string(vmcode::operation op)
    {
    switch (op)
      {
      case vmcode::ADD: return "add";
      case vmcode::ADDSD: return "addsd";
      case vmcode::AND: return "and";
      case vmcode::CMP: return "cmp";
      case vmcode::CMPEQPD: return "cmpeqpd";
      case vmcode::CMPLTPD: return "cmpltpd";
      case vmcode::CMPLEPD: return "cmplepd";
      case vmcode::CVTSI2SD: return "cvtsi2sd";
      case vmcode::CVTTSD2SI: return "cvttsd2si";
      case vmcode::CSQRT:return std::string("csqrt");
      case vmcode::CSIN:return std::string("csin");
      case vmcode::CCOS:return std::string("ccos");
      case vmcode::CEXP:return std::string("cexp");
      case vmcode::CLOG:return std::string("clog");
      case vmcode::CLOG2:return std::string("clog2");
      case vmcode::CABS:return std::string("cabs");
      case vmcode::CTAN:return std::string("ctan");
      case vmcode::CATAN:return std::string("catan");
      case vmcode::CATAN2:return std::string("catan2");
      case vmcode::CPOW:return std::string("cpow");
      case vmcode::CMIN:return std::string("cmin");
      case vmcode::CMAX:return std::string("cmax");
      case vmcode::SWAP:return std::string("swap");
      case vmcode::DEC: return "dec";
      case vmcode::DIV: return "div";
      case vmcode::DIV2: return "div2";
      case vmcode::DIVSD: return "divsd";      
      case vmcode::IDIV: return "idiv";
      case vmcode::IDIV2: return "idiv2";
      case vmcode::IMUL: return "imul";
      case vmcode::INC: return "inc";
      case vmcode::LEA: return "lea";
      case vmcode::MOD: return "mod";
      case vmcode::MODSD: return "modsd";
      case vmcode::MOV: return "mov";
      case vmcode::MOVMSKPD: return "movmskpd";
      case vmcode::MUL: return "mul";
      case vmcode::MULSD: return "mulsd";
      case vmcode::NEG: return "neg";
      case vmcode::NOP: return "nop";
      case vmcode::OR: return "or";
      case vmcode::POP: return "pop";
      case vmcode::PUSH: return "push";
      case vmcode::RET: return "ret";
      case vmcode::SAL: return "sal";
      case vmcode::SAR: return "sar";
      case vmcode::SHL: return "shl";
      case vmcode::SETE: return "sete";
      case vmcode::SETNE: return "setne";
      case vmcode::SETL: return "setl";
      case vmcode::SETG: return "setg";
      case vmcode::SETLE: return "setle";
      case vmcode::SETGE: return "setge";
      case vmcode::SHR: return "shr";
      case vmcode::SQRTPD: return "sqrtpd";
      case vmcode::SUB: return "sub";
      case vmcode::SUBSD: return "subsd";
      case vmcode::TEST: return "test";
      case vmcode::UCOMISD: return "ucomisd";
      case vmcode::XOR: return "xor";
      case vmcode::XORPD: return "xorpd";
      case vmcode::MOVADD: return "movadd";
      case vmcode::MOVSUB: return "movsub";
      case vmcode::MOVMUL: return "movmul";
      case vmcode::MOVDIV: return "movdiv";
      case vmcode::MOVSHL: return "movshl";
      case vmcode::MOVSHR: return "movshr";
      case vmcode::ADDSHL: return "addshl";
      }
    return "";
    }

  std::string _operand2string(vmcode::operand op, uint64_t mem, const std::string& text)
    {
    switch (op)
      {
      case vmcode::RAX: return "rax";
      case vmcode::RBX: return "rbx";
      case vmcode::RCX: return "rcx";
      case vmcode::RDX: return "rdx";
      case vmcode::RDI: return "rdi";
      case vmcode::RSI: return "rsi";
      case vmcode::RSP: return "rsp";
      case vmcode::RBP: return "rbp";
      case vmcode::R8: return "r8";
      case vmcode::R9: return "r9";
      case vmcode::R10: return "r10";
      case vmcode::R11: return "r11";
      case vmcode::R12: return "r12";
      case vmcode::R13: return "r13";
      case vmcode::R14: return "r14";
      case vmcode::R15: return "r15";
      case vmcode::XMM0: return "xmm0";
      case vmcode::XMM1: return "xmm1";
      case vmcode::XMM2: return "xmm2";
      case vmcode::XMM3: return "xmm3";
      case vmcode::XMM4: return "xmm4";
      case vmcode::XMM5: return "xmm5";
      case vmcode::XMM6: return "xmm6";
      case vmcode::XMM7: return "xmm7";
      case vmcode::XMM8: return "xmm8";
      case vmcode::XMM9: return "xmm9";
      case vmcode::XMM10: return "xmm10";
      case vmcode::XMM11: return "xmm11";
      case vmcode::XMM12: return "xmm12";
      case vmcode::XMM13: return "xmm13";
      case vmcode::XMM14: return "xmm14";
      case vmcode::XMM15: return "xmm15";
      case vmcode::NUMBER: return uint64_to_hex(mem);
      case vmcode::MEM_RAX: return mem ? ("[rax" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[rax]";
      case vmcode::MEM_RBX: return mem ? ("[rbx" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[rbx]";
      case vmcode::MEM_RCX: return mem ? ("[rcx" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[rcx]";
      case vmcode::MEM_RDX: return mem ? ("[rdx" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[rdx]";
      case vmcode::MEM_RDI: return mem ? ("[rdi" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[rdi]";
      case vmcode::MEM_RSI: return mem ? ("[rsi" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[rsi]";
      case vmcode::MEM_RSP: return mem ? ("[rsp" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[rsp]";
      case vmcode::MEM_RBP: return mem ? ("[rbp" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[rbp]";
      case vmcode::MEM_R8: return  mem ? ("[r8" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[r8]";
      case vmcode::MEM_R9: return  mem ? ("[r9" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[r9]";
      case vmcode::MEM_R10: return mem ? ("[r10" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[r10]";
      case vmcode::MEM_R11: return mem ? ("[r11" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[r11]";
      case vmcode::MEM_R12: return mem ? ("[r12" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[r13]";
      case vmcode::MEM_R13: return mem ? ("[r13" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[r13]";
      case vmcode::MEM_R14: return mem ? ("[r14" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[r14]";
      case vmcode::MEM_R15: return mem ? ("[r15" + ((int64_t(mem)) > 0 ? ("+" + _to_string(mem)) : _to_string_signed(mem)) + "]") : "[r15]";
      case vmcode::LABELADDRESS: return text;
      }
    return "";
    }
  }

void vmcode::instruction::stream(std::ostream& out) const
  {
  switch (oper)
    {
    case COMMENT:
    {
    out << "; " << text << std::endl;
    break;
    }
    case GLOBAL:
    {
    out << "global " << text << std::endl;
    out << "SECTION .text" << std::endl;
    out << text << ":" << std::endl;
    break;
    }
    case LABEL:
    {
    out << text << ":" << std::endl;
    break;
    }
    case LABEL_ALIGNED:
    {
    out << text << ":" << std::endl;
    break;
    }
    case EXTERN:
    {
    out << "extern " << text << std::endl;
    break;
    }
    case CALLEXTERNAL:
    case CALL:
    {
    out << "\tcall";
    if (operand1 != vmcode::EMPTY)
      {
      out << " " << _operand2string(operand1, operand1_mem, text);
      }
    else
      out << " " << text;
    out << std::endl;
    break;
    }
    case JMP:
    {
    out << "\tjmp";
    if (operand1 != vmcode::EMPTY)
      {
      out << " " << _operand2string(operand1, operand1_mem, text);
      }
    else
      out << " " << text;
    out << std::endl;
    break;
    }
    case JE:
    {
    out << "\tje " << text << std::endl;
    break;
    }
    case JNE:
    {
    out << "\tjne " << text << std::endl;
    break;
    }
    case JL:
    {
    out << "\tjl " << text << std::endl;
    break;
    }
    case JLE:
    {
    out << "\tjle " << text << std::endl;
    break;
    }
    case JA:
    {
    out << "\tja " << text << std::endl;
    break;
    }
    case JB:
    {
    out << "\tjb " << text << std::endl;
    break;
    }
    case JG:
    {
    out << "\tjg " << text << std::endl;
    break;
    }
    case JGE:
    {
    out << "\tjge " << text << std::endl;
    break;
    }   
    default:
    {
    out << "\t" << _operation2string(oper);
    if (operand1 != vmcode::EMPTY)
      {
      out << " " << _operand2string(operand1, operand1_mem, text);
      }
    if (operand2 != vmcode::EMPTY)
      {
      out << ", " << _operand2string(operand2, operand2_mem, text);
      }
    if (operand3 != vmcode::EMPTY)
      {
      out << ", " << _operand2string(operand3, operand3_mem, text);
      }
    out << std::endl;
    break;
    }
    }
  }

std::string vmcode::operation_to_string(operation oper)
  {
  switch (oper)
    {
    case vmcode::ADD: return std::string("ADD");
    case vmcode::ADDSD: return std::string("ADDSD");
    case vmcode::AND: return std::string("AND");
    case vmcode::CALL:return std::string("CALL");
    case vmcode::CALLEXTERNAL:return std::string("CALLEXTERNAL");
    case vmcode::COMMENT: return std::string("COMMENT");
    case vmcode::CMP: return std::string("CMP");
    case vmcode::CMPEQPD: return std::string("CMPEQPD");
    case vmcode::CMPLTPD: return std::string("CMPLTPD");
    case vmcode::CMPLEPD: return std::string("CMPLEPD");
    case vmcode::CVTSI2SD: return std::string("CVTSI2SD");
    case vmcode::CVTTSD2SI:return std::string("CVTTSD2SI");
    case vmcode::CSQRT:return std::string("CSQRT");
    case vmcode::CSIN:return std::string("CSIN");
    case vmcode::CCOS:return std::string("CCOS");
    case vmcode::CEXP:return std::string("CEXP");
    case vmcode::CLOG:return std::string("CLOG");
    case vmcode::CLOG2:return std::string("CLOG2");
    case vmcode::CABS:return std::string("CABS");
    case vmcode::CTAN:return std::string("CTAN");
    case vmcode::CATAN:return std::string("CATAN");
    case vmcode::CATAN2:return std::string("CATAN2");
    case vmcode::CPOW:return std::string("CPOW");
    case vmcode::CMIN:return std::string("CMIN");
    case vmcode::CMAX:return std::string("CMAX");
    case vmcode::SWAP:return std::string("SWAP");
    case vmcode::DEC: return std::string("DEC");
    case vmcode::DIV: return std::string("DIV");
    case vmcode::DIV2: return std::string("DIV2");
    case vmcode::DIVSD: return std::string("DIVSD");
    case vmcode::EXTERN: return std::string("EXTERN");  
    case vmcode::GLOBAL:return std::string("GLOBAL");
    case vmcode::LABEL:return std::string("LABEL");
    case vmcode::LABEL_ALIGNED:return std::string("LABEL_ALIGNED");
    case vmcode::IDIV:return std::string("IDIV");
    case vmcode::IDIV2:return std::string("IDIV2");
    case vmcode::IMUL:return std::string("IMUL");
    case vmcode::INC: return std::string("INC");
    case vmcode::JE: return std::string("JE");
    case vmcode::JL: return std::string("JL");
    case vmcode::JLE:return std::string("JLE");
    case vmcode::JA: return std::string("JA");
    case vmcode::JB: return std::string("JB");
    case vmcode::JG: return std::string("JG");
    case vmcode::JGE:return std::string("JGE");
    case vmcode::JNE:return std::string("JNE");
    case vmcode::JMP:return std::string("JMP"); 
    case vmcode::LEA: return std::string("LEA");
    case vmcode::MOD: return std::string("MOD");
    case vmcode::MODSD: return std::string("MODSD");
    case vmcode::MOV: return std::string("MOV");
    case vmcode::MOVMSKPD: return std::string("MOVMSKPD");
    case vmcode::MUL: return std::string("MUL");
    case vmcode::MULSD:return std::string("MULSD");
    case vmcode::NEG:return std::string("NEG");
    case vmcode::NOP:return std::string("NOP");
    case vmcode::OR:return std::string("OR");
    case vmcode::POP: return std::string("POP");
    case vmcode::PUSH: return std::string("PUSH");
    case vmcode::RET: return std::string("RET");
    case vmcode::SAL: return std::string("SAL");
    case vmcode::SAR: return std::string("SAR");
    case vmcode::SETE: return std::string("SETE");
    case vmcode::SETNE:return std::string("SETNE");
    case vmcode::SETL: return std::string("SETL");
    case vmcode::SETG: return std::string("SETG");
    case vmcode::SETLE:return std::string("SETLE");
    case vmcode::SETGE:return std::string("SETGE");
    case vmcode::SHL: return std::string("SHL");
    case vmcode::SHR:return std::string("SHR");
    case vmcode::SQRTPD:return std::string("SQRTPD");
    case vmcode::SUB: return std::string("SUB");
    case vmcode::SUBSD: return std::string("SUBSD");
    case vmcode::TEST: return std::string("TEST");
    case vmcode::UCOMISD:return std::string("UCOMISD");
    case vmcode::XOR: return std::string("XOR");
    case vmcode::XORPD: return std::string("XORPD");
    case vmcode::MOVADD: return std::string("MOVADD");
    case vmcode::MOVSUB: return std::string("MOVSUB");
    case vmcode::MOVMUL: return std::string("MOVMUL");
    case vmcode::MOVDIV: return std::string("MOVDIV");
    case vmcode::MOVSHL: return std::string("MOVSHL");
    case vmcode::MOVSHR: return std::string("MOVSHR");
    case vmcode::ADDSHL: return std::string("ADDSHL");
    }
  return std::string();
  }

std::string vmcode::operand_to_string(operand op)
  {
  switch (op)
    {
    case vmcode::EMPTY: return std::string("EMPTY");
    case vmcode::RAX: return std::string("RAX");
    case vmcode::RBX: return std::string("RBX");
    case vmcode::RCX: return std::string("RCX");
    case vmcode::RDX: return std::string("RDX");
    case vmcode::RDI: return std::string("RDI");
    case vmcode::RSI: return std::string("RSI");
    case vmcode::RSP: return std::string("RSP");
    case vmcode::RBP: return std::string("RBP");
    case vmcode::R8:  return std::string("R8");
    case vmcode::R9:  return std::string("R9");
    case vmcode::R10: return std::string("R10");
    case vmcode::R11: return std::string("R11");
    case vmcode::R12: return std::string("R12");
    case vmcode::R13: return std::string("R13");
    case vmcode::R14: return std::string("R14");
    case vmcode::R15: return std::string("R15");
    case vmcode::MEM_RAX: return std::string("MEM_RAX");
    case vmcode::MEM_RBX: return std::string("MEM_RBX");
    case vmcode::MEM_RCX: return std::string("MEM_RCX");
    case vmcode::MEM_RDX: return std::string("MEM_RDX");
    case vmcode::MEM_RDI: return std::string("MEM_RDI");
    case vmcode::MEM_RSI: return std::string("MEM_RSI");
    case vmcode::MEM_RSP: return std::string("MEM_RSP");
    case vmcode::MEM_RBP: return std::string("MEM_RBP");
    case vmcode::MEM_R8:  return std::string("MEM_R8");
    case vmcode::MEM_R9:  return std::string("MEM_R9");
    case vmcode::MEM_R10: return std::string("MEM_R10");
    case vmcode::MEM_R11: return std::string("MEM_R11");
    case vmcode::MEM_R12: return std::string("MEM_R12");
    case vmcode::MEM_R13: return std::string("MEM_R13");
    case vmcode::MEM_R14: return std::string("MEM_R14");
    case vmcode::MEM_R15: return std::string("MEM_R15");
    case vmcode::NUMBER: return std::string("NUMBER");
    case vmcode::XMM0: return std::string("XMM0");
    case vmcode::XMM1: return std::string("XMM1");
    case vmcode::XMM2: return std::string("XMM2");
    case vmcode::XMM3: return std::string("XMM3");
    case vmcode::XMM4: return std::string("XMM4");
    case vmcode::XMM5: return std::string("XMM5");
    case vmcode::XMM6: return std::string("XMM6");
    case vmcode::XMM7: return std::string("XMM7");
    case vmcode::XMM8: return std::string("XMM8");
    case vmcode::XMM9: return std::string("XMM9");
    case vmcode::XMM10:return std::string("XMM10");
    case vmcode::XMM11:return std::string("XMM11");
    case vmcode::XMM12:return std::string("XMM12");
    case vmcode::XMM13:return std::string("XMM13");
    case vmcode::XMM14:return std::string("XMM14");
    case vmcode::XMM15:return std::string("XMM15");
    case vmcode::LABELADDRESS: return std::string("LABELADDRESS");
    }
  return std::string();
  }

VM_END
