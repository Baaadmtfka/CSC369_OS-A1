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

#ifndef CSC369H1_PROCESSES_CPU_H
#define CSC369H1_PROCESSES_CPU_H

#include <stddef.h>
#include <stdint.h>

// Forward declaration, so we don't have to include unicorn.h here
typedef struct uc_struct uc_engine;

/**
 * A wrapper around a Unicorn processor.
 *
 * This wrapper helps with tasks like: setting up the Unicorn processor,
 * registering call backs to deal with interrupts and faults, and provide an
 * abstraction for the starting/stopping of the user-mode emulation.
 *
 * However, you will still need to access the underlying Unicorn processor to
 * perform other tasks, like: saving and restoring registers, writing to memory,
 * etc.
 */
typedef struct csc369_cpu_t CSC369_CPU;

/**
 * The reason why the user program "trapped" into the kernel.
 *
 * @see CSC369_CPURegisterTrapHandler
 */
typedef enum
{
  CSC369_TRAP_TYPE_INTERRUPT,
  CSC369_TRAP_TYPE_SYSTEM_CALL,
  CSC369_TRAP_TYPE_FAULT
} CSC369_TrapType;

/**
 * The typedef of a function pointer that is called when handling traps.
 * - The first argument is the reason for the trap.
 * - The second argument depends on the first.
 * - The third argument is provided by whoever registered the trap handler.
 *
 * @see CSC369_CPURegisterTrapHandler
 */
typedef void (*CSC369_TrapHandlerFunc)(CSC369_TrapType,
                                       uint32_t,
                                       void* user_data);

/**
 * Create a new CPU.
 *
 * The CPU has 8192 bytes of "physical", byte-addressable memory.
 *
 * @return A (newly allocated) CPU, or NULL on error.
 */
CSC369_CPU*
CSC369_CPUCreate(void);

/**
 * Destroy (i.e., free memory) an allocated CPU.
 *
 * @param cpu A pointer to the CPU.
 *
 * @pre CPU is not NULL.
 */
void
CSC369_CPUDestroy(CSC369_CPU* cpu);

/**
 * The trap handler that the CPU should call when certain events happen.
 *
 * @cpu cpu A pointer to the CPU.
 * @param f A function pointer to the trap handler.
 * @param user_data A pointer to the user data to send to f.
 *
 * @pre cpu and f are not NULL.
 */
void
CSC369_CPURegisterTrapHandler(CSC369_CPU* cpu,
                              CSC369_TrapHandlerFunc f,
                              void* user_data);

/**
 * Get the trap handler currently registered with the CPU.
 *
 * @param cpu A pointer to the CPU.
 * @param[out] f A pointer to a function pointer to the trap handler.
 * @param[out] user_data A pointer to a pointer to the user data that is sent to
 * f.
 *
 * @pre CPU, f, and user_data are not NULL.
 */
void
CSC369_CPUGetTrapHandler(CSC369_CPU* cpu,
                         CSC369_TrapHandlerFunc* f,
                         void** user_data);

/**
 * Resume (or begin) the process currently allocated to this cpu.
 *
 * This should be used after performing a context switch on the processor, or
 * when trying to run the very first process.
 *
 * @param cpu A pointer to the CPU.
 *
 * @pre CPU is not NULL.
 */
void
CSC369_CPUResume(CSC369_CPU* cpu);

/**
 * Return the underlying Unicorn processor.
 *
 * Useful to access the Unicorn Engine library's other functions.
 *
 * WARNING: You should not use functions that are "wrapped" by this library to
 * avoid issues with the automated tester. Most likely, the functions from the
 * Unicorn Engine that you will want to use involve: reading/writing registers
 * and/or memory and allocating/saving/restoring the CPU context. Do **not** use
 * uc_emu_start under any circumstances, except by calling CSC369_CPUResume.
 *
 * @param cpu A pointer to the CPU.
 *
 * @return A pointer to this CPU's unicorn processor.
 *
 * @pre CPU is not NULL.
 */
uc_engine*
CSC369_CPUExposeUnicorn(CSC369_CPU* cpu);

#endif // CSC369H1_PROCESSES_CPU_H
