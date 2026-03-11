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

/*
 * To better understand the embedded assembly, see:
 * - https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
 * - https://gcc.gnu.org/onlinedocs/gcc/extensions-to-the-c-language-family/how-to-use-inline-assembly-language-in-c-code.html#extended-asm
 * - https://stackoverflow.com/questions/9506353/how-to-invoke-a-system-call-via-syscall-or-sysenter-in-inline-assembly
 */
#define SYS_exit 1
#define SYS_pid 2
#define SYS_print_int 3

void
CSC369_exit(int status)
{
  register int rax __asm__ ("rax") = SYS_exit;
  register int rdi __asm__ ("rdi") = status;

  __asm__ __volatile__ (
      "syscall"
      : "+r" (rax)
      : "r" (rdi)
      : "rcx", "r11", "memory"
      );
}

int
CSC369_pid(void)
{
  register int rax __asm__ ("rax") = SYS_pid;

  __asm__ __volatile__ (
      "syscall"
      : "+r" (rax)
      :: "rcx", "r11", "memory"
      );

  return rax;
}

void
CSC369_print_int(int integer)
{
  register int rax __asm__ ("rax") = SYS_print_int;
  register int rdi __asm__ ("rdi") = integer;

  __asm__ __volatile__ (
      "syscall"
      : "+r" (rax)
      : "r" (rdi)
      : "rcx", "r11", "memory"
      );
}
