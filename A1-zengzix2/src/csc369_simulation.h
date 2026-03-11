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

#ifndef CSC369H1_PROCESSES_SIMULATION_H
#define CSC369H1_PROCESSES_SIMULATION_H

#include <stddef.h>

typedef struct csc369_simulation_t CSC369_Simulation;

/**
 * Create a new simulation.
 *
 * @return A (newly allocated) simulation, or NULL on error.
 *
 * @related CSC369_SimulationDestroy
 */
CSC369_Simulation*
CSC369_SimulationCreate(size_t program_count, char** program_paths);

/**
 * Destroy (i.e., free memory) an allocated simulation.
 *
 * @param simulation The address of the simulation object.
 *
 * @pre simulation is not NULL.
 */
void
CSC369_SimulationDestroy(CSC369_Simulation* simulation);

/**
 * Start a simulation where processes "cooperate" with the operating system.
 *
 * @param simulation The simulation to run.
 * @param pathname The path to the first executable program.
 *
 * @return 0 on success, -1 otherwise.
 *
 * @pre simulation and pathname or not NULL.
 */
int
CSC369_SimulationStart(CSC369_Simulation* simulation);

#endif // CSC369H1_PROCESSES_SIMULATION_H
