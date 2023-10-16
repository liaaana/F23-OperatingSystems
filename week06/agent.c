#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define FILE_LENGTH 100

void readTextFile() {
    printf("text.txt: \n");
    FILE *textFile = fopen("./text.txt", "r");
    if (textFile == NULL) {
        perror("Error opening text.txt");
        exit(EXIT_FAILURE);
    }

    char data[FILE_LENGTH];
    if (fgets(data, FILE_LENGTH, textFile) == NULL) {
        perror("Error reading text.txt");
        fclose(textFile);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", data);
    fclose(textFile);
}

void sigusr1_handler(int signum) {
    readTextFile();
}

void sigusr2_handler(int signum) {
    printf("Process terminating...\n");
    exit(0);
}


int main() {
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);

    FILE *agentPIDFile;
    agentPIDFile = fopen("/var/run/agent.pid", "w");

    if (agentPIDFile == NULL) {
        perror("Error opening the file");
        return EXIT_FAILURE;
    }

    fprintf(agentPIDFile, "%d", getpid());
    fclose(agentPIDFile);
    readTextFile();

    while (1) {
        sleep(1);
    }

    return 0;
}