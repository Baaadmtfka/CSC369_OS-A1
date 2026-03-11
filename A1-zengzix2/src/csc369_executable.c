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
#include "csc369_executable.h"

#include "csc369_logger.h"

#include <elf.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

/**
 * Return the contents of a binary file at path_to_file as an array of bytes, as
 * well as the number of bytes read.
 *
 * Ownership of the array of bytes is passed to the caller. That is, the caller
 * is responsible for freeing the returned array.
 *
 * @param pathname A path to the binary file.
 * @param count The number of bytes read.
 *
 * @return An allocated array of bytes, or NULL on error.
 *
 * @pre count is not NULL.
 */
uint8_t*
CSC369_ReadBinaryFile(char const* pathname, size_t* count)
{
  FILE* binary_file = fopen(pathname, "rb");
  if (binary_file == NULL) {
    CSC369_LOG_ERRNO("fopen");
    abort();
  }

  struct stat file_attributes;
  if (stat(pathname, &file_attributes) == -1) {
    CSC369_LOG_ERRNO("stat");
    abort();
  }

  uint8_t* contents = calloc(file_attributes.st_size, sizeof(uint8_t));
  *count =
    fread(contents, sizeof(contents[0]), file_attributes.st_size, binary_file);
  if (*count != (size_t)file_attributes.st_size) {
    CSC369_LOG_ERRNO("fread");
    abort();
  }

  fclose(binary_file);

  CSC369_LOG_TRACE(
    "Loaded %s into kernel memory (%zu bytes read)", pathname, *count);
  return contents;
}

size_t
CSC369_LoadProgram(char const* pathname,
                   uint8_t** text,
                   uint64_t* program_entry)
{
  size_t bytes_read;
  uint8_t* base = CSC369_ReadBinaryFile(pathname, &bytes_read);

  // Read the ELF header
  Elf64_Ehdr const* elf_header = (Elf64_Ehdr const*)(base);
  // Save the address of the first instruction
  *program_entry = elf_header->e_entry;

  // Refer to the array of section headers
  Elf64_Shdr const* sections = (Elf64_Shdr const*)(base + elf_header->e_shoff);
  // Read the section header string table (shstrtab)
  char const* shstrtab =
    (char const*)(base + sections[elf_header->e_shstrndx].sh_offset);

  // Using the ELF header, read in each section header
  size_t code_size = 0;
  for (Elf64_Half i = 0; i < elf_header->e_shnum; i++) {
    char const* name = shstrtab + sections[i].sh_name;

    if (!strcmp(name, ".text")) {
      code_size = sections[i].sh_size;
      CSC369_LOG_TRACE("Saving %s section (size: %lu bytes).", name, code_size);
      *text = calloc(sizeof(uint8_t), code_size);
      memcpy(*text, base + sections[i].sh_offset, code_size);
    }
  }

  free(base);
  return code_size;
}
