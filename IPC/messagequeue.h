/*
 * messagequeue.h
 *
 * Created on Thu Oct 08 2024
 *
 * created by Kahsay Tinsae Ghilay
 * Matricle Nummer ********
 *
 * Written for the course Operating Systems
 * 
 * 
 * Header file for messagequeue.c
 * misc functions to create messagequeue
 * send message through messageQueue
 * close and unlink messageQueue
 */

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

# include <mqueue.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>


# define MQ_NAME "/messageQueue"
// permission RW,RW,R (user,group, everyone else)
# define PERMISSION 0664 // user :rw-x, Group: rw-x, everybody: r-x

struct mq_attr;
// create messageQueue
int createForReceiver();
int createForSender();
// close and unlink message queue
void closeQueue(mqd_t mqd);
void unlinkQueue();
// will this also be possible?
void sendMessage(mqd_t *mqd, char *message, int length);
void receiveMessage(mqd_t mqd, char*msg, struct mq_attr attr, unsigned *priority);
// get attr.
void getQueueAttribute( mqd_t mqd, struct mq_attr *attr);

#endif //MESSAGEQUEUE_H



