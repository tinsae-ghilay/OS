/*
 * sharedmemory.c
 *
 * Created on Thu Oct 09 2024
 *
 * created by Kahsay Tinsae Ghilay
 * Matricle Nummer ********
 *
 * Written for the course Operating Systems
 */
#define _XOPEN_SOURCE 700
# include "sharedmemory.h"
# include <stdio.h>
#include <fcntl.h>
# include <stdlib.h>
# include <signal.h>
# include <errno.h>

// exit on error
static void fin(char *msg){
    perror(msg);
	unlinkShared();
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
	int shm = shm_open(SH_NAME, O_CREAT| O_EXCL | O_RDWR, PERMISSION);
	// this may not be needed but since specificaly asked
	// EEXIST : Both O_CREAT and O_EXCL were specified to shm_open() and
    // the shared memory object specified by name already exists.
	// (source:https://man7.org/linux/man-pages/man3/shm_open.3.html)
	if(errno == EEXIST){
		fprintf(stderr,"Error: segment with the name %s already exists\n", SH_NAME);
		fin("Error");
	}
	// On success, shm_open() returns a file descriptor (a nonnegative integer)
	if (shm == -1) {
		fin("Error creating shared memory for Write");
	}
	// on success we return file descriptor
    return shm;
}


// Configure the size of the shared memory segment
void configureMem(int memd, int size)
{
	// On success, zero is returned.  On error, -1 is returned, and
    // errno is set to indicate the error.
	// (source: https://man7.org/linux/man-pages/man2/ftruncate.2.html)
    if(ftruncate(memd,size) == -1){
		close(memd);
        fin("Error configuring memory size");
    }
}

// map the shared memory segment
// the we only have to write to this segment
// and the other procces is able to read it using because it has this adress too
void* mapSegment(int memd, int size)
{
    // address of memory segment
    void* seg = mmap(0,size,PROT_WRITE,MAP_SHARED,memd,0);

	// On success, mmap() returns a pointer to the mapped area.  On
    // error, the value MAP_FAILED (that is, (void *) -1) is returned,
    // and errno is set to indicate the error. (source:https://man7.org/linux/man-pages/man2/mmap.2.html)
    if(seg == MAP_FAILED){ // not mapped
		close(memd);
        fin("Error maping Shared segment to memory");
    }

    return seg;
}

//unmap shared memory segment from memory
// we pass the pointer to the segment
void unmapShared(void* adr, int size)
{
	// On success, munmap() returns 0.  On failure, it returns -1, and
    // errno is set to indicate the error (probably to EINVAL).
	// (source: https://man7.org/linux/man-pages/man2/munmap.2.html)
	if (munmap(adr, size) == -1) { // unable to unmap
		fin("Could not unmap the shared memory segment from memory");
	}
}

// unlink shared memory
void unlinkShared()
{
	// On success, shm_unlink() returns 0.
	// (source:https://man7.org/linux/man-pages/man3/shm_open.3.html)
    if(shm_unlink(SH_NAME)){
        fin("Error unlinking shared memory");
    }
}

void registerShutdownHandler(void* fnc) {
    struct sigaction my_signal;

	my_signal.sa_handler = fnc;
	sigemptyset (&my_signal.sa_mask);
	sigaddset(&my_signal.sa_mask, SIGTERM);
	sigaddset(&my_signal.sa_mask, SIGINT);
	sigaddset(&my_signal.sa_mask, SIGQUIT);
	sigaddset(&my_signal.sa_mask, SIGALRM);
	my_signal.sa_flags = 0;

	if (sigaction(SIGTERM, &my_signal, NULL) != 0) {
		perror("Fehler beim Registrieren des Signal-Handlers");
		exit(1);
	}
	if (sigaction(SIGINT, &my_signal, NULL) != 0) {
		perror("Fehler beim Registrieren des Signal-Handlers");
		exit(1);
	}
	if (sigaction(SIGQUIT, &my_signal, NULL) != 0) {
		perror("Fehler beim Registrieren des Signal-Handlers");
		exit(1);
	}
	// Register SIGALRM handler
    if (sigaction(SIGALRM, &my_signal, NULL) != 0) {
        perror("Fehler beim Registrieren des Signal-Handlers");
        exit(1);
    }
}