#include <stdio.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool readElfIn(FILE *fp, char **elfTag, int *excuteFile);
bool isElf(char *tag);
bool isExcute(int tag);
bool initSrc(char *src);
bool excuteFile(char *src);
bool isStop(char *tag);
bool stop(char *src);

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
  char *elfTag;
  int excuteTag = -1;
  if(false == readElfIn(fp, &elfTag, &excuteTag)){
    printf("readElfin error!\n");
    exit(-1);
  }
  if(true == isElf(elfTag)){
    printf("[%s] is a elf file.\n", argv[1]);
  }else{
    printf("[%s] is not a elf file.\n", argv[1]);
    exit(-1);
  }
  if(true == isExcute(excuteTag)){
    printf("[%s] is a excuteable file.\n", argv[1]);
  }else{
    printf("[%s] is not a excuteable file.\n", argv[1]);
    exit(-1);
  }
  return 0;
}

bool readElfIn(FILE *fp, char **elfTag, int *excuteTag)
{
  Elf64_Ehdr hdr;
  if(1 != fread(&hdr, sizeof(hdr), 1, fp)){
    printf("failed to read read elf header.\n");
    return false;
  }
  (*elfTag) = (char *)malloc(sizeof(char) * (4));
  sprintf(*elfTag, "%c%c%c%c", hdr.e_ident[EI_MAG1], hdr.e_ident[EI_MAG2],
          hdr.e_ident[EI_MAG3], '\0');
  (*excuteTag) = hdr.e_type;
  return true;
}

bool isElf(char *tag)
{
  if(0 == strcmp(tag, "ELF")){
    return true;
  }else{
    return false;
  }
}

bool isExcute(int tag)
{
  if(2 == tag){
    return true;
  }else{
    return false;
  }
}
