#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

int main(int argc, char **argv, char **envp)
{
    FILE *fp = fopen(argv[1], "rb");
    if(NULL == fp){
        printf("erro open file: %s\n", argv[1]);
        exit(-1);
    }
    long fsize = 0;
    char *buffer;
    size_t result;

    /* get file size and then rewind to the file begin */
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    rewind(fp);

    /* malloc memery to buffer. */
    buffer = (char *)malloc(sizeof(char) * fsize);
    if(NULL == buffer){
        printf("malloc buffer erro. \n");
        exit(-2);
    }

    result = fread(buffer, 1, fsize, fp);
    if(fsize != result){
        printf("readf buffer erro.\n");
        exit(-3);
    }

    Elf32_Ehdr *ehdr = (Elf32_Ehdr*)(buffer);
    Elf32_Phdr *phdr = (Elf32_Phdr*)(buffer + ehdr->e_phoff);
    int i = 0;
    for( ; i < ehdr->e_phnum; i++, phdr++){
        printf("p_type:\t\t%#016x\n", phdr->p_type);
        printf("p_flags:\t%#016x\n", phdr->p_flags);
        printf("p_offset:\t%#016x \n", phdr->p_offset);
        printf("p_vaddr:\t%#016x\n", phdr->p_paddr);
        printf("p_filesz:\t%#016x\n", phdr->p_filesz);
        printf("p_memsz:\t%#016x\n", phdr->p_memsz);
        printf("p_align:\t%#016x\n", phdr->p_align);
        printf("\n===========================================\n");
    }
    fclose(fp);
    free(buffer);
    exit(0);
}
