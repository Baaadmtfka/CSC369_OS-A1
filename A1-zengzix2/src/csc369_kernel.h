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

#ifndef CSC369H1_PROCESSES_KERNEL_H
#define CSC369H1_PROCESSES_KERNEL_H

#include <stddef.h>
#include <stdint.h>

// Forward declarations
typedef struct csc369_cpu_t CSC369_CPU;

typedef struct csc369_kernel_t CSC369_Kernel;

/**
 * Create an operating system kernel.
 *
 * @param max_procs The maximum number of processes this kernel supports.
 *
 * @return The newly allocated kernel, or NULL on error.
 *
 * @pre max_procs >= 1
 */
CSC369_Kernel*
CSC369_KernelCreate(CSC369_CPU* cpu, size_t max_procs);

/**
 * Destroy the kernel and free up any resources it was using.
 *
 * @param kernel A pointer to the kernel to destroy.
 *
 * @pre kernel is not NULL.
 */
void
CSC369_KernelDestroy(CSC369_Kernel* kernel);

/**
 * Start running the operating system kernel.
 *
 * This should only ever be called once, after all processes have been created
 * for this kernel. The kernel should run the first process that was created.
 *
 * @param kernel A pointer to the kernel.
 *
 * @return -1 on error, otherwise this may never return.
 */
int
CSC369_KernelRun(CSC369_Kernel* kernel);

/**
 * Create a new process in the kernel.
 *
 * All calls to this function must happen before CSC369_KernelRun for a given
 * kernel.
 *
 * @param kernel A pointer to the kernel.
 * @param program_path A path to the program executable.
 *
 * @return If successful, the PID of the newly created process, otherwise -1.
 *
 * @pre kernel is not NULL.
 */
int32_t
CSC369_KernelCreateProcess(CSC369_Kernel* kernel, char const* program_path);

#endif // CSC369H1_PROCESSES_KERNEL_H
