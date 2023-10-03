#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 8


int main() {
    char inputCommandLine[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];

    while (1) {
        printf("Enter a command to execute. If you want to finish enter \"finish\": ");
        if (fgets(inputCommandLine, MAX_COMMAND_LENGTH, stdin) == NULL) {
            printf("ERROR: Impossible to read inputCommandLine\n");
            exit(EXIT_FAILURE);
        }
        inputCommandLine[strcspn(inputCommandLine, "\n")] = '\0';
        if (strcmp(inputCommandLine, "finish") == 0) {
            break;
        }

        char *inputLinePart = strtok(inputCommandLine, " ");
        int i = 0;
        while (inputLinePart != NULL) {
            args[i++] = inputLinePart;
            inputLinePart = strtok(NULL, " ");
        }
        args[i] = NULL;
        pid_t processPID = fork();

        if (processPID == 0) {
            if (execvp(args[0], args) == -1) {
                perror("ERROR: Impossible to run command by execvp\n");
                exit(EXIT_FAILURE);
            }
        } else if (processPID < 0) {
            perror("ERROR: Fork failed\n");
            exit(EXIT_FAILURE);
        } else {
            int status;
            waitpid(processPID, &status, 0);
        }
    }

    return 0;
}
