#include "../../include/kernel/string.h"

int count_arg(char *str, char delim)
{
    int i = 0;
    char *p = str;
    while(0 != *p){
        if(delim == *p)
            i++;
        p++;
    }
    return (i + 1);
}

int str_to_argv(char *str, char delim, char ***argv)
{
    int len = 0;
    int argc = 0;
    int i = 0;
    int j = 0;
    int index_num = 0;
    char *str_p;
    char *p;
    char *q;
    int k;

    len = strlen(str);
    index_num = len;

    str_p = (char *)kmalloc(sizeof(char) * len, GFP_ATOMIC);
    if (NULL == str_p)
		goto STR_P_MEM_ERRO;

    memset(str_p, 0, len);
    strcpy(str_p, str);

    argc = count_arg(str, delim);

    *argv = (char **)kmalloc(sizeof(char*) * argc, GFP_ATOMIC);
    if(NULL == *argv)
    	goto ARGV_MEM_ERRO;

    memset(*argv, 0, sizeof((*argv)));

    p = str_p;
    q = str_p;

    for (; j <= index_num; j++)
    {
        if (delim == *p)
        {
            *p = 0;
            len = strlen(q);
            (*argv)[i] = (char *)kmalloc(sizeof(char) * len, GFP_ATOMIC);
            if(NULL == (*argv)[i])
            	goto ARGV_I_MEM_ERRO;

            memset((*argv)[i], 0, len);
            strcpy((*argv)[i], q);
            i++;
            p++;
            q = p;
        }else if (0 == *p){
            len = strlen(q);
            (*argv)[i] = (char *)kmalloc(sizeof(char) * len, GFP_ATOMIC);
            if(NULL == (*argv)[i])
            	goto ARGV_I_MEM_ERRO;

            memset((*argv)[i], 0, len);
            strcpy((*argv)[i], q);            
        }else {
            p++;
        }
    }

    kfree(str_p);
    str_p = NULL;

    return argc;

STR_P_MEM_ERRO:
	return -1;

ARGV_MEM_ERRO:
	kfree(str_p);
    str_p = NULL;	

	return -2;

ARGV_I_MEM_ERRO:
	kfree(str_p);
	str_p = NULL;

	k = 0;
	for(; k <= i; k++){
		if(NULL != (*argv)[k]){
			kfree((*argv)[k]);
			(*argv)[k] = NULL;
		}
	}
	kfree(*argv);
	*argv = NULL;
	return -3;
}

int free_argv(char ***argv)
{
    int i = 0;

    while(NULL != (*argv)[i]){
        kfree((*argv)[i]);
        (*argv)[i] = NULL;
        i++;
    }

    if(NULL != (*argv)){
        kfree(*argv);
        *argv = NULL;
    }
    return 0;
}

int atoi(char *str)
{
    int len;
    int i;
    int retval;

    if(NULL == str)
        return -1;
    len = strlen(str);
    if(0 == len)
        return -2;
    i = 0;
    retval = 0;
    for(; i < len; i++){
        if(0 == i){
            retval = (str[0] - 48);
        }else{
        retval = (retval * 10) + (str[i] - 48);
        }
    }
    return retval;
}

// int intostr(int num, char *dst, int index)
// {
//    dst[i]

// }
