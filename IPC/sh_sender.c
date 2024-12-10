/*
 * sh_sender.c
 *
 * Created on Thu Oct 09 2024
 *
 * created by Kahsay Tinsae Ghilay
 * Matricle Nummer ********
 *
 * Written for the course Operating Systems
 * 
 * 
 * A process that shares a memory with another
 * writes a string to shared memory and sets it'S read flag to 0
 * gets input from user(stdin)
 */

# include <stdio.h>
# include <stdlib.h>
# include "sharedmemory.h"
# include <string.h>


int main()
{
    // create memory segment
    int memd = createSharedForWrite();

    // configure segment
    configureMem(memd);

    // map segment
    void* adr = mapSegment(memd);

    // because we set the size of segment to our struct size when configuring
    shared_data_t *msg = (shared_data_t*) adr;
    printf("Write your messages below \n > ");

    // now we can read the message from this pointer
    while(fgets(msg->buff, MSG_SIZE-1,stdin) != NULL){
        // because we wrote new data to shared memory, we set flag to 0(not read)
        msg->read = 0;
        // we can also add an exit mechanism like we did in ms_queue
        // by comparing string with keyword of our choice
        // e.g 
        if(strcmp(msg->buff, "exit\n") == 0){
            printf("yey! we are done\n");
            break;
        }
        printf(" > ");
    }

    // unmap segment
    unmapShared(adr);
}