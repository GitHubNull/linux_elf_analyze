#ifndef STR_UNPACK_H
#define STR_UNPACK_H

// #include "../../include/kernel/nlk.h"
#include "../../include/kernel/string.h"

int unpack_argv(char *src);
int unpack_envp(char *src, int seq);
int unpack_start(char *src);

typedef struct
{
	char **argv;
	int argc;
}APP_ARG;

typedef struct
{
	char **envp;
	int envpc;
}APP_ENVP;


// // static
// static APP_ARG app_arg;
// static APP_ENVP app_envp;
// static int envp_len;
// //static char app_name[128];
// static char *envp_buffer;


// APP_ARG app_arg;
// APP_ENVP app_envp;
// int envp_len;
// //static char app_name[128];
// char *envp_buffer;

#endif