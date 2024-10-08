#include <stdio.h>
# include <mqueue.h>
# include <stdlib.h>
# include <string.h>

#define MY_Q "/myqueue"

int main(void) {

    // message que name
    // we open message que with O_CREATE from this because this will be sender
    mqd_t mqd = mq_open(MY_Q, (O_CREAT|O_WRONLY),0666,NULL);
    // check if msg queue was created successfuly
    if(mqd == -1) {
        perror("Could not create msg queue\n");
        exit(EXIT_FAILURE);
    }
    // lets get message from stdin
    char msg[1024];
    int status = 0;
    printf("write a message to send\n > ");
    // while we have input we keep sending it.
    while(fgets(msg, 1023,stdin) != NULL) {

        // unless we want to exit of course
        if(strcmp(msg,"exit\n") == 0){

            break;
        }
        // lets send message
        status = mq_send(mqd,msg,1024,3);

        if(status==-1) { // message was not sent?
            perror("Couldnt send message\n");
            exit(EXIT_FAILURE);
        }
        printf(" > ");
    }
    // we are done. so lets close message queue
    if(mq_close(mqd) == -1){
        perror("error closing msg queue");
    }
    return EXIT_SUCCESS;
}
