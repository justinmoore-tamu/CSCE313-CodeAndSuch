/****************
LE2: Introduction to Unnamed Pipes
****************/
#include <unistd.h> // pipe, fork, dup2, execvp, close

#include <stdio.h> // cout
#include <stdlib.h> // exit
// using namespace std;

int main () {
    // lists all the files in the root directory in the long format
    char* cmd1[] = {(char*) "ls", (char*) "-al", (char*) "/", nullptr};
    // translates all input from lowercase to uppercase
    char* cmd2[] = {(char*) "tr", (char*) "a-z", (char*) "A-Z", nullptr};

    // TODO: add functionality
    // Create pipe
    int pipefds[2];  // [1] is write end, [0] is read end
    pid_t pid;  // return number from creating the pipe

    // make the pipe
    if (pipe(pipefds) == -1) { // -1 is falure, 0 is success
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create child to run first command
    pid = fork();
    // In child, redirect output to write end of pipe            **** 0: stdin, 1:stdout, 2:stderr
    if (pid == 0) { // in child
        dup2(pipefds[1], 1);
        // dup2(1, pipefds[1]);    
        // Close the read end of the pipe on the child side.
        close(pipefds[0]);
        // In child, execute the command
        execvp(cmd1[0], cmd1);
    } else { // in parent of first fork
            // Create another child to run second command
            pid = fork();
            // In child, redirect input to the read end of the pipe
            if (pid == 0) { // in the child of this fork
                dup2(pipefds[0], 0);
                // dup2(0, pipefds[0]);
                // Close the write end of the pipe on the child side.
                close(pipefds[1]);
                // Execute the second command.
                execvp(cmd2[0],cmd2);
            }
        // Reset the input and output file descriptors of the parent.
        // not changed so not do
    }

    // this changes nothing so idk if needed
    // close(pipefds[1]);
    // close(pipefds[0]);

}
