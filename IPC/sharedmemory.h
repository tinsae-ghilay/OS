/*
 * sharedmemory.h
 *
 * Created on Thu Oct 09 2024
 *
 * created by Kahsay Tinsae Ghilay
 * Matricle Nummer ********
 *
 * Written for the course Operating Systems
 * 
 * Header file for sharedmemory.c
 * declares functions to create and manage shared memory
 */

# ifndef SHAREDMEMORY_H
# define SHAREDMEMORY_H

# include <sys/mman.h>
# include <unistd.h>

// shared memory name
# define SH_NAME "/sharedmemory"
// permission RW,RW,R
# define PERMISSION 0664

// shared momory size
# define SH_SIZE sizeof(shared_data_t)
# define MSG_SIZE 1024


typedef struct
{
    // the actual message
    char buff[MSG_SIZE];
    // flag set to 1 if new message arrives
    int read;
} shared_data_t;


// create shared memory for receiver
int createSharedForRead();

// create shared memory for sender
int createSharedForWrite();

// Configure the size of the shared memory segment
void configureMem(int memd);

// map the shared memory segment
void* mapSegment(int memd);

//unmap shared memory
void unmapShared(void* adr);

// unlink shared memory
void unlinkShared();

# endif //SHAREDMEMORY_H