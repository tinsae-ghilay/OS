/*
 * bankingsystem.c
 *
 * Created on Sun Oct 13 2024
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
# include "sharedmemory.h"
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/wait.h>
# include <string.h>
# include <signal.h>

# define ACCOUNTS_SIZE 12
# define CREDIT 5000


/*

Legen Sie ein Shared-Memory Segment (ob Sie System-V oder POSIX Shared-Memory dafür verwenden, bleibt Ihnen überlassen) an. 
Wenn schon ein Shared-Memory Segment mit demselben Namen existiert, 
dann soll eine entsprechende Fehlermeldung auf stderr ausgegeben und das Programm beendet werden. 
In diesem Shared-Memory Segment soll das Array mit den Konten abgelegt werden.
*/

// account structure -> holds amount and credit status
typedef struct account{
    int amount;
    int credit;

}account;

// data to be stored in shared memory
// includes mutex and account
typedef struct sh_attr{
    account acounts[ACCOUNTS_SIZE][sizeof(account)];
    pthread_mutex_t *mutex;
}sh_attr;

// child process pids
pid_t pids[5];
int server;

// shared mem attribute(data)
sh_attr *shared_data;


/*
    Schreiben Sie eine Funktion clientAccessAccount(), 
    die eine bestimme Geldsumme (wird als Parameter übergeben) von einem bestimmten Konto 
    (wird auch als Parameter übergeben) abbucht oder hinzubucht (davon abhängig, 
    ob die Geldsumme positiv oder negativ ist).
*/
// critical region syncronised using Mutex
// changes amount (stored in shared memory)
void clientAccessAccount(int acct, int amount)
{
    // critical region
    //printf("> gettin to lock\n");
    pthread_mutex_lock(shared_data->mutex);
    //printf("> got lock lock\n");
    // once we have lock we change value
    shared_data->acounts[acct]->amount+= amount;
    srand(time(NULL));
    //usleep(rand() % 1000);
    //printf("Done work on child process\n");
    // and we unlock
    pthread_mutex_unlock(shared_data->mutex);

}

/*
    Schreiben Sie eine Funktion serverReadAccounts(), 
    die den derzeitigen Kontostand aller Konten auf der Konsole ausgibt. 
    Wenn ein Kontostand negativ ist, soll diese Funktion dem Konto automatisch 
    einen Kredit gewähren (die Kreditsumme darf hardgecoded werden). 
    Für jedes Konto soll die gesamte Kreditsumme (Summe aller individuellen Kredite) 
    im Shared-Memory Segment abgespeichert werden.
*/

// critical region
// reads data stored in shared memory
// updates credit (stored in shared memory)
// uses mutex for synchronisation
void serverReadAccounts()
{
    for(int i = 0; i < ACCOUNTS_SIZE; i++){

        // mutex lock on single account
        pthread_mutex_lock(shared_data->mutex);
        // if amount is below 0
        // add some credit and update amount
        if(shared_data->acounts[i]->amount < 0){
            shared_data->acounts[i]->amount+= CREDIT;
            shared_data->acounts[i]->credit+= CREDIT;
        }
        // unlock mutex, we only need it when we write
        // after this we only do reading.
        pthread_mutex_unlock(shared_data->mutex);
        if((i+1) % 3 == 0){
            printf("\t %d :\t %d \n",i,shared_data->acounts[i]->amount);
        }else{
            printf("\t  %d : %d ",i,shared_data->acounts[i]->amount);
        }
    }
}
/*
    Sorgen Sie dafür dass die Funktionen clientAccessAccount() 
    und serverReadAccounts() synchroniziert sind, 
    sodass nur jeweils ein Prozess zu jedem Zeitpunkt auf ein Konto zugreifen kann. 
    Verwenden Sie dafür Mutexe.
*/
/*
    Verwenden Sie alarm(3), um dafür zu sorgen, 
    dass die Funktion serverReadAccounts() alle 3 Sekunden ausgeführt wird.
*/

void sig_handle(int signum){
    
    if(signum == SIGALRM){ // signal is an alarm
        // we print account status
        printf("Accounts balances : {\n");
        serverReadAccounts();
        printf(" }\n");
        return;
    }
    // SIGTERM or other signal received?
    char buff[64];
    // we prompt user is indeed wants to exit
    printf("\nYou really want to quite? [y/n] ");
    fgets(buff, 64,stdin);
    // does user reall want to exit?
    if(strncmp(buff, "y", 1) == 0){
        // first we kill all child processes
        for(int i = 0; i < 5; i++){
            // in killing all processes, we are losing the chance to free some memory
            // and we cannot unlink shared memory too.
            // so shared memory persists, ane we have "already exists " error whn we restart program
            // atleast I think that is sthe reason.
            // but I dont know any other way.
            kill(pids[i],signum);
        }
    }
    //unmapShared(&server,sizeof(sh_attr));
    unlinkShared();
    
    // the we exit parent process normally
    printf("> Good bye!\n");
    exit(EXIT_SUCCESS);
}

/*
    Zum Testen soll mithilfe von fork(3) mindestens vier Kindprozesse erzeugt werden, 
    die alle in einer Endlos-Schleife die Funktion clientAccessAccount() ausführen. 
    Auf was für ein Konto welche Geldsumme abgebucht oder hinzugebucht wird, 
    soll mit Zufallszahlen gesteuert werden.

*/
int main(){

    // shared data created here
    // error handled in source
    server = createSharedForWrite();

    // truncating shared memory segment
    configureMem(server, sizeof(sh_attr));

    // mapping shared data segment
    sh_attr *content = mapSegment(server,sizeof(shared_data));

    // Allocate memory for the mutex
    content->mutex = malloc(sizeof(pthread_mutex_t));

    // map shared memory
    shared_data = content;
    // set account initial values to 0
    for(int i = 0; i < ACCOUNTS_SIZE; i++){
        shared_data->acounts[i]->amount = 0;
    }
    
    // init mutex and handling error
    if(pthread_mutex_init(shared_data->mutex, NULL)!= 0){
        free(shared_data->mutex);
        perror("Mutex initialisation");
    }
    printf("Program start...\n");

    for(int i = 0; i < 5; i++){
        // fork a process
        pids[i] = fork();

        if(pids[i] < 0){ // no process created?
            perror("Error creating a process");
            free(shared_data->mutex);
            exit(EXIT_FAILURE);
        }else if(pids[i] == 0){
            
            while(1){
                srand(time(NULL)^shared_data->acounts[i]->amount);
                int amount = (rand() % 100) - 50;
                int index= rand() % 12;
                clientAccessAccount(index,amount);
                usleep(rand()%10000);
            }
        }
    }
    // signal handler @see sharedmemory.c / sharedmemory.h
    registerShutdownHandler(&sig_handle);
    while(1){
        // alarm every 3 seconds
        alarm(3);
        pause();
    }
    
    for(int i = 0; i < 5; i++){
        waitpid(pids[i],NULL,0);
    }
    

    //free(shared_data->acounts);
    free(shared_data->mutex);
    pthread_mutex_destroy(shared_data->mutex);
    // unmap shared memory
    // @see sharedmemory.c / sharedmemory.h
    unmapShared(shared_data, sizeof(sh_attr));
    // unlink shared memory
    // @see sharedmemory.c / sharedmemory.h
    unlinkShared();
}