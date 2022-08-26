///////////////////////////////////////////////////////////////////////////////
// Includes
/////////////////////////////////////////////////////////////////////////////////

#include "vmtests.h"
#include <stdint.h>
#include <iostream>
#include "vm/vmcode.h"
#include "vm/vm.h"
#include "test_assert.h"
#include <chrono>

VM_BEGIN

namespace
  {
  void test_vm_mov_bytecode()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::RCX);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    TEST_EQ(3, size);
    TEST_EQ((int)vmcode::MOV, (int)f[0]);
    TEST_EQ((int)vmcode::RAX | operand_has_8bit_mem, (int)f[1]);
    TEST_EQ((int)vmcode::RCX | operand_has_8bit_mem, (int)f[2]);

    vmcode::operation op;
    vmcode::operand operand1;
    vmcode::operand operand2;
    vmcode::operand operand3;
    uint64_t operand1_mem;
    uint64_t operand2_mem;
    uint64_t operand3_mem;
    uint64_t sz = disassemble_bytecode(op, operand1, operand2, operand3, operand1_mem, operand2_mem, operand3_mem, f);
    TEST_EQ(3, sz);
    TEST_EQ(vmcode::MOV, op);
    TEST_EQ(vmcode::RAX, operand1);
    TEST_EQ(vmcode::RCX, operand2);
    TEST_EQ(vmcode::EMPTY, operand3);
    TEST_EQ(0, operand1_mem);
    TEST_EQ(0, operand2_mem);
    TEST_EQ(0, operand3_mem);

    free_bytecode(f, size);
    }

  void test_vm_movadd_bytecode()
    {
    vmcode code;
    code.add(vmcode::MOVADD, vmcode::RAX, vmcode::RCX, vmcode::RDX);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    TEST_EQ(4, size);
    TEST_EQ((int)vmcode::MOVADD, (int)f[0]);
    TEST_EQ((int)vmcode::RAX | operand_has_8bit_mem, (int)f[1]);
    TEST_EQ((int)vmcode::RCX | operand_has_8bit_mem, (int)f[2]);
    TEST_EQ((int)vmcode::RDX | operand_has_8bit_mem, (int)f[3]);

    vmcode::operation op;
    vmcode::operand operand1;
    vmcode::operand operand2;
    vmcode::operand operand3;
    uint64_t operand1_mem;
    uint64_t operand2_mem;
    uint64_t operand3_mem;
    uint64_t sz = disassemble_bytecode(op, operand1, operand2, operand3, operand1_mem, operand2_mem, operand3_mem, f);
    TEST_EQ(4, sz);
    TEST_EQ(vmcode::MOVADD, op);
    TEST_EQ(vmcode::RAX, operand1);
    TEST_EQ(vmcode::RCX, operand2);
    TEST_EQ(vmcode::RDX, operand3);
    TEST_EQ(0, operand1_mem);
    TEST_EQ(0, operand2_mem);
    TEST_EQ(0, operand3_mem);

    free_bytecode(f, size);
    }

  void test_vm_mov_bytecode_2()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0x6F77206F6C6C6548);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    TEST_EQ(12, size);
    TEST_EQ((int)vmcode::MOV, (int)f[0]);
    TEST_EQ((int)vmcode::RAX, (int)f[1]);
    TEST_EQ((int)vmcode::NUMBER, (int)f[2]);
    TEST_EQ(4 << 4, (int)f[3]);
    TEST_EQ(0x48, f[4]);
    TEST_EQ(0x65, f[5]);
    TEST_EQ(0x6C, f[6]);
    TEST_EQ(0x6C, f[7]);
    TEST_EQ(0x6F, f[8]);
    TEST_EQ(0x20, f[9]);
    TEST_EQ(0x77, f[10]);
    TEST_EQ(0x6F, f[11]);

    vmcode::operation op;
    vmcode::operand operand1;
    vmcode::operand operand2;
    vmcode::operand operand3;
    uint64_t operand1_mem;
    uint64_t operand2_mem;
    uint64_t operand3_mem;
    uint64_t sz = disassemble_bytecode(op, operand1, operand2, operand3, operand1_mem, operand2_mem, operand3_mem, f);
    TEST_EQ(12, sz);
    TEST_EQ(vmcode::MOV, op);
    TEST_EQ(vmcode::RAX, operand1);
    TEST_EQ(vmcode::NUMBER, operand2);
    TEST_EQ(0, operand1_mem);
    TEST_EQ(0x6F77206F6C6C6548, operand2_mem);

    free_bytecode(f, size);
    }

  void test_vm_nop_bytecode()
    {
    vmcode code;
    code.add(vmcode::NOP);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    TEST_EQ(1, size);
    TEST_EQ((int)vmcode::NOP, (int)f[0]);

    vmcode::operation op;
    vmcode::operand operand1;
    vmcode::operand operand2;
    vmcode::operand operand3;
    uint64_t operand1_mem;
    uint64_t operand2_mem;
    uint64_t operand3_mem;
    uint64_t sz = disassemble_bytecode(op, operand1, operand2, operand3, operand1_mem, operand2_mem, operand3_mem, f);
    TEST_EQ(1, sz);
    TEST_EQ(vmcode::NOP, op);
    
    free_bytecode(f, size);
    }

  void test_vm_lea()
    {
    vmcode code;
    code.add(vmcode::LEA, vmcode::RAX, vmcode::MEM_RSP, -16);
    code.add(vmcode::RET);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);

    registers reg;

    try {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }

    TEST_EQ(reg.rax, reg.rsp-16-8);
   
    free_bytecode(f, size);
    }

  void test_vm_ret()
    {
    vmcode code;
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }

    free_bytecode(f, size);
    }

  void test_vm_mov()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }

    TEST_EQ(10, reg.rax);

    free_bytecode(f, size);
    }

  void test_vm_mov_2()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, -10);
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }

    TEST_EQ(-10, reg.rax);

    free_bytecode(f, size);
    }

  void test_vm_add()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 12);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(22, reg.rax);
    TEST_EQ(12, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_call()
    {
    vmcode code;
    code.add(vmcode::SUB, vmcode::RSP, vmcode::NUMBER, 40);
    code.add(vmcode::CALL, "L_label");
    code.add(vmcode::ADD, vmcode::RSP, vmcode::NUMBER, 40);
    code.add(vmcode::RET);
    code.add(vmcode::LABEL, "L_label");
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 12);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(22, reg.rax);
    TEST_EQ(12, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_call_2()
    {
    vmcode code;
    code.add(vmcode::CALL, "L_label");
    code.add(vmcode::RET);
    code.add(vmcode::LABEL, "L_label_2");
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 12);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::RET);
    code.add(vmcode::LABEL, "L_label");
    code.add(vmcode::CALL, "L_label_2");
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(22, reg.rax);
    TEST_EQ(12, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_jmp()
    {
    vmcode code;
    code.add(vmcode::JMP, "L_label");
    code.add(vmcode::LABEL, "L_label");
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 12);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(22, reg.rax);
    TEST_EQ(12, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_call_aligned()
    {
    vmcode code;   
    code.add(vmcode::CALL, "L_label");
    code.add(vmcode::RET);
    code.add(vmcode::LABEL_ALIGNED, "L_label");
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 12);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(22, reg.rax);
    TEST_EQ(12, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_call_aligned_2()
    {
    vmcode code;
    code.add(vmcode::CALL, "L_label");
    code.add(vmcode::RET);
    code.add(vmcode::LABEL_ALIGNED, "L_label_2");
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 12);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::RET);
    code.add(vmcode::LABEL_ALIGNED, "L_label");
    code.add(vmcode::CALL, "L_label_2");
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(22, reg.rax);
    TEST_EQ(12, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_jmp_aligned()
    {
    vmcode code;
    code.add(vmcode::JMP, "L_label");
    code.add(vmcode::LABEL_ALIGNED, "L_label");
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 12);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(22, reg.rax);
    TEST_EQ(12, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_jmp_register()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RDX, vmcode::LABELADDRESS, "L_label");
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 12);
    code.add(vmcode::ADD, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::JMP, vmcode::RDX);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 0);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 0);
    code.add(vmcode::LABEL, "L_label");
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(22, reg.rax);
    TEST_EQ(12, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_shift()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RBX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::RDX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::R8, vmcode::NUMBER, -10);
    code.add(vmcode::MOV, vmcode::R9, vmcode::NUMBER, -10);
    code.add(vmcode::MOV, vmcode::R10, vmcode::NUMBER, -10);
    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, -10);

    code.add(vmcode::SHL, vmcode::RAX, vmcode::NUMBER, 1);
    code.add(vmcode::SAL, vmcode::RBX, vmcode::NUMBER, 1);
    code.add(vmcode::SHR, vmcode::RCX, vmcode::NUMBER, 1);
    code.add(vmcode::SAR, vmcode::RDX, vmcode::NUMBER, 1);

    code.add(vmcode::SHL, vmcode::R8, vmcode::NUMBER, 1);
    code.add(vmcode::SAL, vmcode::R9, vmcode::NUMBER, 1);
    code.add(vmcode::SHR, vmcode::R10, vmcode::NUMBER, 1);
    code.add(vmcode::SAR, vmcode::R11, vmcode::NUMBER, 1);

    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(20, reg.rax);
    TEST_EQ(20, reg.rbx);
    TEST_EQ(5, reg.rcx);
    TEST_EQ(5, reg.rdx);

    TEST_EQ(-20, reg.r8);
    TEST_EQ(-20, reg.r9);
    TEST_EQ(9223372036854775803, reg.r10);
    TEST_EQ(-5, reg.r11);
    free_bytecode(f, size);
    }

  void test_vm_jl()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, 5);
    code.add(vmcode::JL, "L_label");
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 3);
    code.add(vmcode::RET);
    code.add(vmcode::LABEL, "L_label");
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 4);
    code.add(vmcode::RET);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(3, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_jl_2()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, 20);
    code.add(vmcode::JL, "L_label");
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 3);
    code.add(vmcode::RET);
    code.add(vmcode::LABEL, "L_label");
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 4);
    code.add(vmcode::RET);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(4, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_jle()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::JLE, "L_label");
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 3);
    code.add(vmcode::RET);
    code.add(vmcode::LABEL, "L_label");
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 4);
    code.add(vmcode::RET);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(4, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_jge()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::CMP, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::JGE, "L_label");
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 3);
    code.add(vmcode::RET);
    code.add(vmcode::LABEL, "L_label");
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 4);
    code.add(vmcode::RET);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;

    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(4, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_push_pop()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::PUSH, vmcode::RAX);
    code.add(vmcode::POP, vmcode::RCX);
    code.add(vmcode::RET);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;
    reg.rax = 0;
    reg.rcx = 0;
    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(10, reg.rax);
    TEST_EQ(10, reg.rcx);
    free_bytecode(f, size);
    }

  void test_vm_movq()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 10);
    code.add(vmcode::MOV, vmcode::XMM0, vmcode::RAX);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 15);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::XMM0);    
    code.add(vmcode::RET);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;
    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(10, reg.rax);
    TEST_EQ(10, *reinterpret_cast<uint64_t*>(&reg.xmm0));
    free_bytecode(f, size);
    }

  void test_vm_addsd()
    {
    vmcode code;
    double v1 = 1.5;
    double v2 = 0.2;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, *reinterpret_cast<uint64_t*>(&v1));
    code.add(vmcode::MOV, vmcode::XMM0, vmcode::RAX);
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, *reinterpret_cast<uint64_t*>(&v2));
    code.add(vmcode::MOV, vmcode::XMM1, vmcode::RAX);
    code.add(vmcode::ADDSD, vmcode::XMM0, vmcode::XMM1);
    code.add(vmcode::RET);

    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;
    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(1.7, reg.xmm0);
    free_bytecode(f, size);
    }
  

  void test_vm_imul()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 12);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 13);
    code.add(vmcode::IMUL, vmcode::RCX);
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;
    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(156, reg.rax);
    free_bytecode(f, size);
    }

  void test_vm_imul2()
    {
    vmcode code;
    code.add(vmcode::MOV, vmcode::RAX, vmcode::NUMBER, 12);
    code.add(vmcode::MOV, vmcode::RCX, vmcode::NUMBER, 13);
    code.add(vmcode::IMUL, vmcode::RAX, vmcode::RCX);
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;
    try
      {
      run_bytecode(f, size, reg);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ(156, reg.rax);
    free_bytecode(f, size);
    }
  
  void test_vm_harmonic()
    {
    /*
    xorpd xmm0, xmm0
    mov r9, 0x01
    mov r10, 0x32
    mov r11, 0x3FF0000000000000
    mov xmm4, r11
  L_0:
    cmp r9, r10
    jge L_1
    cvtsi2sd xmm5, r9
    mov xmm2, xmm4
    divsd xmm2, xmm5
    addsd xmm0, xmm2
    inc r9
    jmp L_0
  L_1:
    ret
    */
    vmcode code;
    code.add(vmcode::XORPD, vmcode::XMM0, vmcode::XMM0);
    code.add(vmcode::MOV, vmcode::R9, vmcode::NUMBER, 1);
    code.add(vmcode::MOV, vmcode::R10, vmcode::NUMBER, 1000000);
    code.add(vmcode::MOV, vmcode::R11, vmcode::NUMBER, 0x3FF0000000000000);
    code.add(vmcode::MOV, vmcode::XMM4, vmcode::R11);
    code.add(vmcode::LABEL, "L_0");
    code.add(vmcode::CMP, vmcode::R9, vmcode::R10);
    code.add(vmcode::JGE, "L_1");
    code.add(vmcode::CVTSI2SD, vmcode::XMM5, vmcode::R9);
    code.add(vmcode::MOV, vmcode::XMM2, vmcode::XMM4);
    code.add(vmcode::DIVSD, vmcode::XMM2, vmcode::XMM5);
    code.add(vmcode::ADDSD, vmcode::XMM0, vmcode::XMM2);
    code.add(vmcode::INC, vmcode::R9);
    code.add(vmcode::JMP, "L_0");
    code.add(vmcode::LABEL, "L_1");
    code.add(vmcode::RET);
    uint64_t size;
    uint8_t* f = (uint8_t*)vm_bytecode(size, code);
    registers reg;
    try
      {
      auto tic = std::chrono::high_resolution_clock::now();
      run_bytecode(f, size, reg);
      auto toc = std::chrono::high_resolution_clock::now();
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(toc - tic).count();
      printf("Harmonic timing: %lldms\n", ms);
      }
    catch (std::logic_error e)
      {
      std::cout << e.what() << "\n";
      }
    TEST_EQ_CLOSE(14.3927, reg.xmm0, 1e-4);
    free_bytecode(f, size);
    }

  } // namespace

VM_END

void run_all_vm_tests()
  {
  using namespace VM;
  
  test_vm_mov_bytecode();  
  test_vm_mov_bytecode_2();
  test_vm_nop_bytecode();
  test_vm_movadd_bytecode();
  test_vm_ret();
  test_vm_mov();
  test_vm_mov_2();
  test_vm_add();
  test_vm_call();
  test_vm_call_2();
  test_vm_jmp();
  test_vm_call_aligned();
  test_vm_call_aligned_2();
  test_vm_jmp_aligned();
  test_vm_jmp_register();
  test_vm_shift();
  test_vm_jl();
  test_vm_jl_2();
  test_vm_jle();  
  test_vm_jge();
  test_vm_push_pop();
  test_vm_movq();
  test_vm_addsd();
  test_vm_imul();
  test_vm_imul2();
  test_vm_harmonic();
  test_vm_lea();
  }
