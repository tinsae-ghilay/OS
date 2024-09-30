//
// Created by tgk on 9/29/24.
//
# include "buffer.h"
# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <unistd.h>

// test count
#define TEST_COUNT 3
#define SLEEP_SEC 1

// flag to let threads know test should end
int done_testing = 0;

// conditions for consumer and producer
pthread_cond_t c_cond, p_cond;
pthread_mutex_t mutex;

// circular buffer data structure to hold generated data
// this must be freed when done by calling free(b);
CircularBuffer *b;

// frees memory allocated in circular buffer
// calls destroy on condition variables and mutex
void cleanUp(){
    // free allocated memory and destroy mutex and conditions
    pthread_cond_destroy(&c_cond);
    pthread_cond_destroy(&p_cond);
    pthread_mutex_destroy(&mutex);
    free_buffer(b);
}

// consumer thread function
// locks circulr buffer (this is a critical region)
// if buffer is not empty reads data(random number)
// adds it to previously calculated temprature
// devides the result by 2 and calls it average temprature
// -> if buffer is empty, thread sleeps and notifies producer threads.
void *consume(void *arg){
    // variable to hold immidiate read value
    int t = 0;
    // variable to hold average temprature value
    int temp = 0;
    // lock buffer for reading
    pthread_mutex_lock(&mutex);

    // we continue lock while testing is in progress
    while(!done_testing){

		while(isEmpty(b)){ //while buffer is empty
            // thread should give up lock until buffer is not empty
            printf("Logging Programm wurde schlafengelegt!\n");
            pthread_cond_wait(&c_cond, &mutex);
            printf("Logging Programm wurde aufgeweckt!\n");
            sleep(SLEEP_SEC);
        }
        // if buffer is not empty, register data in t
        while(dequeue(b,&t)){
            // add all recorded tempratures
            temp = temp +t;
        }
        // signal in case other threads are waiting
        pthread_cond_signal(&p_cond);
        printf("Current average temprature : %d°C\n",temp/b->size);
        temp = 0;
	}
    // we are done, so broadcast condition to notify any threads that may be waiting
    pthread_mutex_unlock(&mutex);
    // and unlock mutex
    pthread_cond_broadcast(&p_cond);
    return NULL;
}

void *produce(void *arg){

    // geting id from arg as we pass it when craeting thread
    // we could get thread id from system by including <sys/syscall.h> with id = syscall(SYS_gettid).
    // but this is better for our use case as we dont realy need thread id
    int id = *(int*)arg;

    // lock buffer for writting
    pthread_mutex_lock(&mutex);

    // continue lock for duration of test
    for(int i = 0; i < TEST_COUNT; i++){

        // if buffer is full, sleep and notify looging(consumer thread)
        while(b->is_full == 1 && !done_testing){ 
            printf("Temprature sensor %d wurde schlafengelegt!\n",id);
            pthread_cond_wait(&p_cond, &mutex);
            printf("Temprature sensor %d wurde aufgeweckt!\n", id);
        }
        // fill buffer while we can
        while(enqueue(b,(rand() % 50)) && !done_testing);
        sleep(SLEEP_SEC);
        // signal when done filling
        pthread_cond_signal(&c_cond);
    }
    // we are done testing at this stage
    // so we unlock mutex and broadcast incase any thread is waiting.
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&c_cond);
    // also set done testing flag to one
    // because loggin program(consumer) has to know that no more production(measurement) will go on
    done_testing=1;
    return NULL;
}

int main()
{
    // initialise buffer of size 7
    b = init(7);
    // init condition for consumer
    pthread_cond_init(&c_cond,NULL);
    // init condition for producers
    pthread_cond_init(&p_cond,NULL);
    // init mutex
    pthread_mutex_init(&mutex,NULL);
    
    // threads
    pthread_t consumer,producer1, producer2;
    // we could get exact thread Id's using pthread_self() but that doesnt help.
    int temprature = 0, one = 1, two = 2;
    // create threads
    if(pthread_create(&producer1,NULL,&produce,&one) ||
    pthread_create(&producer2,NULL,&produce,&two)||
    pthread_create(&consumer,NULL,&consume,&temprature)){
        fprintf(stderr,"Error creating one of the threads\n");
        cleanUp();
    }

    // wait and join
    if(!pthread_join(consumer,NULL))
        printf("Logging program done\n");
    if(!pthread_join(producer1,NULL))
        printf("Temprature Sensor 1 done\n");
    if(!pthread_join(producer2,NULL))
        printf("Temprature Sensor 2 done\n");

    // clean up buffer, conditions and mutex
    cleanUp();
    return 0;
}