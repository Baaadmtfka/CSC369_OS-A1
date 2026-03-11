add_library(csc369_processes
    csc369_cpu.h
    csc369_cpu.c
    csc369_executable.c
    csc369_executable.h
    csc369_kernel.c
    csc369_kernel.h
    csc369_logger.h
    csc369_process.h
)

add_library(CSC369::processes ALIAS csc369_processes)

target_include_directories(csc369_processes
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(csc369_processes
    PUBLIC
      Unicorn::Unicorn
)

target_compile_options(csc369_processes
    PRIVATE
      -Wall -Wextra -Werror
)

add_executable(procsim
    csc369_simulation.h
    csc369_simulation.c
    sim_main.c
)

target_link_libraries(procsim
    PRIVATE
      CSC369::processes
)


target_compile_options(procsim
    PRIVATE
      -Wall -Wextra -Werror
)

set_target_properties(procsim
    PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CSC369H1_PROCESSES_SANDBOX_DIR}"
)
