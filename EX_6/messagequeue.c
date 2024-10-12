/*
 * messagequeue.c
 *
 * Created on Thu Oct 08 2024
 *
 * created by Kahsay Tinsae Ghilay
 * Matricle Nummer ********
 *
 * Written for the course Operating Systems
 * 
 * 
 * misc functions to create messagequeue
 * send message through messageQueue
 * close and unlink messageQueue
 */

# include "messagequeue.h"
# include <mqueue.h>

// creates messagequeue fro receiver
int createForReceiver(char *name)
{
    mqd_t mqd = mq_open(name, O_RDWR);
    if(mqd == -1){
        perror("Error openning message queue from receiver");
        exit(EXIT_FAILURE);
    }
    return mqd;
}

// creates messagequeue fro sender
int createForSender(char *name)
{
    // we open message que with O_CREATE from this because this will be sender
    mqd_t mqd = mq_open(name, (O_CREAT|O_WRONLY),PERMISSION,NULL);
    // check if msg queue was created successfuly
    if(mqd == -1) {
        perror("Could not create msg queue from sender");
        exit(EXIT_FAILURE);
    }
    return mqd;
}

// send message // add priority to this
void sendMessage(mqd_t *mqd, char *message, int length, int priority)
{
    // lets try to send message
    if(mq_send(*mqd,message,length,priority)==-1) { // message was not sent?
        perror("Couldnt send message\n");
        exit(EXIT_FAILURE);
    }
}

// gets message queue attribute
void getQueueAttribute(mqd_t mqd, struct mq_attr *attr){
    if(mq_getattr(mqd,attr) == -1){
        perror("couldnt get attributes from queue");
        exit(EXIT_FAILURE);
    }
}

// receive message
void receiveMessage(mqd_t mqd, char*msg, struct mq_attr attr, unsigned *priority){
    // check if there is a message
    if(mq_receive(mqd,msg,attr.mq_msgsize,priority) == -1){
        perror("Could not receive message");
        exit(EXIT_FAILURE);
    }
}

// closes message queue
void closeQueue(mqd_t mqd)
{
    if(mq_close(mqd) == -1){
        perror("Error closing msg queue");
        exit(EXIT_FAILURE);
    }
}

// unlinks message queue
// only needed in receiver.
void unlinkQueue(char *name)
{
    // trying to unlink ms_queue
    if(mq_unlink(name) == -1){
        perror("Error unlinking mq");
        exit(EXIT_FAILURE);
    }
}