set(CSC369_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/csc369_user_program.lds)

function(add_workload WORKLOAD_NAME WORKLOAD_SOURCE)
  add_executable(${WORKLOAD_NAME}
      ${WORKLOAD_SOURCE}
      start.c
      syscall.h
      syscall.c
  )

  target_compile_options(${WORKLOAD_NAME}
      PRIVATE
        -fno-stack-protector
        -fno-asynchronous-unwind-tables
  )

  target_link_options(${WORKLOAD_NAME}
      PRIVATE
        -nostdlib
        -static
        -T ${CSC369_LINKER_SCRIPT}
  )

  set_target_properties(${WORKLOAD_NAME}
      PROPERTIES
        LINK_DEPENDS ${CSC369_LINKER_SCRIPT}
        RUNTIME_OUTPUT_DIRECTORY "${CSC369H1_PROCESSES_SANDBOX_DIR}"
  )
endfunction()

add_workload(return_3 return_3.c)
add_workload(return_pid return_pid.c)
add_workload(hello_42 hello_42.c)
add_workload(hello_pid hello_pid.c)
