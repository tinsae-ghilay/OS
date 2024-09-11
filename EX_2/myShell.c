//
// Created by tgk on 9/11/24.
//

# include <signal.h>
# include <stdio.h>
# include <string.h>
//# include <sys/types.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>


int main(int argc, char* argv[])
{
    // endless loop because we will keep our program open untill user enters 'exit'
    while(1){
        // first prompt
        printf("> ");

        // start reading user input
        char cmd[1024];
        if(fgets(cmd,1024, stdin) == NULL) {
            continue;
        }

        // append a null terminator to command line argument
        cmd[strlen(cmd)-1] = '\0';

        // see if user wants to exit
        if(strcmp(cmd, "exit") == 0 ||  strcmp(cmd, "EXIT") == 0 ) {
            // user wants to exit
            printf("myShell has been closed, Good bye!");
            break;
        }

        // lets try to execute the command

        // first lets fork to start a new proccess
        pid_t pid = fork();

        // lets check proccess
        // If successful, fork() returns 0 to the child process and the process ID of the newly created child to the parent process.
        // If unsuccessful, fork() fails to create a child process, returns -1 to the parent, and sets errno to one of the following values:
        // [source](https://www.ibm.com/docs/en/zos/2.5.0?topic=functions-fork-create-new-process)

        if(pid <= -1) {  // means error in starting new proccess

            perror("Error in starting new Proccess during fork() call");

        }else if(pid == 0) { // child proccess succesfuly started

            // lets parse cmd int to an array of command arguments

            int i = 0;
            char* commands[512];
            char* token = strtok(cmd, " ");
            while(token != NULL && i < 511) {
                commands[i] = token;
                token = strtok(NULL," ");
                i++;
            }
            // terminate commands
            commands[i] = NULL;

            // so we execute command
            execv(commands[0],commands);
            // here program should have executed command and start over. as in continue
            // if not, it means we have error
            perror(" Error executing command by calling execv(argv[0], argv)");
            exit(1);

        }else {

            // if proccess is parent proccess, we tell it to wait. unless we enter "exit". parrent proccess should wait for child proccess to finish
            waitpid(pid,0,0);

        }

    }
}