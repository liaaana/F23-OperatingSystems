#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t childOne, childTwo;
    clock_t parentTime, childOneTime, childTwoTime;

    parentTime = clock();

    childOne = fork();
    childOneTime = clock();
    if (childOne == 0) {
        printf("Child process 1 with PID: %d and PPID: %d and EXECUTION TIME: %f\n", getpid(), getppid(), ((double)(clock() - childOneTime)) / CLOCKS_PER_SEC * 1000);
        return 0;
    } else {
        childTwo = fork();
        childTwoTime = clock();
        if (childTwo == 0) {
            printf("Child process 2 with PID: %d, PPID: %d and EXECUTION TIME: %f\n", getpid(), getppid(), (double)(clock() - childTwoTime) / CLOCKS_PER_SEC * 1000);
            return 0;
        } else {
            printf("Parent process with PID: %d, PPID: %d and EXECUTION TIME: %f\n", getpid(), getppid(), (double)(clock() - parentTime) / CLOCKS_PER_SEC * 1000);
        }
    }
    wait(NULL);
    wait(NULL);
    return 0;
}

