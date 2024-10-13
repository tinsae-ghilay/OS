/*
 * client.c
 *
 * Created on Sat Oct 12 2024
 *
 * by Kahsay Tinsae Ghilay
 * Course tittle Operating Systems
 *
 *
 * command line arguments 2,
 * Auftrag and priority. 
 * sends a message to another 'printerervice.c'
 * 
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include "messagequeue.h"
# include <time.h>

char  MESSAGE[8][30]= {"Hello this is a message to\n"," printer service prints this\n", 
                    "This is another print message\n", "please print this paragraph\n", 
                    "Hey printer.Thanks for print\n","Ok this is getting serious.\n", 
                    "this is too much"," this is final text\n"};



int main(int argv, char** argc){
    
    // have we got enough arguments?
    if(argv < 3){ // not enough arguments
        perror("Please provide enough arguments $name, $priorität");
        return EXIT_FAILURE;
    }

    // now lets get arguments and put them in struct content
    queue *msg = malloc(sizeof(queue));
    strncpy(msg->name,argc[1],10* sizeof(char));

    // creating message que for sending
    mqd_t mqd = createForSender(MQ_NAME);

    // set attribute of mesage que
    // random index for message
    srand(time(NULL));
    int i = rand() % 8;

    // copy messages to attribute
    strncpy(msg->message,MESSAGE[i],sizeof(MESSAGE[i]+1));
    unsigned pr = atoi(argc[2]); 

    // sending message
    sendMessage(&mqd, (char*)msg,sizeof(queue),pr);
    // struct to receive feed back
    struct mq_attr attr;
    // feed back queue
    mqd_t caller = createForReceiver(MQ_CALL);
    printf("Sent message[%d] with priority %d to service. \nWaiting for response...\n",i ,pr);
    // get attribute of feedback queue
    getQueueAttribute(caller, &attr);

    // buffer initialised because valgrind complained
    char callback_message[attr.mq_msgsize];

    // lets wait for response and print it
    receiveMessage(caller,callback_message,attr,&pr);
    printf("Server response: %s\n", callback_message);
    
    free(msg);
    // close 
    closeQueue(mqd);
    
    // these are a problem. if I close them here, 
    // I can seem to get them when new instance of this proccess is run again
    //closeQueue(caller);
    //unlinkQueue(MQ_CALL);
    return EXIT_SUCCESS;
}

