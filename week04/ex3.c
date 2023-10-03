#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *args[]){
    int n = atoi(args[1]);
    
    for (int i = 0; i < n; i++) {
        pid_t processPID = fork();
        sleep(5);
    }

    return 0;
}
