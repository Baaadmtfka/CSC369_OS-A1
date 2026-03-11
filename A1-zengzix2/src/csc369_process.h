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

#ifndef CSC369H1_PROCESSES_PROCESS_H
#define CSC369H1_PROCESSES_PROCESS_H

#include <stdint.h>

// Forward declaration
typedef struct uc_context uc_context;

/**
 * The start address of a process' instructions.
 */
#define CSC369_PROCESS_TEXT_START 0x0000

/**
 * The start address of a process' stack, which grows downward.
 */
#define CSC369_PROCESS_STACK_START 0x2000

/**
 * The maximum size, in bytes, of a process' stack.
 */
#define CSC369_PROCESS_STACK_SIZE 0x1000

/**
 * The different states of a process in the system.
 */
typedef enum
{
  CSC369_PROCESS_STATE_BLOCKET,   // 0
  CSC369_PROCESS_STATE_READY,     // 1
  CSC369_PROCESS_STATE_RUNNING,   // 2
  CSC369_PROCESS_STATE_EXIT       // 3
} CSC369_ProcessState;

/**
 * The Process Control Block (PCB) is a collection of attributes that the OS
 * uses to (as the name implies) control processes.
 */
typedef struct csc369_pcb_t
{
  /**
   * The state of the registers on the CPU.
   */
  uc_context* context;

  int pid;
  CSC369_ProcessState p_state;
  uint8_t* p_text;
  uint8_t* p_stack;
  size_t p_size;
  
  uint64_t rsp, rbp, rip, rax, rdi;  // registers
} CSC369_PCB;

#endif // CSC369H1_PROCESSES_PROCESS_H
