#include "splite.h"
#include <stdio.h>

int splite(char *src, char **dst, char *delim)
{
    char **tmp = dst;
    int cnt = 0;
    if((NULL == src) || 0 == (strlen(src))
       ||(' ' == src[0])||('\n' == src[0]))
        return cnt;
    char *p;
    while((p = strstr(src, delim))){
        snprintf(*dst++, p - src, "%s", src);
        src = p + strlen(delim);
    }
    return cnt;
}
