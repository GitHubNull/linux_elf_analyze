#ifndef STRING_H
#define STRING_H

#include <linux/string.h>
#include <linux/slab.h>

int count_arg(char *str, char delim);
int str_to_argv(char *str, char delim, char ***argv);
int free_argv(char ***argv, int cnt);
int atoi(char *str);

// EXPORT_SYMBOL(str_to_argv);
// EXPORT_SYMBOL(free_argv);
// EXPORT_SYMBOL(atoi);

#endif