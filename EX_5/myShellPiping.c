//
// Created by tgk on 9/11/24.
//

# define _XOPEN_SOURCE 700
# include <signal.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>

// declaring acceptable number of Proccesses that can be run
# define P_COUNT 5
// read end of pipe
# define READER 0
// write end of pipe
# define WRITER 1

int has_child_processes = 0;
int x = 0;
// array to hold proccess IDs for each fork() we call.
pid_t pids[P_COUNT];
// Signal handler responsible for gracefull shutdown
void graceful_shutdown_handler(int signum) {
    
    // kill the proccess
    if(has_child_processes > 1){ // we have child proccesse so we kill them
        for(int i = 1; i < x; i++){
            kill(pids[i],signum);
        }
        has_child_processes = 0;
        printf("\nChild proccesses killed\n");
    }else{ // we dont have child processes so we prompt
        printf("\nExit myShell? (y/n)");
        char buffer[128];
        fgets(buffer, 128, stdin);
        // jusr so we can be flexible 
        int yes = strncmp(buffer, "y", 1) == 0 || 
                    strncmp(buffer, "yes", 3) == 0 ||
                    strncmp(buffer, "Y", 1) == 0 ||
                    strncmp(buffer, "YES", 3) == 0;
        if (yes) {
            printf("myShellPiping has been closed, Good bye!\n");
            int pid = getpid();
            // will try this later
            kill(pid,signum);
            // or
            // raise(signum);
            //exit(EXIT_SUCCESS);
        }else{
            printf("OK. Staying\n");
        }
    }
}


// Signal handler
void handleSignal(){

    // declare signal action struct
    struct sigaction my_signal;
    // fill the structure
    my_signal.sa_handler = graceful_shutdown_handler;
    // if a signal is active, all signals have to be ignored
    sigemptyset (&my_signal.sa_mask);
    sigaddset(&my_signal.sa_mask, SIGTERM);
    sigaddset(&my_signal.sa_mask, SIGINT);
    sigaddset(&my_signal.sa_mask, SIGQUIT);
    // if a program is interrupted by signal. it should be restarted using SA_RESTART
    // SA_RESTART has an unexpected behaviour. 
    // we have to press enter multiple times before we get to needed program state
    my_signal.sa_flags = 0;//SA_RESTART; 

    // Register signal handler for SIGTERM, SIGINT and SIGQUIT
    if (sigaction(SIGTERM, &my_signal, NULL) != 0) {
        perror("Fehler beim Registrieren des Signal-Handlers");
        exit(1);
    }
    if (sigaction(SIGINT, &my_signal, NULL) != 0) {
        perror("Fehler beim Registrieren des Signal-Handlers");
        exit(1);
    }
    if (sigaction(SIGQUIT, &my_signal, NULL) != 0) {
        perror("Fehler beim Registrieren des Signal-Handlers");
        exit(1);
    }

}


void executeCommand(char* cmd){
    
    // lets parse cmd in to an array of command arguments
    char* cmds[8];

    // first part of command before space
    char* token = strtok(cmd, " ");

    // if we must have a script file identification, then
    // lets check if its a file first by trying to open it using the first word as a path
    FILE* f = fopen(token,"r");

    // index of commands
    int i = 0;

    // if this is a file
    if(f != NULL) {
        // is it a script file?
        char line[128];
        if(fgets(line,128,f)==NULL) {// getting the first line

            printf("Sorry, first line turned out to be empty: bad syntax cannot run this file");
            exit(1);
        }

        // if it has SheBang the it is a script file
        // we could also just check if file extension is .sh with strstr(stack, needle).
        // I tried it, it works. but what if it doesnt have a file extension?

        if(line[0] == '#' && line[1] == '!') { // we have a script file.

            // adding bash to system call.
            cmds[0] = "/bin/bash";
            // since we set first item of commands, we increment index
            i++;
        }
    }
    // closing the file as we have no need for it.
    fclose(f);

    // adding the rest of the commands to the array of commands
    // remember, if it is a script, commands[0] is already set and i = 1 here, if not i = 0, and items will be set accordingly
    while(token != NULL && i < 8) {
        cmds[i] = token;
        token = strtok(NULL," ");
        i++;
    }

    // terminate commands
    cmds[i] = NULL;
    
    // and we execute
    execv(cmds[0],cmds);
    // here program should have executed command and start over. as in continue
    // if not, it means we have error
    perror(" Error executing command by calling execv(argv[0], argv)");
    exit(EXIT_FAILURE);

}

int main()
{
    // endless loop because we will keep our program open untill user enters 'exit'
    while(1){
        // set value of x to 0
        x = 0;
        handleSignal();
        // first prompt
        printf("> ");

        // start reading user input
        char cmd[1024];
        // if we have no input we go back
        if(fgets(cmd,1024, stdin) == NULL) {
            continue;
        }

        // append a null terminator to command line argument
        cmd[strlen(cmd)-1] = '\0';

        // check if string is empty. we go back
        if(cmd[0]=='\0'){
            continue;
        }

        // see if user wants to exit
        if(strcmp(cmd, "exit") == 0 ||  strcmp(cmd, "EXIT") == 0 ) {
            // user wants to exit?
            //graceful_shutdown_handler(SIGTERM);
            printf("Exit requested\n");
            break;
            // incase user decides not to close, we restart
            //continue;
        }
        // I am limmiting cammands to include / as there needs to be a full path
        // that usualy includes back-slash eg. /usr/bin or ./script.sh
        // but since we need exit command. this is intentionaly put here after we check for exit command
        // I am not going to do more here.
        if(strstr(cmd,"/") == NULL){
            printf("Invalid command Please use commands with full path e.g /bin/make or ./script.sh\n");
            continue;
        }

        // We need an array to separate commands split by Pipe symbol
        // I set all elements to NULL for no reason other than safety
        char *pipe_cmds[P_COUNT] = {NULL};
        // we should devide command based on the pipe Symbol (|) using strtok()
        char* tkn = strtok(cmd,"|");
        pipe_cmds[0] = tkn;
        x++;

        // pids so we can wait for them.
        while(tkn != NULL && x < P_COUNT-1) {
            pipe_cmds[x] = tkn;
            tkn = strtok(NULL,"|");
            x++;
        }

        // file descriptors for the pipes between commands.
        // every pipe has two file descriptors write and read ends in each pipe
        int pipefd[x][2];
        // proccess forking
        //int i = 0;
        for(int i = 0; i < x; i++){
            // pipes and proccess
            // create pipes for the proccess and check for error during creation
            if(pipe(pipefd[i])){
                perror("Error in creating Pipe");
            }
            // fork a proccess that will execute a command
            pids[i] = fork();
            // since we called for, we know we have a child proccess.
            has_child_processes++;
            if(pids[i] <= -1){
                perror("Error in starting new Proccess during fork() call");
            }

            if(pids[i] == 0){ 
                // child proccess
                // close write end of previous pipe
                if (i > 0) { // this is not the first in the pipeline
                
                    close(pipefd[i - 1][WRITER]);
                    // redirect input (STDIN) to Read end of previous Pipe
                    dup2(pipefd[i - 1][READER], STDIN_FILENO);
                }
                // close read end of Next pipe
                if (i < x - 1) { // this is not the last in in the pipeline
                    close(pipefd[i][READER]);
                    // redirect output (STDOUT) to Write end  of next pipe
                    dup2(pipefd[i][WRITER], STDOUT_FILENO);
                }

                // command execution
                executeCommand(pipe_cmds[i]);

            }else{ // else we are in parent proccess
                
                if (i < x - 1) {
                    // we close the write end of this proccess
                    close(pipefd[i][WRITER]);
                }
            }
            
        }
        // we wait for all proccesses to finish
        for(int i = 0; i < x; i++){
            wait(NULL);
        }
    }
    return EXIT_SUCCESS;

}