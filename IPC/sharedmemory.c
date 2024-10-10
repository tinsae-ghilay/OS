/*
 * sharedmemory.c
 *
 * Created on Thu Oct 09 2024
 *
 * created by 2024 Kahsay Tinsae Ghilay
 * Matricle Nummer 01640282
 *
 * Written for the course Operating Systems
 */

# include "sharedmemory.h"
# include <stdio.h>
#include <fcntl.h>
# include <stdlib.h>

// exit on error
static void fin(char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

// create shared memory for receiver
int createSharedForRead()
{
    int shm = shm_open(SH_NAME,O_RDWR, PERMISSION);
    if(shm == -1){
        fin("Error openning Shared memory for read");
    }
    return shm;
}

// create shared memory for sender
int createSharedForWrite()
{
    // Create the shared memory object
	int shm = shm_open(SH_NAME, O_CREAT | O_RDWR, PERMISSION);
	if (shm == -1) {
		fin("Error creating shared memory for Write");
	}
    return shm;
}

// Configure the size of the shared memory segment
void configureMem(int memd)
{
    if(ftruncate(memd,SH_SIZE) == -1){
        fin("Error configuring memory size");
    }
}

// map the shared memory segment
// the we only have to write to this segment
// and the other procces is able to read it using because it has this adress too
void* mapSegment(int memd)
{
    // address of memory segment
    void* seg = mmap(0,SH_SIZE,PROT_WRITE,MAP_SHARED,memd,0);

    if(seg == MAP_FAILED){ // not mapped
        fin("Error maping Shared segment to memory");
    }

    return seg;
}

//unmap shared memory segment from memory
// we pass the pointer to the segment
void unmapShared(void* adr)
{
	if (munmap(adr, SH_SIZE) == -1) { // unable to unmap
		fin("Could not unmap the shared memory segment from memory");
	}
}

// unlink shared memory
void unlinkShared()
{
    if(shm_unlink(SH_NAME)){
        fin("Error unlinking shared memory");
    }
}