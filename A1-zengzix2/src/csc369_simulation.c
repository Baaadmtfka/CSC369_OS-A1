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

#include "csc369_simulation.h"

#include "csc369_cpu.h"
#include "csc369_kernel.h"
#include "csc369_logger.h"

#include <assert.h>

#include <unicorn/unicorn.h>

typedef struct csc369_simulation_t
{
  CSC369_CPU* cpu;
  CSC369_Kernel* kernel;
} CSC369_Simulation;

CSC369_Simulation*
CSC369_SimulationCreate(size_t program_count, char** program_paths)
{
  CSC369_Simulation* simulation = calloc(1, sizeof(CSC369_Simulation));
  if (simulation == NULL) {
    CSC369_LOG_ERRNO("calloc");
    return NULL;
  }

  simulation->cpu = CSC369_CPUCreate();
  if (simulation->cpu == NULL) {
    CSC369_LOG_ERROR("Could not create cpu.");
    CSC369_CPUDestroy(simulation->cpu);
    return NULL;
  }

  simulation->kernel = CSC369_KernelCreate(simulation->cpu, program_count);
  if (simulation->kernel == NULL) {
    CSC369_LOG_ERROR("Could not create kernel.");
    CSC369_SimulationDestroy(simulation);
    return NULL;
  }

  for (size_t i = 0; i < program_count; i++) {
    int32_t const pid =
      CSC369_KernelCreateProcess(simulation->kernel, program_paths[i]);
    if (pid == -1) {
      CSC369_LOG_ERROR("Could not create process for program %s.",
                       program_paths[i]);
    } else {
      CSC369_LOG_TRACE("Created new process with pid %d for program %s.",
                       pid,
                       program_paths[i]);
    }
  }

  return simulation;
}

void
CSC369_SimulationDestroy(CSC369_Simulation* simulation)
{
  if (simulation->cpu != NULL) {
    CSC369_CPUDestroy(simulation->cpu);
  }
  if (simulation->kernel != NULL) {
    CSC369_KernelDestroy(simulation->kernel);
  }

  free(simulation);
}

int
CSC369_SimulationStart(CSC369_Simulation* simulation)
{
  assert(simulation != NULL);

  CSC369_KernelRun(simulation->kernel);
  return 0;
}
