/*
 * mine.c
 *
 * Created on Thu Oct 17 2024
 *
 * created by Kahsay Tinsae Ghilay
 * Matrikelnummer: ********
 *
 * Course tittle Operating Systems
 *
 *
 *
 *
 *
 */
#define _XOPEN_SOURCE   600
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>
# include <signal.h>
# include <string.h>
# include <time.h>

# define CAPACITY 10
# define WORKERS 6
# define REPEATS 10



typedef struct{
    int is_full;
    int container[CAPACITY];
    int current_index;
}thread_data_t;

thread_data_t data = {0,{0},0};
int task_end_flag = 0, itterations = 0;
pthread_mutex_t mutex;
pthread_cond_t producer_c, consumer_c; 


// signal handler
void registerShutdownHandler(void* fnc) {

    // creating a sigaction struct
    struct sigaction my_signal;
    // if a signal is active, diactivate all other signals
	my_signal.sa_handler = fnc;
	sigemptyset (&my_signal.sa_mask);
    // let the system handle other signals except SIGTERM and SIGINT
	sigaddset(&my_signal.sa_mask, SIGINT);
	sigaddset(&my_signal.sa_mask, SIGALRM);

    // if a system call is interrupted by a signal
    // it should end up where it left off
	my_signal.sa_flags = SA_RESTART;
    // register signal handler for SIGINT as that is what we need. 
	if (sigaction(SIGINT, &my_signal, NULL) != 0) {
		perror("Fehler beim Registrieren des Signal-Handlers");
		exit(1);
	}

	// Register SIGALRM handler as well
    if (sigaction(SIGALRM, &my_signal, NULL) != 0) {
        perror("Fehler beim Registrieren des Signal-Handlers");
        exit(1);
    }
}

// clean up to destroy mutex and conditions
void clean_up()
{
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&consumer_c);
    pthread_cond_destroy(&producer_c);
}

// sig action handler
void sig_handle(int signum){
    
    if(signum == SIGALRM){ // signal is an alarm
        // we check if we have reached maximum itterations
        // if yes, we set task_end _flag to true and we end program that way
        // else we increment iteration counter
        (itterations == REPEATS)? task_end_flag = 1: itterations ++;
        return;
    }
    
    // SIGINT or other signal received?
    if(signum != SIGINT){
        // I am not interested in other signals
        return;
    }
    char buff[64];

    pthread_mutex_lock(&mutex); // we hold lock and stop other threads from doing anything.
    // we prompt user is indeed wants to exit
    printf("\nYou really want to quite? [y/n] ");
    while(fgets(buff, 64,stdin) != NULL){
        // does user reall want to exit?
        if(strncmp(buff, "y", 1) == 0){
            // set task end to true
            task_end_flag = 1;
            // and brodcast or signal to all threads that may be waiting
            pthread_cond_signal(&consumer_c);
            pthread_cond_broadcast(&producer_c);
            pthread_mutex_unlock(&mutex);
            // and clean up
            clean_up();
            // the we exit parent process normally
            printf("> Good bye!\n");
            exit(EXIT_SUCCESS);
        }
        // if user pressed n the we break the loop and get back to whatever we were doing
        if(strncmp(buff, "n", 1) == 0){
            // and brodcast or signal to all threads that may be waiting
            pthread_cond_signal(&consumer_c);
            pthread_cond_broadcast(&producer_c);
            pthread_mutex_unlock(&mutex);
            return;
        }else{
            printf("please choose y to end program or n to continue : ");
            continue;
        }
    }
}

// producer function
void *produce(void *arg)
{
    int id = *(int*)arg;

    // while not end we will keep repeating this task
    while(!task_end_flag){

        // lets try lock
        pthread_mutex_lock(&mutex);

        // after having lock, check if we can produce
        if(data.is_full){ // nothing to produce

            // producer should wait
            printf("producer %d sleeps\n",id);
            pthread_cond_wait(&producer_c,&mutex);
            // when we are notified by producer_c, producer wakes up
            printf("producer %d received signal and woke up\n",id);
        }
        // if data is not full, we produce one
        data.container[data.current_index] = 1;

        // update current index (slot)
        data.current_index++;
        // to simulate worker doing some thing 
        // we sleep a random amount of time
        srand(time(NULL));
        usleep(rand() % 30000);
        printf("Producer %d added a product\n",id);

        // data might be full when we add.
        if(data.current_index == CAPACITY){
            data.is_full = 1;
        }

        // we have done our work 
        // so we have to signal either to other producers or consumer
        // depending on how full our container is
        (data.is_full)? pthread_cond_signal(&consumer_c): pthread_cond_signal(&producer_c);
        // we free lock
        pthread_mutex_unlock(&mutex);
        // and we are simulating workder doing some thing elese
        // by sleeping for random amount of time
        srand(time(NULL));
        usleep(rand() % 30000);
    }

    // if task_end_flag is set we finish
    // since consumer might be waiting on this thread signaling it
    // we broadcast (or signal since we only have one consumer in this case)
    pthread_cond_signal(&consumer_c);
    pthread_mutex_unlock(&mutex);
    free(arg);
    // there is nothing to return from this function
    return NULL;
}

void *consume(void *arg)
{
    // while not end we will keep repeating this task
    while(!task_end_flag){

        // lets try lock
        pthread_mutex_lock(&mutex);
        // after having lock, check if we can produce
        while(!data.is_full){ // nothing to produce
            // producer should wait
            printf("Consumer sleeping\n");
            pthread_cond_wait(&consumer_c,&mutex);
            // when we are notified by producer_c, producer wakes up
            printf("Consumer received signal and woke up\n");
        }
        // we clear the whole shelf
        printf("Consumer consuming .");
        for(int i = 0; i < CAPACITY; i++){
            // just to make things interesting, we will show ...... while consuming, so we flush stdout
            fflush(stdout);
            // lets also assume consumption takes time
            usleep(100000);
            printf(" .");
            data.container[i] = 0;
        }
        printf("done!\n");
        // we have cleared data. so we set index to 0
        data.current_index = 0;
        // and set data as empty
        data.is_full = 0;
        // we have done our work so we have to signal producers
        pthread_cond_signal(&producer_c);
        // we free lock
        pthread_mutex_unlock(&mutex);
        // we may do other activities before we get back again, 
        // so imitate this by sleeping for random aount of time
        srand(time(NULL));
        usleep((rand() % 30000)); // then we repeat
    }
    // if task_end_flag is set we finish
    // since producers might be waiting on this thread signaling
    // we broadcast 
    pthread_cond_broadcast(&producer_c);
    pthread_mutex_unlock(&mutex);
    // there is nothing to return from this function
    return NULL;
}

// main function
int main()
{
    // alarm trigger
    // signal handler @see sharedmemory.c / sharedmemory.h
    registerShutdownHandler(&sig_handle);

    // init mutex
    if(pthread_mutex_init(&mutex,NULL) != 0){
        perror("Error init mutex");
        exit(EXIT_FAILURE);
    }
    // init conditions
    if(pthread_cond_init(&consumer_c, NULL) != 0 ||
    pthread_cond_init(&producer_c, NULL) != 0){
        perror("Error init conditions");
        pthread_mutex_destroy(&mutex);
        exit(EXIT_FAILURE);
    }

    // lets now declare thread ids for consumer and producer
    pthread_t consumer;
    pthread_t producers[WORKERS];
    // now lets create threads

    for(int i = 0; i < WORKERS; i++){
        // this will be our thread identifier
        int *id = malloc(sizeof(int));
        *id = i;
        // creating threads
        if(pthread_create(&producers[i], NULL, &produce, id) != 0){ // error creating threads
            // if thread creation didnt succeed, we have to clean up
            free(id);
            clean_up();
            perror("Error creating threads");
            exit(EXIT_FAILURE); 
        }
    }
    // consumer thread.
    if(pthread_create(&consumer,NULL,&consume, NULL) != 0){
        clean_up();
        perror("Error consumer thread");
        exit(EXIT_FAILURE);
    }
    while(!task_end_flag){
        // alarm every 3 seconds
        alarm(3);
        pause();
    }
    // we wait for threads to join
    if(pthread_join(consumer,NULL)!= 0){ // error in joining
        perror("Error Joining consumer thread");
        clean_up();
        exit(EXIT_FAILURE); 
    }
    // wait for worker threads to join
    for(int i = 0; i < WORKERS; i++){
        if(pthread_join(producers[i], NULL) != 0){ // error in joining
            clean_up();
            perror("Error joining threads");
            exit(EXIT_FAILURE); 
        }
    }

    // clean up when we are done
    clean_up();
    return EXIT_SUCCESS;

}