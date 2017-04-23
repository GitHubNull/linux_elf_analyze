#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#define MAX_PAR 128
#define CMD_TIP "es>"
#define EXIT_FLAG "quit"
#define CMD_BUF_SIZE 256

void ctrlcHandle(int signalnum)
{
    printf("quit\n");
    exit(0);
}

int main(int argc, char *argv[], char *envp[])
{
    char *new_argv[MAX_PAR];
    char cmdBuff[CMD_BUF_SIZE];
    pid_t pid = -1;
    int paranum = 0;
    int status;
    signal(SIGINT, ctrlcHandle);
    while(1){
        printf(CMD_TIP);
        fflush(stdout);
        fgets(cmdBuff, CMD_BUF_SIZE, stdin);
        printf("%s\n", cmdBuff);
        /*if('\n' == cmdBuff[0])
            continue;
        else if( '\n' == cmdBuff[strlen(cmdBuff) - 1] )
            cmdBuff[strlen(cmdBuff) - 1] = '\0';
        else if(0 == strcmp(cmdBuff, EXIT_FLAG))
            break;
        new_argv[0] = strtok(cmdBuff, " ");
        while(paranum < MAX_PAR && (new_argv[paranum] = strtok(NULL, " "))){
            paranum++;
        }
        if( 0 > (pid = vfork())){
            perror("vfork");
            exit(EXIT_FAILURE);
        }
        if(0 == pid){
            execvp(new_argv[0], new_argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
            waitpid(pid, &status, 0);*/
    }
    exit(0);
}
