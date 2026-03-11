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

#ifndef CSC369H1_PROCESSES_SYSCALL_H
#define CSC369H1_PROCESSES_SYSCALL_H

/**
 * Exit the program.
 *
 * @param status The exit code of the program.
 */
void
CSC369_exit(int status);

/**
 * Get the unique process identifier (PID) of this process.
 *
 * @return The PID.
 */
int
CSC369_pid(void);

/**
 * Print the integer.
 *
 * @param integer The integer value to print.
 */
void
CSC369_print_int(int integer);

#endif // CSC369H1_PROCESSES_SYSCALL_H
