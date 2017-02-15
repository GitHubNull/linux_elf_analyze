#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>

int readElfIn(char *fileName);
int isElf(char *tag);
int isExcute(int tag);
int initSrc(char *src);
int excuteFile(char *src);
int isStop(char *tag);
int stop(char *src);

int main(int argc, char **argv)
{
  if(2 > argc){
    printf("argc is not enough!\n");
    exit(-1);
  }if(0 == strcmp("", argv[1])){
    printf("argv[1] = null!\n");
    exit(-1);
  }
  FILE* fp = fopen(argv[1], "rb");
  if(NULL == fp){
    printf("faied to open file: %s.\n", argv[1]);
    exit(-1);
  }

  Elf32_Ehdr hdr;
  if(1 != fread(&hdr, sizeof(hdr), 1, fp)){
    printf("failed to read elf header.\n");
    exit(-1);
  }

  char *strp = (char *)malloc(sizeof(char) * (4));
  sprintf(strp, "%c%c%c", hdr.e_ident[EI_MAG1], hdr.e_ident[EI_MAG2], hdr.e_ident[EI_MAG3]);
  strp[3] = 0;
  if(0 == isElf(strp)){
    printf("file: %s is elf file.\n", argv[1]);
  }

  if(0 == isExcute(hdr.e_type)){
    printf("file: %s is a excuteable file.\n", argv[1]);
  }
  return 0;
}

int isElf(char *tag)
{
  if(NULL != strstr(tag, "ELF")){
    return 0;
  }else{
    return -1;
  }
}

int isExcute(int tag)
{
  if(2 == tag){
    return 0;
  }else{
    return -1;
  }
}
