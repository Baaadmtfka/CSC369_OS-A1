/*
 * This code is provided solely for the personal and private use of students
 * taking the CSC369H course at the University of Toronto. Copying for purposes
 * other than this use is expressly prohibited. All forms of distribution of
 * this code, including but not limited to public repositories on GitHub,
 * GitLab, Bitbucket, or any other online platform, whether as given or with
 * any changes, are expressly prohibited.
 *
 * Authors: Mario Badr
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2024 Mario Badr
 */
#include "csc369_cpu.h"

#include "csc369_logger.h"
#include "csc369_process.h"

#include <assert.h>
#include <unicorn/unicorn.h>

#ifndef CSC369_DEBUG_REGISTERS
// Change this to 1 if you want to see register values logged
#define CSC369_DEBUG_REGISTERS 0
#endif

#if CSC369_DEBUG_REGISTERS == 1
// You may find this useful for debugging purposes.
static void
PrintRegisters(uc_engine* uc, uint32_t size)
{
  int64_t rsp, rbp, rip, rax, rdi;
  uc_reg_read(uc, UC_X86_REG_RSP, &rsp);
  uc_reg_read(uc, UC_X86_REG_RBP, &rbp);
  uc_reg_read(uc, UC_X86_REG_RIP, &rip);
  uc_reg_read(uc, UC_X86_REG_RAX, &rax);
  uc_reg_read(uc, UC_X86_REG_RDI, &rdi);

  CSC369_LOG_TRACE("%u | RIP 0x%" PRIx64 ", RSP 0x%" PRIx64 ", RBP 0x%" PRIx64
                   ", RAX 0x%" PRIx64 ", RDI 0x%" PRIx64,
                   size,
                   rip,
                   rsp,
                   rbp,
                   rax,
                   rdi);
}
#endif

typedef struct csc369_cpustat_t
{
  /**
   * The number of instructions executed.
   */
  int32_t instruction_count;

  /**
   * The number of interrupts encountered.
   */
  int32_t interrupt_count;
} CSC369_CPUStats;

typedef struct csc369_cpu_t
{
  uc_engine* uc;
  CSC369_CPUStats stats;

  CSC369_TrapHandlerFunc handle_trap;
  void* trap_user_data;
} CSC369_CPU;

static void
CallTrapHandler(CSC369_CPU* cpu, CSC369_TrapType type, uint32_t info)
{
  if (cpu->handle_trap == NULL) {
    CSC369_LOG_ERROR("Trap handler was NULL, cannot handle trap.");
    abort();
  }

  cpu->handle_trap(type, info, cpu->trap_user_data);
}

static void
OnInterrupt(uc_engine* uc, uint32_t interrupt_number, void* user_data)
{
  UNUSED(uc);

  CSC369_CPU* cpu = (CSC369_CPU*)user_data;
  cpu->stats.interrupt_count++;

  CallTrapHandler(cpu, CSC369_TRAP_TYPE_INTERRUPT, interrupt_number);
}

static bool
OnInvalidMem(uc_engine* uc,
             uc_mem_type type,
             uint64_t address,
             int size,
             int64_t value,
             void* user_data)
{
  UNUSED(uc);
  UNUSED(type);
  UNUSED(address);
  UNUSED(size);
  UNUSED(value);

  CSC369_CPU* cpu = (CSC369_CPU*)user_data;
  CallTrapHandler(cpu, CSC369_TRAP_TYPE_FAULT, type);

  return false;
}

static void
OnSysCall(uc_engine* uc, void* user_data)
{
  // The trap handler may not return to this function.
  //  1. If it does not, then we want to increment rip to the next instruction.
  //  2. But if it does, then we don't want to have incremented rip.

  // A syscall instruction is two bytes. We will want to resume after this
  // instruction, so update RIP before calling the trap handler.
  uint64_t rip_before, rip;
  uc_reg_read(uc, UC_X86_REG_RIP, &rip_before);
  rip = rip_before + 2;
  uc_reg_write(uc, UC_X86_REG_RIP, &rip);

  CSC369_CPU* cpu = (CSC369_CPU*)user_data;
  CallTrapHandler(cpu, CSC369_TRAP_TYPE_SYSTEM_CALL, 0);

  // We returned from the trap handler, so Resume was never called
  uc_reg_write(uc, UC_X86_REG_RIP, &rip_before);
}

static void
OnInstruction(uc_engine* uc, uint64_t address, uint32_t size, void* user_data)
{
  UNUSED(uc);
  UNUSED(address);
  UNUSED(size);

#if CSC369_DEBUG_REGISTERS == 1
  PrintRegisters(uc, size);
#endif

  CSC369_CPU* cpu = (CSC369_CPU*)user_data;
  cpu->stats.instruction_count++;
}

static int
AddUnicornHooks(uc_engine* uc, CSC369_CPU* cpu)
{
  uc_err error;

  // Need to add this hook so that unicorn ensures the PC is always
  // "synchronized"
  uc_hook code_hook;
  error = uc_hook_add(uc, &code_hook, UC_HOOK_CODE, OnInstruction, cpu, 1, 0);
  if (error) {
    CSC369_LOG_ERROR("uc_hook failed, %u (%s)", error, uc_strerror(error));
    return -1;
  }

  uc_hook interrupt_hook;
  error =
    uc_hook_add(uc, &interrupt_hook, UC_HOOK_INTR, OnInterrupt, cpu, 1, 0);
  if (error) {
    CSC369_LOG_ERROR("uc_hook failed, %u (%s)", error, uc_strerror(error));
    return -1;
  }

  uc_hook bad_mem_hook;
  error = uc_hook_add(
    uc, &bad_mem_hook, UC_HOOK_MEM_INVALID, OnInvalidMem, cpu, 1, 0);
  if (error) {
    CSC369_LOG_ERROR("uc_hook failed, %u (%s)", error, uc_strerror(error));
    return -1;
  }

  uc_hook syscall_hook;
  error = uc_hook_add(
    uc, &syscall_hook, UC_HOOK_INSN, OnSysCall, cpu, 1, 0, UC_X86_INS_SYSCALL);
  if (error) {
    CSC369_LOG_ERROR("uc_hook failed, %u (%s)", error, uc_strerror(error));
    return -1;
  }

  return 0;
}

CSC369_CPU*
CSC369_CPUCreate(void)
{
  CSC369_CPU* cpu = calloc(1, sizeof(CSC369_CPU));
  if (cpu == NULL) {
    CSC369_LOG_ERRNO("calloc");
    return NULL;
  }

  // Create an x86-64 "software" processor
  uc_err error = uc_open(UC_ARCH_X86, UC_MODE_64, &cpu->uc);
  if (error) {
    CSC369_LOG_ERROR("uc_open failed, %u (%s)", error, uc_strerror(error));
    CSC369_CPUDestroy(cpu);
    return NULL;
  }

  // Register callbacks for different processor events
  AddUnicornHooks(cpu->uc, cpu);

  // Create the "memory map".
  error = uc_mem_map(cpu->uc, 0, 0x2000, UC_PROT_ALL);
  if (error) {
    CSC369_LOG_ERROR("uc_mem_map failed, %u (%s)", error, uc_strerror(error));
    return NULL;
  }

  return cpu;
}

void
CSC369_CPUDestroy(CSC369_CPU* cpu)
{
  assert(cpu != NULL);

  if (cpu->uc != NULL) {
    uc_close(cpu->uc);
  }

  free(cpu);
}

void
CSC369_CPURegisterTrapHandler(CSC369_CPU* cpu,
                              CSC369_TrapHandlerFunc f,
                              void* user_data)
{
  cpu->handle_trap = f;
  cpu->trap_user_data = user_data;
}

void
CSC369_CPUGetTrapHandler(CSC369_CPU* cpu,
                         CSC369_TrapHandlerFunc* f,
                         void** user_data)
{
  assert(cpu != NULL);
  assert(f != NULL);
  assert(user_data != NULL);

  *f = cpu->handle_trap;
  *user_data = cpu->trap_user_data;
}

void
CSC369_CPUResume(CSC369_CPU* cpu)
{
  assert(cpu != NULL);
  int64_t rip;
  uc_reg_read(cpu->uc, UC_X86_REG_RIP, &rip);

  uc_ctl_flush_tlb(cpu->uc); // MUST DO THIS
  uc_err error = uc_emu_start(cpu->uc, rip, CSC369_PROCESS_STACK_START, 0, 0);
  if (error) {
    CSC369_LOG_ERROR("uc_emu_start failed, %u (%s)", error, uc_strerror(error));
    abort();
  }
}

uc_engine*
CSC369_CPUExposeUnicorn(CSC369_CPU* cpu)
{
  assert(cpu != NULL);
  return cpu->uc;
}
