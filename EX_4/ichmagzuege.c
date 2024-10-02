//
// Created by tgk on 9/28/24.
//

# include <pthread.h>
# include <semaphore.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <errno.h>

// test with 10 trains
#define THREAD_COUNT 20

// Train as struct for thread
typedef struct Train{
     char name[20];
     int duration;
} Train;

// semaphore
sem_t semaphore;

// parameter train
// train (thread with the train data) will try to gain access untill it is allowed to pass
void * threadFunc(void* args){

    // train struct
    Train *t = args;
    // because we will use while loop, this flag will prevent from repeated print statements
    int informed = 0; 
    // try to access lock untill access is granted(semaphore value is not less than 0)
    /** according to [this](https://man7.org/linux/man-pages/man3/sem_trywait.3p.html)
     * The sem_trywait() function shall lock the semaphore referenced by
     * sem only if the semaphore is currently not locked; that is, if
     * the semaphore value is currently positive. Otherwise, it shall
     * not lock the semaphore. if this is not the case, it will set errno == EAGAIN
     * which means we can also # include <errno.h> and check if(errno == EAGAIN) and know thread is waiting
    */
    while(!sem_trywait(&semaphore)){ 
        if(!informed){ // if we have not informed once (first try_wait())
            printf("Fahrterlaubnis für %s verweigert und muss auf ein freies Gleis warten!\n",t->name);
            // set flag so we dont repeate this
            informed = 1;
            sleep(t->duration);
        }
    }
    // if we reach here, it means we gained access and thread can execute
    printf("Fahrterlaubnis für %s wurde erteilt! \n", t->name);
    sleep(t->duration);
    // and post when done
    sem_post(&semaphore);

    // and free thread data
    free(t);
    return NULL;
}


int main()
{
    // lets init semaphore and check its success
    if(sem_init(&semaphore, 0, 3)){
        perror("Error initiating Semaphore");

    }

    // creating threads
    pthread_t threads[THREAD_COUNT];
    for(int i = 0; i < THREAD_COUNT; i++){

        // train data and malloc null safety
        Train *t = malloc(sizeof(Train));
        if(t == NULL){
            perror("Error allocating memory for Thread data");
            exit(1);
        }
        // setting train attribute (member) values
        sprintf(t->name,"ZN%d",i+1);
        t->duration = (rand()%3);
        usleep(rand()%700000);
        // create thread and null safety
        if(pthread_create(&threads[i], NULL, &threadFunc, t)){
            perror("Error creating threads");
            free(t);
            exit(1);
        }
    }
    // join threads
    for(int i = 0; i < THREAD_COUNT; i++){
        pthread_join(threads[i], NULL);
    }
    // destroy semaphore
    sem_destroy(&semaphore);
    return EXIT_SUCCESS;
}
