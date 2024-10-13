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


typedef struct account{
    int amount;
    int credit;

}account;

typedef struct sh_attr{
    account acounts[ACCOUNTS_SIZE][sizeof(account)];
    pthread_mutex_t *mutex;
}sh_attr;

pid_t pids[5];

sh_attr *shared_data;

// parent pid
pid_t parent = 0;


/*

    Legen Sie ein Shared-Memory Segment (ob Sie System-V oder POSIX Shared-Memory dafür verwenden, bleibt Ihnen überlassen) an. 
    Wenn schon ein Shared-Memory Segment mit demselben Namen existiert, 
    dann soll eine entsprechende Fehlermeldung auf stderr ausgegeben und das Programm beendet werden. 
    In diesem Shared-Memory Segment soll das Array mit den Konten abgelegt werden.
*/
/*
    Schreiben Sie eine Funktion clientAccessAccount(), 
    die eine bestimme Geldsumme (wird als Parameter übergeben) von einem bestimmten Konto 
    (wird auch als Parameter übergeben) abbucht oder hinzubucht (davon abhängig, 
    ob die Geldsumme positiv oder negativ ist).
*/

void clientAccessAccount(account *account, int amount)
{
    //printf("> gettin to lock\n");
    pthread_mutex_lock(shared_data->mutex);
    //printf("> got lock lock\n");
    account->amount+= amount; // getting stuck here
    srand(time(NULL));
    usleep(rand() % 1000);
    //printf("Done work on child process\n");
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
void serverReadAccounts()
{

    for(int i = 0; i < ACCOUNTS_SIZE; i++){

        // mutex lock on single account
        pthread_mutex_lock(shared_data->mutex);
        if(shared_data->acounts[i]->amount < 0){
            shared_data->acounts[i]->amount+= CREDIT;
            shared_data->acounts[i]->credit+= CREDIT;
        }
        // unlock mutex
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
        printf("Accounts status : {\n");
        serverReadAccounts();
        printf(" }\n");
        return;
    }
    // SIGTERM or other signal received?
    char buff[64];
    // we prompt user is indeed wants to exit
    printf("\nYou really want to quite transactions? [y/n] ");
    fgets(buff, 64,stdin);
    // does user reall want to exit?
    if(strncmp(buff, "y", 1) == 0){
        // first we kill all child processes
        for(int i = 0; i < 5; i++){
            kill(pids[i],signum);
        }
    // the we exit parent process normally
    printf("> Good bye!\n");
    exit(EXIT_SUCCESS);
    }
}

/*
    Zum Testen soll mithilfe von fork(3) mindestens vier Kindprozesse erzeugt werden, 
    die alle in einer Endlos-Schleife die Funktion clientAccessAccount() ausführen. 
    Auf was für ein Konto welche Geldsumme abgebucht oder hinzugebucht wird, 
    soll mit Zufallszahlen gesteuert werden.


*/

void for_child(int i){

}
int main(){
    parent = getpid();

    // shared data created here
    // error handled in source
    int server = createSharedForWrite();

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
        perror("Mitex initialisation");
    }

    for(int i = 0; i < 5; i++){
        pids[i] = fork();

        /*int shm = createSharedForRead();
        void* adr = mapSegment(shm,sizeof(shared_data));*/
        // because we set the size of segment to our struct size when configuring
        //sh_attr *msg = (sh_attr*) adr;

        if(pids[i] < 0){
            perror("Error creating a process");
            exit(EXIT_FAILURE);
        }else if(pids[i] == 0){
            
            while(1){
                srand(time(NULL)^shared_data->acounts[i]->amount);
                int amount = (rand() % 100) - 50;
                int index= rand() % 12;
                clientAccessAccount(shared_data->acounts[index],amount);
                usleep(rand()%10000);
            }
        }
    }
    registerShutdownHandler(&sig_handle);
    // signal handler
    while(1){
        alarm(3);
        pause();
        //serverReadAccounts(shared_data->acounts);
    }
    
    for(int i = 0; i < 5; i++){
        waitpid(pids[i],NULL,0);
    }
    

    //free(shared_data->acounts);
    free(shared_data->mutex);
    pthread_mutex_destroy(shared_data->mutex);
    unmapShared(shared_data, sizeof(shared_data));
    unlinkShared();
}

/*
    Sorgen Sie bei allen Systemaufrufen für eine entsprechende 
    Fehlerbehandlung und geben Sie im Bedarfsfall eine Fehlermeldung auf stderr aus.

*/