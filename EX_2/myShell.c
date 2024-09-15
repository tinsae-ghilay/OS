//
// Created by tgk on 9/11/24.
//

# include <signal.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>


int main()
{
    // endless loop because we will keep our program open untill user enters 'exit'
    while(1){
        // first prompt
        printf("> ");

        // start reading user input
        char cmd[128];
        if(fgets(cmd,128, stdin) == NULL) {
            continue;
        }

        // append a null terminator to command line argument
        cmd[strlen(cmd)-1] = '\0';

        // see if user wants to exit
        if(strcmp(cmd, "exit") == 0 ||  strcmp(cmd, "EXIT") == 0 ) {
            // user wants to exit
            printf("myShell has been closed, Good bye!\n");
            break;
        }

        // first lets fork to start a new proccess
        pid_t pid = fork();

        // lets check proccess
        // If successful, fork() returns 0 to the child process and the process ID of the newly created child to the parent process.
        // If unsuccessful, fork() fails to create a child process, returns -1 to the parent, and sets errno to one of the following values:
        // [source](https://www.ibm.com/docs/en/zos/2.5.0?topic=functions-fork-create-new-process)

        if(pid <= -1) {  // means error in starting new proccess

            perror("Error in starting new Proccess during fork() call");

        }else if(pid == 0) { // child proccess succesfuly started

            // lets parse cmd in to an array of command arguments
            char* commands[8];

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
                // we could also just check if file extension is .sh with strstr(stack, needle) and I tried it, it works. but what if it doesnt have a file extension?
                if(line[0] == '#' && line[1] == '!') {
                    // we have a script file.
                    printf("SheBang! this is a script file and executing it results in the following\n");

                    // adding bash to system call.
                    commands[0] = "/bin/bash";
                    // since we set first item of commands, we increment index
                    i++;
                }else { // its not a script file
                    printf("SheDang! this is not a script file and executing it results in the following\n");
                }
            }
            // closing the file as we have no need for it.
            fclose(f);

            // adding the rest of the commands to the array of commands
            // remember, if it is a script, commands[0] is already set and i = 1 here, if not i = 0, and items will be set accordingly
            while(token != NULL && i < 8) {
                commands[i] = token;
                token = strtok(NULL," ");
                i++;
            }
            // terminate commands
            commands[i] = NULL;
            // and we execute
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