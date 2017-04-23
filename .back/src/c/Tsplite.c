#include <stdio.h>
#include "splite.h"

int main()
{
    char *src = "This is a test";
    char **dst = (char**)malloc(sizeof(char**));
    char *delim = " ";
    int cnt = splite(src, dst, delim);
    if(cnt < 0)
        exit(-1);
    int i = 0;
    //printf("ok\n");
    for(; i < cnt; i++){
        printf("%s\n", dst[i]);
    }
    free(dst);
    exit(0);
}
