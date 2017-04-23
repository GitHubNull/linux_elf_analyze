#include <stdio.h>
////////////////////////////////////// This is a test of comment.  //////////////////////////////////////////
int main(int argc, char **argv, char **envp)
{
    int cnt = 0;
    while(*(envp)){
        printf("envp[%d] = %s\n", cnt, *(envp));
        cnt += 1;
        envp++;
    }
    return 0;
}
