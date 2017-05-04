#include "../../include/kernel/str_unpack.h"

extern int str_to_argv(char *str, char delim, char ***argv);
extern int free_argv(char ***argv);
extern int atoi(char *str);

// extern APP_ARG app_arg;
// extern char app_name[128];
// extern APP_ENVP app_envp;
// extern int envp_len;
// extern char *envp_buffer;

static APP_ARG app_arg;
static APP_ENVP app_envp;
static int envp_len;
//static char app_name[128];
static char *envp_buffer;

int unpack_argv(char *src)
{
	int retval;
	int tmp;

	retval = str_to_argv(src, ' ', &(app_arg.argv));
	if(0 >= retval)
		goto out;
	app_arg.argc = retval;
	tmp = retval - 1;
	retval = atoi(app_arg.argv[tmp]);
	if(0 >= retval)
		goto out1;

	envp_len = retval;
	kfree(app_arg.argv[tmp]);
	memset(app_arg.argv[tmp], 0, sizeof(app_arg.argv[tmp]));
	app_arg.argc = tmp;

	return 0;

out:
	free_argv( &(app_arg.argv));
	app_arg.argc = 0;
	return retval;

out1:
	free_argv(&(app_arg.argv));
	app_arg.argc = 0;
	return retval;
}

int unpack_envp(char *src, int seq)
{
	int retval;

	if(NULL == envp_buffer){
		envp_buffer = (char *)kzalloc(sizeof(char) * envp_len, GFP_ATOMIC);
		if(NULL == envp_buffer)
			goto out1;
	}
	
	strcat(envp_buffer, src);

	if(0 == seq){
		retval = str_to_argv(envp_buffer, ' ', &(app_envp.envp));
		if(0 >= retval)
			goto out;

		kfree(envp_buffer);
		envp_buffer = NULL;

		app_envp.envpc = retval;
		return 0;
	}

	return 1;

out:
	free_argv(&(app_envp.envp));
	app_envp.envpc = 0;

out1:
	kfree(envp_buffer);
	envp_buffer = NULL;
	return -1;
}

int unpack_start(char *src)
{
	if(NULL == src)
		return -1;
	if(0 == (strcmp("010", src)))
		return 0;
	else
		return -1;
}