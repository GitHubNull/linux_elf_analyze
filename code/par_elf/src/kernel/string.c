#include "../../include/kernel/string.h"

int count_arg(char *str, char delim)
{
    int i = 0;
    char *p = str;
    printk("count_arg start...\n");
    while(0 != *p){
        if(delim == *p)
            i++;
        p++;
    }
    return i;
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
    printk("str_to_argv start...\n");

    len = strlen(str);
    if(0 > len)
    {
        printk("str len < 0 error.\n");
        return -1;
    }

    index_num = len;

    str_p = (char *)kmalloc(sizeof(char) * (len + 1), GFP_ATOMIC);
    if (NULL == str_p)
    {
        printk("str_p = kmalloc error.\n");
        return -2;
    }

    memset(str_p, 0, (len + 1));
    strcpy(str_p, str);

    argc = count_arg(str, delim);
    printk("argc = %d\n", argc);
    if(0 > argc)
    {
        printk("argc = count_arg error.\n");
        kfree(str_p);
        str_p = NULL;
        return -3;
    }

    *argv = (char **)kmalloc(sizeof(char*) * argc, GFP_ATOMIC);
    if(NULL == *argv)
    {
        printk("argv = argv = ...kmalloc error.\n");
        kfree(str_p);
        str_p = NULL;
        return -4;
    }

    memset(*argv, 0, sizeof(argv));

    p = str_p;
    q = str_p;

    for (; j <= index_num; j++)
    {
        if (delim == *p)
        {
            *p = 0;
            len = strlen(q);
            (*argv)[i] = (char *)kmalloc(sizeof(char) * (len + 1), GFP_ATOMIC);
            if(NULL == (*argv)[i])
            {
                printk("argv[%d] kmalloc error.\n", i);
                goto ARGV_I_MEM_ERRO;
            }

            memset((*argv)[i], 0, (len + 1));
            strcpy((*argv)[i], q);
            i++;
            p++;
            q = p;
        }else if (0 == *p){
            len = strlen(q);
            (*argv)[i] = (char *)kmalloc(sizeof(char) * (len + 1), GFP_ATOMIC);
            if(NULL == (*argv)[i])
            {
                printk("argv[%d] kmalloc error.\n", i);
                goto ARGV_I_MEM_ERRO;
            }

            memset((*argv)[i], 0, (len + 1));
            strcpy((*argv)[i], q);            
        }else {
            p++;
        }
    }

    kfree(str_p);
    str_p = NULL;

    return argc;

ARGV_I_MEM_ERRO:
    kfree(str_p);
    str_p = NULL;

    k = 0;
    for(; k <= i; k++)
    {
        if(NULL != (*argv)[k])
        {
            kfree((*argv)[k]);
            (*argv)[k] = NULL;
        }
    }
    kfree(*argv);
    *argv = NULL;
    return -5;
}

int free_argv(char ***argv, int cnt)
{
    int i = 0;
    if(NULL == *argv)
    {   
        printk("NULL == argv error.\n");
        return -1;
    }
    printk("free_argv start...");
    for(; i < cnt; i++)
    {
        kfree((*argv)[i]);
        (*argv)[i] = NULL;
        i++;        
    }

    if(NULL != (*argv))
    {
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
    printk("atoi start...");

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
