# Original Source: https://gist.github.com/vogr/e5e70319dfad9e4cf5ad847d101bfac3
#
# Find module for the Unicorn Engine
# https://www.unicorn-engine.org/
#
# Usage:
#     LIST(APPEND CMAKE_MODULE_PATH "/path/to/this_file-s_parent_dir")
#     find_package(Unicorn REQUIRED)
#     ...
#     add_executable(pgm ...)
#     target_link_libraries(pgm Unicorn::Unicorn)


find_package(PkgConfig)

pkg_check_modules(PC_Unicorn IMPORTED_TARGET QUIET unicorn)

if(PC_Unicorn_FOUND)
  if(NOT TARGET Unicorn::Unicorn)
    set_target_properties(PkgConfig::PC_Unicorn PROPERTIES IMPORTED_GLOBAL TRUE)
    add_library(Unicorn::Unicorn ALIAS PkgConfig::PC_Unicorn)
  endif()
else()
  # Try to find header and library without pkg-config

  find_path(
      Unicorn_INCLUDE_DIRS
      NAMES
        unicorn/unicorn.h
      PATHS
        /u/csc369h/winter/pub/dep/include
  )

  find_library(
      Unicorn_LIBRARY
      NAMES
        libunicorn.so
      PATHS
        /u/csc369h/winter/pub/dep/lib
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Unicorn
      REQUIRED_VARS Unicorn_INCLUDE_DIRS Unicorn_LIBRARY
  )

  if (Unicorn_FOUND AND NOT TARGET Unicorn::Unicorn)
    add_library(Unicorn::Unicorn INTERFACE IMPORTED)
    target_include_directories(Unicorn::Unicorn INTERFACE ${Unicorn_INCLUDE_DIRS})
    target_link_libraries(Unicorn::Unicorn INTERFACE ${Unicorn_LIBRARY})
  endif()
endif()
