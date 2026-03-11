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

#ifndef CSC369H1_PROCESSES_EXECUTABLE_H
#define CSC369H1_PROCESSES_EXECUTABLE_H

#include <stddef.h>
#include <stdint.h>

/**
 * Load the program binary at pathname into text and return its entry and size.
 *
 * @param pathname A path to the program binary.
 * @param text The program's code.
 * @param[out] program_entry The address to start running this program.
 *
 * @return The total number of bytes of the program code.
 *
 * @pre pathname, text, and program_entry are not NULL.
 */
size_t
CSC369_LoadProgram(char const* pathname,
                   uint8_t** text,
                   uint64_t* program_entry);

#endif // CSC369H1_PROCESSES_EXECUTABLE_H
