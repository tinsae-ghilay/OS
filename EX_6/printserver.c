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

int main(){

    // create ms_queue
    mqd_t mqd = createForReceiver(MQ_NAME);

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
    int cycle = 0;
    while(cycle < 5){
        // sender from this side
        mqd_t  callback_queue = createForSender(MQ_CALL);

        // receive message from client
        receiveMessage(mqd,(char*)buff,attr,&pr);
        // print out 
        printf("Druckauftrag %s wird bearbeitet...\n",buff->name);
        int duration = (rand() % 10) + 1;
        // simulate print
        sleep(duration);
        int state = rand() % 100;
        cycle++;
        // send feed back to client
        sendMessage(&callback_queue,"1",2,1);
        // close sender we are done with it
        close(callback_queue);
        // errors can happen
        if(state < 10){
            printf("Fehler bei der Bearbeitung des Druckauftrags %s!\n", buff->name);
            continue;
        }
        printf("Druckauftrag %s abgeschlossen!\n", buff->name);
    }
    // done and clean up
    closeQueue(mqd);
    unlinkQueue(MQ_NAME);
    free(buff);

    return EXIT_SUCCESS;
}
