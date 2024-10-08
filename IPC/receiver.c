#include <stdio.h>
# include <mqueue.h>
# include <stdlib.h>
# include <string.h>

#define MY_Q "/myqueue"

int main(void) {

    // message que name
    // we open message que with O_RDONLY , we just want to read as test
    mqd_t mqd = mq_open(MY_Q, O_RDONLY);

    // struct helps us to get the maximal amount of message?
    struct mq_attr attr;

    // check if msg queue was created successfuly
    if(mqd == -1) {
        perror("Could not create msg queue\n");
        exit(EXIT_FAILURE);
    }

    // buffer to hold the message
    char msg[2048];
    // status of message received
    int status = mq_getattr(mqd,&attr);
    // message priority
    int pr = 0;
    // probably has to check for null.(
    if( status == -1){

        printf("couldnt get attributes from queue\n");
        exit(EXIT_FAILURE);
    }

    // we live while we receive
    while(1){

        // check if there is a message
        if(mq_receive(mqd,msg,attr.mq_msgsize,&pr) == -1){
            perror("Could not receive message");
            exit(EXIT_FAILURE);
        }
        // are we done?
        if(strcmp(msg,"done\n") == 0){ // yes we are done
            printf("yey! we are done !!\n");
            break;
        }
        // print message or tell it to somebody who cares?
        printf("Heard: %s",msg);
    }
    
    // trying to close ms_queue
    if(mq_close(mqd) == -1){
        perror("Error closing message queue");
        exit(EXIT_FAILURE);
    }

    // trying to unlink ms_queue
    if(mq_unlink(MY_Q) == -1){
        perror("Error unlinking mq");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}