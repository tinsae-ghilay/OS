/*
 * messagequeue.c
 *
 * Created on Thu Oct 08 2024
 *
 * created by 2024 Kahsay Tinsae Ghilay
 * Matricle Nummer 01640282
 *
 * Written for the course Operating Systems
 */

# include "messagequeue.h"
# include <mqueue.h>

// creates messagequeue fro receiver
int createForReceiver()
{
    mqd_t mqd = mq_open(MQ_NAME, O_RDONLY);
    if(mqd == -1){
        perror("Error openning message queue from receiver");
        exit(EXIT_FAILURE);
    }
    return mqd;
}

// creates messagequeue fro sender
int createForSender()
{
    // we open message que with O_CREATE from this because this will be sender
    mqd_t mqd = mq_open(MQ_NAME, (O_CREAT|O_WRONLY),PERMISSION,NULL);
    // check if msg queue was created successfuly
    if(mqd == -1) {
        perror("Could not create msg queue from sender");
        exit(EXIT_FAILURE);
    }
    return mqd;
}

// send message
void sendMessage(mqd_t *mqd, char *message, int length)
{
    // lets try to send message
    if(mq_send(*mqd,message,length,3)==-1) { // message was not sent?
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
void unlinkQueue()
{
    // trying to unlink ms_queue
    if(mq_unlink(MQ_NAME) == -1){
        perror("Error unlinking mq");
        exit(EXIT_FAILURE);
    }
}