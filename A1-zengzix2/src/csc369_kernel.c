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
#include "csc369_kernel.h"

#include "csc369_cpu.h"
#include "csc369_executable.h"
#include "csc369_logger.h"
#include "csc369_process.h"
#include "csc369_system_call.h"

#include <assert.h>
#include <stdlib.h>

#include <unicorn/unicorn.h>

#define SYS_exit 1
#define SYS_pid 2
#define SYS_print_int 3

typedef struct csc369_queue_t
{
  size_t start;
  size_t end;
  size_t count;
  size_t capacity;
  CSC369_PCB** buffer;
} CSC369_Queue;

int
CSC369_QueueEnqueue(CSC369_Queue* queue, CSC369_PCB* item)
{
  assert(queue != NULL);
  assert(queue->count < queue->capacity); // Queue full.
  
  queue->buffer[queue->end] = item;
  queue->count++;
  queue->end++;
  if (queue->end == queue->capacity) {
    queue->end = 0;
  }
  return 0;
}

int
CSC369_QueueDequeue(CSC369_Queue* queue, CSC369_PCB** item)
{
  assert(queue != NULL);
  assert(item != NULL);
  assert(queue->count > 0); // Queue empty.
 
  *item = queue->buffer[queue->start];
  queue->count--;
  queue->start++;
  if (queue->start == queue->capacity) {
    queue->start = 0;
  }
  return 0;
}

size_t
CSC369_QueueSize(CSC369_Queue* queue)
{
  assert(queue != NULL);
  return queue->count;
}

typedef struct csc369_kernel_t
{
  /**
   * The CPU being time-shared.
   */
  CSC369_CPU* cpu;
  
  CSC369_Queue* pcbs;  // process table - each entry points to a PCB
  CSC369_PCB* curr_PCB;
  int last_pid;
} CSC369_Kernel;

void
trap_handler(CSC369_TrapType type, uint32_t info, void* user_data)
{
  if (type == CSC369_TRAP_TYPE_SYSTEM_CALL && info == 0) {

    CSC369_Kernel* kernel = (CSC369_Kernel*)user_data;
    uc_engine* uc = CSC369_CPUExposeUnicorn(kernel->cpu);
    uint64_t rax;
    uc_reg_read(uc, UC_X86_REG_RAX, &rax);

    switch (rax) {
      case SYS_exit:
        //CSC369_LOG_TRACE("Exit");
        CSC369_SystemCallExit(kernel);
        break;
      case SYS_pid:
        //CSC369_LOG_TRACE("Pid");
        CSC369_SystemCallPid(kernel);
        break;
      case SYS_print_int:
        //CSC369_LOG_TRACE("Print");
        CSC369_SystemCallPrintInteger(kernel);
        break;
      default:
        CSC369_LOG_TRACE("ERROR");
        break;
    }
  }
}

CSC369_Kernel*
CSC369_KernelCreate(CSC369_CPU* cpu, size_t max_procs)
{
  assert(cpu != NULL);
  UNUSED(max_procs);

  CSC369_Kernel* kernel = calloc(1, sizeof(CSC369_Kernel));
  if (kernel == NULL) {
    CSC369_LOG_ERRNO("calloc");
    return NULL;
  }
  
  kernel->cpu = cpu;
  kernel->last_pid = -1; // init impossible pid value
  
  // set trap handler
  CSC369_TrapHandlerFunc handle_trap = trap_handler;
  CSC369_CPURegisterTrapHandler(cpu, handle_trap, kernel);
  
  CSC369_Queue* pcbs = calloc(1, sizeof(CSC369_Queue)); // allocating queue struct
  if (pcbs == NULL) {
    CSC369_LOG_ERRNO("calloc");
    free(kernel);
    return NULL;
  }
  // init process table
  kernel->pcbs = pcbs;
  kernel->pcbs->capacity = max_procs;
  kernel->pcbs->start = 0;
  kernel->pcbs->end = 0;
  kernel->pcbs->count = 0;
  
  CSC369_PCB** buffer = calloc(max_procs, sizeof(CSC369_PCB*));  // allocating pointer to array of pointers to PCBs
  if (buffer == NULL) {
    CSC369_LOG_ERRNO("calloc");
    free(kernel);
    return NULL;
  }
  kernel->pcbs->buffer = buffer;
  /*for (size_t i = 0; i < max_procs; i++) {
    kernel->pcbs->buffer[i] = NULL;  // initializing process table
  }*/

  return kernel;
}

void
CSC369_KernelDestroy(CSC369_Kernel* kernel)
{
  free(kernel->pcbs);
  free(kernel);
}

void
ReadRegisters(uc_engine* uc, CSC369_PCB* pcb)
{
  uc_reg_read(uc, UC_X86_REG_RIP, &pcb->rip);
  uc_reg_read(uc, UC_X86_REG_RSP, &pcb->rsp);
  uc_reg_read(uc, UC_X86_REG_RBP, &pcb->rbp);
  uc_reg_read(uc, UC_X86_REG_RAX, &pcb->rax);
  uc_reg_read(uc, UC_X86_REG_RDI, &pcb->rdi);
}

void
WriteRegisters(uc_engine* uc, CSC369_PCB* pcb)
{
  uc_reg_write(uc, UC_X86_REG_RIP, &pcb->rip);
  uc_reg_write(uc, UC_X86_REG_RSP, &pcb->rsp);
  uc_reg_write(uc, UC_X86_REG_RBP, &pcb->rbp);
  uc_reg_write(uc, UC_X86_REG_RAX, &pcb->rax);
  uc_reg_write(uc, UC_X86_REG_RDI, &pcb->rdi);
}

int
CSC369_KernelRun(CSC369_Kernel* kernel)
{
  assert(kernel != NULL);
  uc_engine* uc = CSC369_CPUExposeUnicorn(kernel->cpu);
  while (kernel->pcbs->count > 0) {
    CSC369_PCB* pcb;
    CSC369_QueueDequeue(kernel->pcbs, &pcb);
    
    if (pcb != NULL && pcb->p_state == CSC369_PROCESS_STATE_READY) {  // first ready process
      
      if (kernel->last_pid != -1) {
        CSC369_LOG_TRACE("Switched %d with %d.", kernel->last_pid, pcb->pid);
      }
      // update running program
      kernel->curr_PCB = pcb;
      
      // update process state
      pcb->p_state = CSC369_PROCESS_STATE_RUNNING;
      
      // retrieve from context
      if (pcb->context != NULL) {
        //("RETRIEVE");
        
        // write code into cpu
        uc_err error;
        error = uc_mem_write(uc, CSC369_PROCESS_TEXT_START, pcb->p_text, pcb->p_size);
        assert(error == UC_ERR_OK);
        
        // write stack
        error = uc_mem_write(uc, pcb->rsp, pcb->p_stack, CSC369_PROCESS_STACK_START - pcb->rsp);
        assert(error == UC_ERR_OK);
        
        // Set the registers
        WriteRegisters(uc, pcb);
        
        // write context
        uc_context_restore(uc, pcb->context);
        
      } else {
        // write code into cpu
        uc_mem_write(uc, CSC369_PROCESS_TEXT_START, pcb->p_text, pcb->p_size);
  
        // Set the registers
        WriteRegisters(uc, pcb);
      }
      
      CSC369_CPUResume(kernel->cpu);
    }
  }
  
  CSC369_LOG_TRACE("Last process finished.");
  exit(0);
}

int32_t
CSC369_KernelCreateProcess(CSC369_Kernel* kernel, char const* program_path)
{
  assert(kernel != NULL);
  UNUSED(program_path);
  
  if (kernel->pcbs->count <= kernel->pcbs->capacity) {
    CSC369_PCB* pcb = calloc(1, sizeof(CSC369_PCB));  // allocating a PCB
    if (pcb == NULL) {
      CSC369_LOG_ERRNO("calloc");
      return -1;
    }
    
    pcb->pid = kernel->pcbs->count;  // index as pid
    pcb->p_state = CSC369_PROCESS_STATE_READY;  // initially ready
    pcb->p_size = CSC369_LoadProgram(program_path,  // loading program
                       &pcb->p_text,
                       &pcb->rip);
    pcb->rsp = CSC369_PROCESS_STACK_START;
    pcb->rbp = CSC369_PROCESS_TEXT_START;
    pcb->rax = CSC369_PROCESS_TEXT_START;
    pcb->rdi = CSC369_PROCESS_TEXT_START;
    
    CSC369_QueueEnqueue(kernel->pcbs, pcb); // enqueue process
    return pcb->pid;
  }
  
  return -1;
}

/*
 * System call functions.
 */
void
CSC369_SystemCallExit(CSC369_Kernel* kernel)
{
  assert(kernel != NULL);
  
  // Get the current process
  CSC369_PCB* pcb = kernel->curr_PCB;

  // LOG message
  uc_engine* uc = CSC369_CPUExposeUnicorn(kernel->cpu);
  uc_reg_read(uc, UC_X86_REG_RDI, &pcb->rdi);
  int rdi = pcb->rdi;
  CSC369_LOG_TRACE("[pid %d] CSC369_exit(%d)", pcb->pid, rdi);
  
  // Terminate the process (update/store attributes, free spaces)
  pcb->p_state = CSC369_PROCESS_STATE_EXIT;  // has no effect
  kernel->last_pid = pcb->pid;
  if (pcb->p_stack != NULL) {
    free(pcb->p_stack);
  }
  if (pcb->context != NULL) {
    uc_context_free(pcb->context);
  }
  free(pcb->p_text);
  free(pcb);
  
  // Kernel give up control to the next process
  CSC369_KernelRun(kernel);
}

void
CSC369_SystemCallPid(CSC369_Kernel* kernel)
{
  assert(kernel != NULL);
  
  // Get the current process
  CSC369_PCB* pcb = kernel->curr_PCB;
  
  // store pid into rax
  pcb->rax = pcb->pid;
  uc_engine* uc = CSC369_CPUExposeUnicorn(kernel->cpu);
  uc_reg_write(uc, UC_X86_REG_RAX, &pcb->rax);
  
  // LOG message
  int rax = pcb->rax;
  CSC369_LOG_TRACE("[pid %d] CSC369_pid(%d)", pcb->pid, rax);
  
  // update state
  pcb->p_state = CSC369_PROCESS_STATE_READY;
  kernel->last_pid = pcb->pid;
  
  // Kernel give up control to the next process
  ReadRegisters(uc, pcb);
  if (pcb->context != NULL) {
    uc_context_free(pcb->context);
  }
  uc_err error;
  error = uc_context_alloc(uc, &pcb->context);
  assert(error == UC_ERR_OK);
  error = uc_context_save(uc, pcb->context);
  assert(error == UC_ERR_OK);
  
  if (pcb->p_stack != NULL) {
    free(pcb->p_stack);
  }
  uint8_t* p_stack = calloc(CSC369_PROCESS_STACK_START - pcb->rsp, sizeof(uint8_t));
  pcb->p_stack = p_stack;
  error = uc_mem_read(uc, pcb->rsp, pcb->p_stack, CSC369_PROCESS_STACK_START - pcb->rsp);
  assert(error == UC_ERR_OK);
  
  CSC369_QueueEnqueue(kernel->pcbs, pcb);
  CSC369_KernelRun(kernel);
}

void
CSC369_SystemCallPrintInteger(CSC369_Kernel* kernel)
{
  assert(kernel != NULL);
  
  // Get the current process
  CSC369_PCB* pcb = kernel->curr_PCB;
  
  // retrieve rdi
  uc_engine* uc = CSC369_CPUExposeUnicorn(kernel->cpu);
  uc_reg_read(uc, UC_X86_REG_RDI, &pcb->rdi);
  
  // LOG message
  int rdi = pcb->rdi;
  CSC369_LOG_TRACE("[pid %d] CSC369_print_integer(%d)", pcb->pid, rdi);
  
  // update state
  pcb->p_state = CSC369_PROCESS_STATE_READY;
  kernel->last_pid = pcb->pid;
  
  // Kernel give up control to the next process
  ReadRegisters(uc, pcb);
  if (pcb->context != NULL) {
    uc_context_free(pcb->context);
  }
  uc_err error;
  error = uc_context_alloc(uc, &pcb->context);
  assert(error == UC_ERR_OK);
  error = uc_context_save(uc, pcb->context);
  assert(error == UC_ERR_OK);
  
  if (pcb->p_stack != NULL) {
    free(pcb->p_stack);
  }
  uint8_t* p_stack = calloc(CSC369_PROCESS_STACK_START - pcb->rsp, sizeof(uint8_t));
  pcb->p_stack = p_stack;
  error = uc_mem_read(uc, pcb->rsp, pcb->p_stack, CSC369_PROCESS_STACK_START - pcb->rsp);
  assert(error == UC_ERR_OK);
  
  CSC369_QueueEnqueue(kernel->pcbs, pcb);
  CSC369_KernelRun(kernel);
}
