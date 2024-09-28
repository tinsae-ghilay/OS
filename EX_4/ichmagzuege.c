//
// Created by tgk on 9/28/24.
//

# include <pthread.h>
# include <semaphore.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
#include <sys/types.h>

// test with 10 trains
#define THREAD_COUNT 10

// Train as data for thread
typedef struct Train{
     char name[20];
     int duration;
} Train;

// semaphore
sem_t semaphore;

// parameter train
// train (thread with the train data) will try to gain access untill it is allowed to pass
void * threadFunc(void* args){

    Train *t = args;
    // if train has passed. we end our request for passage.
    int passed = 0;
    while(1){ // try to access lock untill access is granted(semaphore value is not less than 0
        if(passed){ // if train has passed(thread has lock once, this is set to 1. we end our request for passage.
            break;
        }
        if(sem_trywait(&semaphore) != 0){ // we ask again if passage is not given
            printf("Fahrterlaubnis für %s verweigert und muss auf ein freies Gleis warten!\n",t->name);
            sleep(1);
        }else{ // we have lock. we do our thing and set passed to 1, so that we exit loop.
            printf("Fahrterlaubnis für %s wurde erteilt! \n", t->name);
            sleep(t->duration);
            passed = 1;
            // and we unlock (signal)
            sem_post(&semaphore);
        }
    }
    // and free thread data
    free(t);
    return NULL;
}


int main()
{
    // lets init thread and check its success
    if(sem_init(&semaphore, 0, 3)){
        perror("Error initiating Semaphore");

    }

    // creating threads
    int *ids = malloc(THREAD_COUNT * sizeof(int));
    for(int i = 0; i < THREAD_COUNT; i++){
        ids[i] = i+1;
    }
    pthread_t threads[THREAD_COUNT];
    for(int i = 0; i < THREAD_COUNT; i++){
        Train *t = malloc(sizeof(Train));
        sprintf(t->name,"Zug nummer %d",i+1);
        t->duration = (rand()%3)+1;
        if(pthread_create(&threads[i], NULL, &threadFunc, t)){
            perror("Error creating threads");
            exit(1);
        }
    }
    // join threads
    for(int i = 0; i < THREAD_COUNT; i++){
        pthread_join(threads[i], NULL);
    }
    sem_destroy(&semaphore);
    free(ids);
    return EXIT_SUCCESS;
}
