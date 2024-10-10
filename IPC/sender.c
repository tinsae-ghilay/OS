/*
 * sender.c
 *
 * Created on Thu Oct 09 2024
 *
 * created by 2024 Kahsay Tinsae Ghilay
 * Matricle Nummer 01640282
 *
 * Written for the course Operating Systems
 */

# include <stdio.h>
# include <mqueue.h>
# include <stdlib.h>
# include <string.h>
# include "messagequeue.h"

int main(void) {

    // message que name
    // we cread message queue
    mqd_t mqd = createForSender();
    
    // lets get message from stdin
    char msg[1024] = {0};
    printf("write a message to send\n > ");
    // while we have input we keep sending it.
    while(fgets(msg, 1023,stdin) != NULL) {

        // unless we want to exit of course
        if(strcmp(msg,"exit\n") == 0){

            break;
        }
        // lets send message
        sendMessage(&mqd,msg,1024);
        // prompt every time we want to send
        printf(" > ");
    }
    // we are done. so lets close message queue
    closeQueue(mqd);
    return EXIT_SUCCESS;
}
