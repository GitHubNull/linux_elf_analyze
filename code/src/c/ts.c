#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char *src = "This is a splite test string.";
    char *delim = " ";
    char *p;
    p = strstr(src, delim);
    int len = p - src + 1;
    char **dst = (char**)malloc(sizeof(char**));
    *dst = (char*)malloc(sizeof(char*));
    snprintf(*dst++, len, "%s", src);
    src = p + strlen(delim);
    //*(q  + (p - q)) = 0;
    printf("src = %s\tq = %s\n", src, *(dst - 1));
    exit(0);
}
