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

#include "syscall.h"

/**
 * The expected signature of a user program's "main" function.
 *
 * @return The exit status of the program.
 */
int
CSC369_main(void);

void
_CSC369_start(void)
{
  CSC369_exit(CSC369_main());
}
