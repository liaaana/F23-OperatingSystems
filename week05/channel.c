#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 1024

int main() {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        printf("ERROR: Pipe creation failed\n");
        return EXIT_FAILURE;
    }

    pid_t PID = fork();
    if (PID == -1) {
        printf("ERROR: Fork failed\n");
        return EXIT_FAILURE;
    }
    if (PID == 0) {
        close(pipe_fd[1]);

        char message[MAX_MESSAGE_SIZE];
        size_t b_read;

        while ((b_read = read(pipe_fd[0], message, MAX_MESSAGE_SIZE)) > 0) {
            printf("Subscriber prints the message: %.*s", (int)b_read, message);
        }


        close(pipe_fd[0]);
    } else {
        close(pipe_fd[0]);
        char message[MAX_MESSAGE_SIZE];

        printf("Enter a message (\"end\" to quit):\n");
        while (fgets(message, MAX_MESSAGE_SIZE, stdin) != NULL) {
            if (strncmp("end", message, 3) == 0){
                break;
            }
            write(pipe_fd[1], message, strlen(message));
        }
        close(pipe_fd[1]);
        wait(NULL);
    }
    return 0;
}
