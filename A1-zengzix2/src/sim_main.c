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

#include <unistd.h>

#include "csc369_simulation.h"

int
main(int argc, char* argv[])
{
  size_t const program_count = argc - optind;
  char** program_paths = &argv[optind];

  CSC369_Simulation* simulation =
    CSC369_SimulationCreate(program_count, program_paths);
  CSC369_SimulationStart(simulation);
  CSC369_SimulationDestroy(simulation);

  return 0;
}
