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

#ifndef CSC369H1_PROCESSES_SYSTEM_CALL_H
#define CSC369H1_PROCESSES_SYSTEM_CALL_H

// Forward declaration
typedef struct csc369_kernel_t CSC369_Kernel;

// Function pointer definition
typedef void (*CSC369_SystemCallFunc)(CSC369_Kernel*);

/**
 * The running program wants to exit.
 */
void
CSC369_SystemCallExit(CSC369_Kernel* kernel);

/**
 * The running program is requesting its PID.
 */
void
CSC369_SystemCallPid(CSC369_Kernel* kernel);

/**
 * The running program is yielding the processor to another process.
 */
void
CSC369_SystemCallYield(CSC369_Kernel* kernel);

/**
 * The running program wants to print an integer.
 */
void
CSC369_SystemCallPrintInteger(CSC369_Kernel* kernel);

#endif // CSC369H1_PROCESSES_SYSTEM_CALL_H
