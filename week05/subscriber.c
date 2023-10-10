#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MAX_MESSAGE_SIZE 1024
#define MAX_PIPE_PATH 15

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("ERROR: Need only index of subscriber\n");
        return EXIT_FAILURE;
    }

    int subIndex = atoi(argv[1]);
    char pipePath[MAX_PIPE_PATH];
    snprintf(pipePath, sizeof(pipePath), "/tmp/ex1/s%d", subIndex);

    while (access(pipePath, F_OK) == -1) {
        sleep(1);
    }

    int pipeFD = open(pipePath, O_RDONLY);
    if (pipeFD == -1) {
        perror("ERROR: opening named pipe failed\n");
        return 1;
    }

    char message[MAX_MESSAGE_SIZE + 1];
    ssize_t readBytes;
    while ((readBytes = read(pipeFD, message, MAX_MESSAGE_SIZE)) > 0) {
        message[readBytes] = '\0';
        printf("Subscriber %d received: %s", subIndex, message);
    }
    close(pipeFD);
    return EXIT_SUCCESS;
}

