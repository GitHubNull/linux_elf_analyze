#include <stdio.h>

int main(int argc, char **argv, char **envp)
{
    int cnt = 0;
    printf("envp[0] = %s\n", *envp);
    while(*(envp + 1)){
        cnt += 1;
        printf("envp[%d] = %s\n", cnt, *(envp++));
    }
    return 0;
}
