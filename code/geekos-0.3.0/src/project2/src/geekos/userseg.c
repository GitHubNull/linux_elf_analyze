/*
 * Segmentation-based user mode implementation
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.23 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/ktypes.h>
#include <geekos/kassert.h>
#include <geekos/defs.h>
#include <geekos/mem.h>
#include <geekos/string.h>
#include <geekos/malloc.h>
#include <geekos/int.h>
#include <geekos/gdt.h>
#include <geekos/segment.h>
#include <geekos/tss.h>
#include <geekos/kthread.h>
#include <geekos/argblock.h>
#include <geekos/user.h>

/* ----------------------------------------------------------------------
 * Variables
 * ---------------------------------------------------------------------- */

#define DEFAULT_USER_STACK_SIZE 8192


/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */


/*
 * Create a new user context of given size
 */


static struct User_Context* Create_User_Context(ulong_t size)
{
	struct User_Context * UserContext;


	size = Round_Up_To_Page(size);
	UserContext = (struct User_Context *)Malloc(sizeof(struct User_Context));

	if (UserContext != 0)
		UserContext->memory = Malloc(size);
	else
		goto fail;

	if (UserContext->memory == 0)
		goto fail;

	memset(UserContext->memory, '\0', size);

	UserContext->size = size;

    /* Allocate an LDT descriptor for the user context */
    UserContext->ldtDescriptor = Allocate_Segment_Descriptor();
    if (UserContext->ldtDescriptor == 0)
        goto fail;
   
    Init_LDT_Descriptor(UserContext->ldtDescriptor, UserContext->ldt, NUM_USER_LDT_ENTRIES);

    
    UserContext->ldtSelector = Selector(KERNEL_PRIVILEGE, true, Get_Descriptor_Index(UserContext->ldtDescriptor));

    /* Initialize code and data segments within the LDT */
    Init_Code_Segment_Descriptor(
        &UserContext->ldt[0],
        (ulong_t) UserContext->memory,
        size / PAGE_SIZE,
        USER_PRIVILEGE
    );
    Init_Data_Segment_Descriptor(
        &UserContext->ldt[1],
        (ulong_t) UserContext->memory,
        size / PAGE_SIZE,
        USER_PRIVILEGE
    );
    UserContext->csSelector = Selector(USER_PRIVILEGE, false, 0);
    UserContext->dsSelector = Selector(USER_PRIVILEGE, false, 1);

    UserContext->refCount = 0;	


	return UserContext;

fail:

	if (UserContext != 0)
	{
		if (UserContext->memory != 0)
			Free(UserContext->memory);
		Free(UserContext);
	}

	return 0;
}


static bool Validate_User_Memory(struct User_Context* userContext,
    ulong_t userAddr, ulong_t bufSize)
{
    ulong_t avail;

    if (userAddr >= userContext->size)
        return false;

    avail = userContext->size - userAddr;
    if (bufSize > avail)
        return false;

    return true;
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Destroy a User_Context object, including all memory
 * and other resources allocated within it.
 */
void Destroy_User_Context(struct User_Context* userContext)
{
    /*
     * Hints:
     * - you need to free the memory allocated for the user process
     * - don't forget to free the segment descriptor allocated
     *   for the process's LDT
     */
	Free_Segment_Descriptor(userContext->ldtDescriptor);

	Disable_Interrupts();
    Free(userContext->memory);
    Free(userContext);
    Enable_Interrupts();
}

/*
 * Load a user executable into memory by creating a User_Context
 * data structure.
 * Params:
 * exeFileData - a buffer containing the executable to load
 * exeFileLength - number of bytes in exeFileData
 * exeFormat - parsed ELF segment information describing how to
 *   load the executable's text and data segments, and the
 *   code entry point address
 * command - string containing the complete command to be executed:
 *   this should be used to create the argument block for the
 *   process
 * pUserContext - reference to the pointer where the User_Context
 *   should be stored
 *
 * Returns:
 *   0 if successful, or an error code (< 0) if unsuccessful
 */
int Load_User_Program(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat, const char *command,
    struct User_Context **pUserContext)
{
    /*
     * Hints:
     * - Determine where in memory each executable segment will be placed
     * - Determine size of argument block and where it memory it will
     *   be placed
     * - Copy each executable segment into memory
     * - Format argument block in memory
     * - In the created User_Context object, set code entry point
     *   address, argument block address, and initial kernel stack pointer
     *   address
     */
   	ulong_t maxva = 0;
	unsigned numArgs;
	ulong_t argBlockSize;
	ulong_t virtSize, argBlockAddr;
	struct User_Context * UserContext = 0;
	int i;


	for (i = 0; i < exeFormat->numSegments; i++)
	{
		struct Exe_Segment * segment = &exeFormat->segmentList[i];
		ulong_t topva = segment->startAddress + segment->sizeInMemory;

		if (topva > maxva)
			maxva = topva;
	}
	
	Get_Argument_Block_Size(command, &numArgs, &argBlockSize);

	virtSize = Round_Up_To_Page(maxva) + DEFAULT_USER_STACK_SIZE;
	argBlockAddr = virtSize;
	virtSize += argBlockSize;

	UserContext = Create_User_Context(virtSize);

	if (UserContext == 0)
		return -1;


   	for (i = 0; i < exeFormat->numSegments; ++i) 
	{
		struct Exe_Segment *segment = &exeFormat->segmentList[i];

		memcpy(UserContext->memory + segment->startAddress,
		exeFileData + segment->offsetInFile,
		segment->lengthInFile);
   	 }

	Format_Argument_Block(UserContext->memory + argBlockAddr, numArgs, argBlockAddr, command);


	UserContext->argBlockAddr = argBlockAddr;
	UserContext->stackPointerAddr = argBlockAddr;

	UserContext->entryAddr = exeFormat->entryAddr;


	*pUserContext = UserContext;

	return 0;
}

/*
 * Copy data from user memory into a kernel buffer.
 * Params:
 * destInKernel - address of kernel buffer
 * srcInUser - address of user buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */

//--:used by some sys calls
bool Copy_From_User(void* destInKernel, ulong_t srcInUser, ulong_t bufSize)
{
    /*
     * Hints:
     * - the User_Context of the current process can be found
     *   from g_currentThread->userContext
     * - the user address is an index relative to the chunk
     *   of memory you allocated for it
     * - make sure the user buffer lies entirely in memory belonging
     *   to the process
     */
	struct User_Context * UserContext = g_currentThread->userContext;

	//--: check if memory if validated
	if (!Validate_User_Memory(UserContext,srcInUser, bufSize))
		return false;

	//--:user->kernel
	memcpy(destInKernel, UserContext->memory + srcInUser, bufSize);
    	return true;
}

/*
 * Copy data from kernel memory into a user buffer.
 * Params:
 * destInUser - address of user buffer
 * srcInKernel - address of kernel buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */
bool Copy_To_User(ulong_t destInUser, void* srcInKernel, ulong_t bufSize)
{
    /*
     * Hints: same as for Copy_From_User()
     */
	struct User_Context * UserContext = g_currentThread->userContext;

	//--: check if memory if validated
	if (!Validate_User_Memory(UserContext, destInUser,  bufSize))
		return false;
	
	//--:kernel->user
	memcpy(UserContext->memory + destInUser, srcInKernel, bufSize);

	return true;
}

/*
 * Switch to user address space belonging to given
 * User_Context object.
 * Params:
 * userContext - the User_Context
 */
void Switch_To_Address_Space(struct User_Context *userContext)
{
    /*
     * Hint: you will need to use the lldt assembly language instruction
     * to load the process's LDT by specifying its LDT selector.
     */
     ushort_t ldtSelector = userContext->ldtSelector;
	//--:mov [ldtSelector] to ldtr using lldt
    __asm__ __volatile__ (
	"lldt %0"
	:
	: "a" (ldtSelector)
	);
}

