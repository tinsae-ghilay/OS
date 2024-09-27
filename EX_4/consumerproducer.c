#include <stdio.h>
# include "circularbuffer.h"
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>

circularbuffer_t buffer;

# define  VALUE  40

void* producer_function(void* args) {

    const int *id = args;

    for(int i = 0; i < VALUE; i++) {
        const cb_element_t temprature = rand() % 50;

        cb_add(&buffer,temprature, *id);
    }
    // this was the solution all along.
    // we tried to set buffer's exit flag here.
    // but it had to be set by the buffer itsel. I looked around and found it in this function
    // in buffer. the same on line 43 too
    cb_finalize(&buffer);
    printf("Producesr has done its job\n");
    return NULL;
}

void* consumer_function(void* arg) {
    for(int i = 1; i < VALUE; i++) {

        int *avg = arg;
        *avg = (*avg + cb_remove(&buffer))/2;
        printf("Temprature is : %d\n",*avg);
        //usleep(1000000);
    }
    cb_finalize(&buffer);
    printf("Consumer has done its job\n");
    return NULL;

}
int main(void)
{
    int average = 0;
    cb_init(&buffer,7);
    int prod_1 = 1, prod_2= 2;
    pthread_t producer_1, producer_2, consumer;
    pthread_create(&producer_1,NULL, producer_function, &prod_1);
    pthread_create(&producer_2,NULL, producer_function, &prod_2);
    pthread_create(&consumer,NULL, consumer_function, &average);

    if(pthread_join(producer_1, NULL)) {
        printf("thread 1 joine error\n");
    }
    if(pthread_join(producer_2, NULL)) {
        printf("thread 2 joine error\n");
    }
    if(pthread_join(consumer, NULL)) {
        printf("Consumer thread  joine error\n");
    }

    cb_destroy(&buffer);

    return 0;
}
