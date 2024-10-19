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

// helps with POSIX compatibility
// broader than _POSIX_C_SOURCE
# define _XOPEN_SOURCE   600
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>
# include <signal.h>
# include <string.h>
# include <time.h>
#include <sys/time.h>
// capacity of container in structure
# define CAPACITY 10
// number of workers(hunters)
# define WORKERS 6
// how much time (* ALARM seconds our program runs)
# define REPEATS 10
// alarm interval in seconds
#define ALARM_INTERVAL 3


// structure to hold our data
typedef struct{
    int is_full;
    int container[CAPACITY];
    int current_index;
}thread_data_t;

// declaring data structure
thread_data_t data = {0,{0},0};

// flag to tell our program to end
int task_end_flag = 0;
// program pause
int pause_flag = 0;

// itteration count -> may be we dont want it
int itterations = 0;

// Mutex
pthread_mutex_t mutex;

// merchant and hunter conditions
pthread_cond_t hunter_c, merchant_c; 

// signal handler
// registers signals 
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
		exit(EXIT_FAILURE);
	}

	// Register SIGALRM handler as well
    if (sigaction(SIGALRM, &my_signal, NULL) != 0) {
        perror("Fehler beim Registrieren des Signal-Handlers");
        exit(EXIT_FAILURE);
    }
}

// clean up mutex and conditions
void clean_up()
{
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&merchant_c);
    pthread_cond_destroy(&hunter_c);
}

// this is to un pause program from pseudo pause induced by a signal
// since program is running in a continous loop, we need to to hold mutex lock to stop this loop
// after that we call this to release lock and broadcast to all threads
void unpause()
{
    // reset pause flag
    pause_flag = 0;
    // release mutex and broadcast conditions for all
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&merchant_c);
    pthread_cond_broadcast(&hunter_c);
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
    // try to tell threads to pause(sleep) and wait for signal
    // this is done in hunter and merchant thread functions
    pause_flag = 1;
    // we hold lock and stop other threads from doing anything.
    pthread_mutex_lock(&mutex);
    // we prompt user is indeed wants to exit
    printf("\nYou really want to quit? [y/n]");
    while(fgets(buff, 64,stdin) != NULL){
        // does user reall want to exit?
        if(strncmp(buff, "y", 1) == 0){
            // set task end to true
            task_end_flag = 1;
            // unpause program and let it end // clean up after itself
            unpause();
            // we have set up program to exit normaly
            // so we return
            return;
        }
        // if user pressed n the we break the loop and get back to whatever we were doing
        if(strncmp(buff, "n", 1) == 0){
            // unpause program and let it end // clean up after itself
            unpause();
            return;
        }else{
            // we ask user to provide the correct response again
            printf("please choose y to end program or n to continue : ");
            continue;
        }
    }
}

// hunter function
void *pick(void *arg)
{
    int id = *(int*)arg;
    // we free arg as we dont need it
    free(arg);
    // while not end we will keep repeating this task
    while(1){
        // lets try lock
        pthread_mutex_lock(&mutex);

        // after having lock, check if we can pick
        // nothing to pick or pause flag is activated  thread sleeps
        if(data.is_full || pause_flag ){ 
            // sleep until condition signal received
            pthread_cond_wait(&hunter_c,&mutex);
        }
        if(task_end_flag){
                break;
        }
        
        // if data is not full, we pick one
        data.container[data.current_index] = 1;

        // to simulate worker doing some thing 
        // we sleep a random amount of time
        srand(time(NULL));
        usleep(rand() % 30000);
        data.current_index++;
        printf("hunter %d picked a coin\n",id);

        // data might be full when we add.
        if(data.current_index == CAPACITY){
            data.is_full = 1;
        }

        // we have done our work 
        // so we have to signal either to other hunters or merchant
        // depending on how full our container is
        (data.is_full)? pthread_cond_signal(&merchant_c): pthread_cond_signal(&hunter_c);
        // we free lock
        pthread_mutex_unlock(&mutex);
        // and we are simulating workder doing some thing elese
        // by sleeping for random amount of time
        srand(time(NULL));
        usleep(rand() % 30000);
    }

    // if task_end_flag is set we finish
    // since merchant might be waiting on this thread signaling it
    // we broadcast (or signal since we only have one merchant in this case)
    pthread_cond_broadcast(&merchant_c);
    pthread_mutex_unlock(&mutex);
    // there is nothing to return from this function
    return NULL;
}

void *shake(void *arg)
{
    // while not end we will keep repeating this task
    while(1){

        // lets try lock
        pthread_mutex_lock(&mutex);
        // after having lock, check if we can pick
        // if data is empty or pause flag is activated  thread sleeps
        if(!data.is_full || pause_flag){
            pthread_cond_wait(&merchant_c,&mutex);
            printf("Merchant is filling %d pebels int to hi bag\n", CAPACITY);
        }
        // if task ended, we exit loop
        if(task_end_flag){
                break;
        }
        // we clear the whole shelf
        printf("merchant consuming .");
        int i = 0;
        while(i < CAPACITY){
            if(pause_flag){ // pause-> exit loop
                break;
            }
            // just to make things interesting, we will show ...... while consuming, so we flush stdout
            fflush(stdout);
            // lets also assume consumption takes time
            usleep(100000);
            printf(" .");
            data.container[i] = 0;
            i++;
        }
        // if exited loop because of pause , or because done
        (pause_flag)?printf("paused\n") : printf("done!\n");
        // we have cleared data. so we set index to 0
        data.current_index = 0;
        // and set data as empty
        data.is_full = 0;
        // we have done our work so we have to signal hunters
        pthread_cond_signal(&hunter_c);
        // we free lock
        pthread_mutex_unlock(&mutex);
        // we may do other activities before we get back again, 
        // so imitate this by sleeping for random aount of time
        srand(time(NULL));
        if(task_end_flag){
            break;
        }
        usleep((rand() % 30000)); // then we repeat
    }
    // if task_end_flag is set we finish
    // since hunters might be waiting on this thread signaling
    // we broadcast 
    pthread_cond_broadcast(&hunter_c);
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
    if(pthread_cond_init(&merchant_c, NULL) != 0 ||
    pthread_cond_init(&hunter_c, NULL) != 0){
        perror("Error init conditions");
        pthread_mutex_destroy(&mutex);
        exit(EXIT_FAILURE);
    }

    // lets now declare thread ids for merchant and hunter
    pthread_t merchant;
    pthread_t hunters[WORKERS];
    // now lets create threads

    for(int i = 0; i < WORKERS; i++){
        // this will be our thread identifier
        int *id = malloc(sizeof(int));
        *id = i;
        // creating threads
        if(pthread_create(&hunters[i], NULL, &pick, id) != 0){ // error creating threads
            // if thread creation didnt succeed, we have to clean up
            free(id);
            clean_up();
            perror("Error creating threads");
            exit(EXIT_FAILURE); 
        }
    }
    // merchant thread.
    if(pthread_create(&merchant,NULL,&shake, NULL) != 0){
        clean_up();
        perror("Error merchant thread");
        exit(EXIT_FAILURE);
    }
    while(!task_end_flag){
        // alarm every 3 seconds
        alarm(3);
        pause();
    }
    // we wait for threads to join
    if(pthread_join(merchant,NULL)!= 0){ // error in joining
        perror("Error Joining merchant thread");
        clean_up();
        exit(EXIT_FAILURE); 
    }
    // wait for worker threads to join
    for(int i = 0; i < WORKERS; i++){
        if(pthread_join(hunters[i], NULL) != 0){ // error in joining
            clean_up();
            perror("Error joining threads");
            exit(EXIT_FAILURE); 
        }
    }
    if(data.current_index > 0){
        printf("cleaning shop");
    }
    while(data.current_index > 0){
        // clear array
        data.container[data.current_index] = 0;
        fflush(stdout);
        printf(" .");
        data.current_index--;
    }
    printf(" shop is empty\n");
    // clean up when we are done
    clean_up();
    printf("> Good bye!\n");
    return EXIT_SUCCESS;

}