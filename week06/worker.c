#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

// 6 digits for big triangular numbers like 113050
#define TRI_BASE 1000000

// current process pid (which executed this program)
pid_t pid;

// current process idx (starts from 0)
int process_idx;

// number of triangular numbers found so far
long tris;

bool is_triangular(long n) {
    for (long i = 1; i <= n; i++) {
        if (i * (i + 1) == 2 * n) {
            return true;
        }
    }
    return false;
}

long big_n() {
    time_t t;
    long n = 0;
    srand((unsigned) time(&t));

    while (n < TRI_BASE)
        n += rand();

    return n % TRI_BASE;
}

void signal_handler(int signum) {
    printf("Process %d (PID=<%d>): count of triangulars found so far is \e[0;31m%ld\e[0m\n", process_idx, pid, tris);

    switch (signum) {
        case SIGTSTP:
            printf("Process %d: stopping....\n", process_idx);
            pause();
            break;
        case SIGCONT:
            printf("Process %d: resuming....\n", process_idx);
            break;
        case SIGTERM:
            printf("Process %d: terminating....\n", process_idx);
            exit(EXIT_SUCCESS);
        default:
            break;
    }
}

int main(int argc, char *argv[]) {
    pid = getpid();
    process_idx = 0;
    tris = 0;

    // get the process_idx from argv
    process_idx = atoi(argv[1]);

    // register the signals
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCONT, signal_handler);
    signal(SIGTERM, signal_handler);

    long next_n = big_n() + 1;
    printf("Process %d (PID=<%d>): has been started\n", process_idx, pid);
    printf("Process %d (PID=<%d>): will find the next triangular number from [%ld, inf)\n", process_idx, pid, next_n);

    tris = 0;
    // in an infinite loop, search for next triangular numbers starting from {next_n}
    while (1) {
        if (is_triangular(next_n)) {
            printf("Process %d (PID=%d): I found this triangular number %lu\n", process_idx, pid, next_n);
            tris++;
        }
        next_n++;
    }

    return 0;
}
