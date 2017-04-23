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
int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength,
		struct Exe_Format *exeFormat)
{
	//TODO("Parse an ELF executable image");
	    int i = 0;
    Print("elf file length: %d \n", (int)exeFileLength);

    elfHeader *elf_head = (elfHeader *)exeFileData;

    exeFormat->numSegments = elf_head->phnum;
    exeFormat->entryAddr = elf_head->entry;

    programHeader *ph = (programHeader *)(exeFileData + elf_head->phoff);
    for(i=0;i<elf_head->phnum;i++)
    {
        exeFormat->segmentList[i].offsetInFile = ph[i].offset;
        exeFormat->segmentList[i].lengthInFile = ph[i].fileSize;
        exeFormat->segmentList[i].startAddress = ph[i].paddr;
        exeFormat->segmentList[i].sizeInMemory = ph[i].memSize;
        exeFormat->segmentList[i].protFlags = ph[i].flags;
    }

    return 0;
}


// static int load_elf_binary(struct geekos_binprm *bprm){
//     struct file *interpreter = NULL;

//     struct  elf_phdr *elf_ppnt, *elf_phdata, *interp_elf_phdata = NULL;
//     int retval = 0, i = 0;

//     char *elf_interpreter = NULL;
//     struct {
//         struct elfhdr elf_ex;
//         struct elfhdr interp_elf_ex;
//     }*ies;  //interpreter_elf_struct

//     // get the exec_header
//     ies->elf_ex = *((struct elfhdr *)bprm->buf);

//     // check the magin number
//     if(0 != memcmp(ies->elf_ex.e_ident, ELFMAG, SELFMAG))
//         goto out;

//     if((ET_EXEC != ies->elf_ex.e_type) && (ET_DYN != ies->elf_ex.e_type))
//         goto out;

//     elf_phdata = load_elf_phdrs(&ies->elf_ex, bprm->file);
//     if(!elf_phdata)
//         goto out;

//     elf_ppnt = elf_phdr;

//     for(; i < ies->elf_ex.e_phnum; i++){
//         if(elf_ppnt->p_type == PT_INTERP){
//             retval = -ENOEXEC;
//             if(elf_ppnt->p_filesz > PATH_MAX || elf_ppnt->p_filesz < 2)
//                 goto out_free_ph;
//             retval = ENOMEM;
//             // elf_interpreter = kmalloc(elf_ppnt->p_filesz,)
//         }
//         elf_ppnt++;
//     }

//     elf_ppnt = elf_phdata;
//     for(i = 0; i < ies->elf_ex.e_phnum; i++, elf_ppnt++)
//         switch(elf_ppnt->p_type){
//             case
//         }

//     // check for interpreter
//     if(elf_interpreter){
//         retval = -ELIBBAD;
//         if(0 != memcmp(ies->interp_elf_ex.e_ident, ELFMAG, SELFMAG))
//             goto out_free_dentry;

//         interp_elf_phdata = load_elf_phdrs(&ies->interp_elf_ex, interpreter);
//         if(!interp_elf_phdata)
//             goto out_free_dentry;

//         elf_ppnt = interp_elf_phdata;
//         for(i = 0; i < ies->interp_elf_ex.e_phnum; i++, elf_ppnt)
//             switch(elf_ppnt->p_type){

//             }
//     }

//     // map elf data
//     for(i = 0, elf_ppnt = elf_phdata; i < ies->elf_ex.e_phnum; i++, elf_ppnt++){

//     }

//     start_thread(regs, elf_entry, bprm->p);

// }
