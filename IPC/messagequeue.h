/*
 * messagequeue.h
 *
 * Created on Thu Oct 08 2024
 *
 * created by Kahsay Tinsae Ghilay
 * Matricle Nummer 01640282
 *
 * Written for the course Operating Systems
 */

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

# include <mqueue.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>


# define MQ_NAME "/messageQueue"
// lets test this
# define PERMISSION 0664 // user :rw-x, Group: rw-x, everybody: r-x

struct mq_attr;

int createForReceiver();
int createForSender();
// close and unlink message queue
void closeQueue(mqd_t mqd);
void unlinkQueue();
// will this also be possible?
void sendMessage(mqd_t *mqd, char *message, int length);
void receiveMessage(mqd_t mqd, char*msg, struct mq_attr attr, unsigned *priority);

void getQueueAttribute( mqd_t mqd, struct mq_attr *attr);

#endif //MESSAGEQUEUE_H



