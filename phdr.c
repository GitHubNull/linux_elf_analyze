#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char **argv, char **envp)
{
    FILE* fp = fopen(argv[1], "rb");
    if(NULL == fp){
        printf("erro to open: %s .\n", argv[1]);
        exit(-1);
    }
    Elf64_Phdr phdr;
    if(1 != fread(&phdr, sizeof(phdr), 1, fp)){
        printf("erro can't read phdr.\n");
        exit(-1);
    }
    if(0 == phdr.p_type){
        printf("OK\n");
    }
    printf("p_type: %x\n", phdr.p_type);
    exit(0);
}
