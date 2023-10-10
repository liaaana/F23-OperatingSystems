#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>


#define MAX_MESSAGE_SIZE 1024
#define MAX_SUBSCRIBERS 4
#define MAX_PIPE_PATH 15


int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("ERROR: Need only number of subscribers\n");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int pipes[MAX_SUBSCRIBERS];

    for (int i = 0; i < n; ++i) {
        char pipe_name[MAX_PIPE_PATH];
        sprintf(pipe_name, "/tmp/ex1/s%d", i + 1);

        if (mkfifo(pipe_name, 0666) == -1 && errno != EEXIST) {
            perror("Error creating named pipe");
            exit(EXIT_FAILURE);
        }

        if ((pipes[i] = open(pipe_name, O_WRONLY)) == -1) {
            perror("Error opening named pipe for writing");
            exit(EXIT_FAILURE);
        }
    }
    char message[MAX_MESSAGE_SIZE];
    printf("Enter a message (\"end\" to quit):\n");
    while (fgets(message, MAX_MESSAGE_SIZE, stdin) != NULL) {

        if (strncmp("end", message, 3) == 0) {
            for (int i = 0; i < n; ++i) {
                close(pipes[i]);
            }
            break;
        }
        for (int i = 0; i < n; ++i) {
            pid_t pid = fork();

            if (pid == 0) {
                write(pipes[i], message, strlen(message));
                exit(EXIT_SUCCESS);
            } else if (pid < 0) {
                perror("Error forking process");
                exit(EXIT_FAILURE);
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        close(pipes[i]);
        wait(NULL);
    }
    return 0;
}


