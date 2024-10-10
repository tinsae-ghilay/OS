/*
 * sh_receiver.c
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
 * reads the shared memory and prints the value sets read flag to 1
 * sleeps if there is no new value
 */

# include <stdio.h>
# include <stdlib.h>
# include "sharedmemory.h"
# include <string.h>


int main()
{
    // create memory segment
    int memd = createSharedForRead();
    // map segment
    void* adr = mapSegment(memd);

    // because we set the size of segment to our struct size when configuring
    shared_data_t *msg = (shared_data_t*) adr;

    // now we can read the message from this pointer
    // but we should not repeat message again and agin if it isnt changed
    // so we need to check and  set flag from here
    // if it is 0(set from writer) then we print and set flag to 1

    //TODO("Works but it doesnt seem this is how shared memory is used");
    // this loop runs the fan like it is on adrenalin
    while(msg != NULL){
        // if message is already read we do nothing.
        // but sleep for quarter of a second (hopefully nobody notices).
        while(msg->read){
            usleep(25000);
        }
        // we can also add an exit mechanism like we did in ms_queue
        // by comparing string with keyword of our choice
        // e.g
        if(strcmp(msg->buff, "done\n") == 0){
            printf("yey! we are done\n");
            break;
        }
        // print message
        printf("Heard: %s", msg->buff);
        msg->read = 1;
    }

    // unmap segment
    unmapShared(adr);
    // unlink too
    unlinkShared();
}