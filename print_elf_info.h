#ifndef PRINT_ELF_INFO_H
#define PRINT_ELF_INFO_H

#include <stdio.h>
#include <elf.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool pri_elf_ehdr(char *filename);
bool pri_elf_ptb(char *filename);
bool pri_elf_stb(char *filename);

#endif PRINT_ELF_INFO_H
