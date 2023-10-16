#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>


#define FILE_LENGTH 100

int agentPID;

void handler(int signum) {
    kill(agentPID, SIGTERM);
    exit(EXIT_SUCCESS);
}

int main() {
    signal(SIGINT, handler);
    FILE *agentPIDFile = fopen("/var/run/agent.pid", "r");
    if (agentPIDFile == NULL) {
        perror("Error: No agent found.\n");
        exit(EXIT_FAILURE);
    }

    char data[FILE_LENGTH];
    fgets(data, FILE_LENGTH, agentPIDFile);
    sscanf(data, "%d", &agentPID);
    fclose(agentPIDFile);


    if (kill(agentPID, 0) == -1) {
        printf("Error: No agent found.\n");
        return EXIT_FAILURE;
    }

    printf("Agent found.\n");

    while (1) {
        printf("Choose a command {\"read\", \"exit\", \"stop\", \"continue\"} to send to the agent: ");
        char command[20];
        scanf("%s", command);
        if (strcmp(command, "read") == 0) {
            kill(agentPID, SIGUSR1);
        } else if (strcmp(command, "exit") == 0) {
            kill(agentPID, SIGUSR2);
            return EXIT_SUCCESS;
        } else if (strcmp(command, "stop") == 0) {
            kill(agentPID, SIGSTOP);
        } else if (strcmp(command, "continue") == 0) {
            kill(agentPID, SIGCONT);
        } else {
            printf("Invalid command.\n");
        }
    }

    return 0;
}