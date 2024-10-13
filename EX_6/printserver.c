/*
 * printserver.c
 *
 * Created on Sat Oct 12 2024
 *
 * created by Kahsay Tinsae Ghilay
 * Matrikelnummer: ********
 *
 * Course tittle Operating Systems
 *
 *
 * receives message from client
 * uses message queue for interprocess comunication
 * 
 */

# include <stdio.h>
# include <stdlib.h>
# include "messagequeue.h"
# include <signal.h>

void sig_handler(int signum){
    printf("Sure you want to raise signal %d [Y/N]",signum);
    char buff[2]= {};
    while(fgets(buff,1,stdin) != NULL){
        if(strcmp(buff, "y")){
            printf("you pressed %s\n",buff);
            exit(EXIT_SUCCESS);
        }
    }
}
int main(){

    // create ms_queue
    mqd_t mqd = createForReceiver(MQ_NAME);
    // sender from this side
    mqd_t  callback_queue = createForSender(MQ_CALL);

    registerShutdownHandler(&sig_handler);

    // get its attribute
    struct mq_attr attr;
    getQueueAttribute(mqd, &attr);

    // buffer to hold message -> 
    // buffer size should be more than message size else seg_faul
    queue *buff = malloc(sizeof(queue));

    unsigned pr = 0;
    // this is a service. makes sense to always be listening
    // in real life,  this service would I imagine  sleep
    // there would be some kind of interrupt to wake it when new message is received
    // but here, I am goind to let it run continously
    while(1){
        

        // receive message from client
        receiveMessage(mqd,(char*)buff,attr,&pr);

        // print out 
        printf("Druckauftrag %s wird bearbeitet...\n",buff->name);
        int duration = (rand() % 10) + 1;
        // simulate print
        sleep(duration);
        int state = rand() % 100;
        // send feed back to client
        // errors can happen
        if(state < 10){
            printf("Fehler bei der Bearbeitung des Druckauftrags %s!\n", buff->name);
            sendMessage(&callback_queue,"Fail",5,1);
            continue;
        }
        sendMessage(&callback_queue,buff->message,sizeof(buff->message),1);
        printf("Druckauftrag %s abgeschlossen!\n", buff->name);
    }
    // done and clean up
    closeQueue(mqd);
    unlinkQueue(MQ_NAME);
    // close sender we are done with it
    close(callback_queue);
    free(buff);

    return EXIT_SUCCESS;
}
