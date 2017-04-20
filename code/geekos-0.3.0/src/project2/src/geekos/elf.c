/*
 * ELF executable loading
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.29 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/kassert.h>
#include <geekos/ktypes.h>
#include <geekos/screen.h>  /* for debug Print() statements */
#include <geekos/pfat.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/user.h>
#include <geekos/elf.h>


/**
 * From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 *   and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
 #if 0	// not used here!--Late Lee
int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat)
{
    elfHeader *elf_header;
    programHeader *phdr;
    int i;

    elf_header= (elfHeader *) exeFileData;

    /*
     * FIXME: when checking offsets, we really ought to be
     * checking overflow cases.  Need to use functions from
     * range.h (which needs to be implemented, too)
     */

    if (exeFileLength < sizeof(elfHeader) ||
	strncmp(exeFileData, "\x7F""ELF", 4) != 0) {
	return ENOEXEC;
    }

    if (elf_header->phnum > EXE_MAX_SEGMENTS) {
	return ENOEXEC;
    }

    if (exeFileLength < elf_header->phoff + (elf_header->phnum * sizeof(programHeader))) {
	return ENOEXEC;
    }

    exeFormat->numSegments = elf_header->phnum;
    exeFormat->entryAddr = elf_header->entry;

    phdr = (programHeader *) (exeFileData + elf_header->phoff);
    for (i = 0; i < elf_header->phnum; ++i) {
	struct Exe_Segment *segment = &exeFormat->segmentList[i];

	/*
	 * Fill in segment offset, length, address
	 * FIXME: should check that segments are valid
	 */
	segment->offsetInFile = phdr[i].offset;
	segment->lengthInFile = phdr[i].fileSize;
	segment->startAddress = phdr[i].vaddr;
	segment->sizeInMemory = phdr[i].memSize;

	if (segment->lengthInFile > segment->sizeInMemory) {
	    return ENOEXEC;
	}

    }

    /* Groovy */
    return 0;
}
 #endif

// copy from P1 ^_^
 int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat)
{
  int i = 0;  
  elfHeader *elf_head = (elfHeader *)exeFileData;
  programHeader *phdr = (programHeader *)(exeFileData + elf_head->phoff);  
  struct Exe_Segment *segment = exeFormat->segmentList;
  for (i = 0; i < elf_head->phnum; i++) {
      segment->offsetInFile = phdr->offset;
      segment->lengthInFile = phdr->fileSize;
      segment->startAddress = phdr->vaddr;
      segment->sizeInMemory = phdr->memSize;
      phdr++;
      segment++;
  }

  exeFormat->numSegments = elf_head->phnum;
  exeFormat->entryAddr = elf_head->entry;
  return 0;
}