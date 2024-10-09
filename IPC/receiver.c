#include <stdio.h>
# include <mqueue.h>
# include <stdlib.h>
# include <string.h>
# include "messagequeue.h"

int main(void) {

    // message que name
    // we open message que with O_RDONLY , we just want to read as test
    mqd_t mqd = createForReceiver();

    // lets get message attributes
    struct mq_attr attr;
    getQueueAttribute(mqd,&attr);


    // buffer to hold the message
    char msg[attr.mq_msgsize+1];
    
    // message priority
    unsigned pr = 0;

    // we live while we receive
    while(1){

        // receive message
        receiveMessage(mqd,msg,attr,&pr);
        
        // are we done?
        if(strcmp(msg,"done\n") == 0){ // yes we are done
            printf("yey! we are done !!\n");
            break;
        }
        // print message or tell it to somebody who cares?
        printf("Heard: %s",msg);
    }
    
    // trying to close ms_queue
    closeQueue(mqd);

    // trying to unlink ms_queue
    unlinkQueue();
    return EXIT_SUCCESS;
}