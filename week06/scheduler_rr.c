#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

#define PS_MAX 10

// holds the scheduling data of one process
typedef struct {
    int idx; // process idx (index)
    int at, bt, rt, wt, ct, tat; // arrival time, burst time, response time, waiting time, completion time, turnaround time.
    int burst; // remaining burst (this should decrement when the process is being executed)
    int rq;
} ProcessData;

// the idx of the running process
int running_process = -1; // -1 means no running processes

// the total time of the timer
unsigned total_time; // should increment one second at a time by the scheduler

// data of the processes
ProcessData data[PS_MAX]; // array of process data

// array of all process pids
pid_t ps[PS_MAX]; // zero valued pids - means the process is terminated or not created yet

int quantum;
int processes_queue[PS_MAX];
unsigned processes_queue_size;

unsigned data_size;

void read_file(FILE *file) {
    int idx, at, bt;
    data_size = 0;
    processes_queue_size = 0;
    memset(ps, 0, sizeof(ps));
    memset(processes_queue, -1, sizeof(processes_queue));

    fscanf(file, "%*s %*s %*s");
    while (fscanf(file, "%d %d %d", &idx, &at, &bt) != EOF) {
        if (data_size < PS_MAX) {
            data[data_size].idx = idx;
            data[data_size].at = at;
            data[data_size].bt = bt;
            data[data_size].rt = -1;
            data[data_size].wt = 0;
            data[data_size].ct = 0;
            data[data_size].tat = 0;
            data[data_size].burst = bt;
            data_size++;
        } else {
            printf("ERROR: Maximum number of processes reached\n");
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
}


void resume(pid_t process) {
    if (kill(process, 0) != -1) {
        kill(process, SIGCONT);
    }
}


void suspend(pid_t process) {
    if (kill(process, 0) != -1) {
        kill(process, SIGTSTP);
    }
}

void terminate(pid_t process) {
    if (kill(process, 0) != -1) {
        kill(process, SIGTERM);
    }
}


void create_process(int new_process) {
    // stopping the running process omitted because we call this function only in schedule_handler if there is no running process
    pid_t child_pid = fork();
    if (child_pid == 0) {
        char process_index[10];
        sprintf(process_index, "%d", new_process);
        char *args[] = {"./worker", process_index, NULL};
        execvp(args[0],
               args); // scheduler runs the program "./worker {new_process}" using one of the exec functions like execvp
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (child_pid > 0) {
        ps[new_process] = child_pid; // added child_pid to ps array
        running_process = new_process; // now the idx of the running process is new_process
        data[new_process].rt = total_time - data[new_process].at;
    } else {
        perror("ERROR: fork failed\n");
        exit(EXIT_FAILURE);
    }
}

// find next process for running
ProcessData find_next_process() {
    // adding new process to queue
    for (int i = 0; i < data_size; i++) {
        if (data[i].at == total_time && data[i].burst > 0) {
            int in_queue = 0;
            for (int j = 0; j < processes_queue_size; j++) {
                if (processes_queue[j] == data[i].idx) {
                    in_queue = 1;
                    break;
                }
            }
            if (in_queue == 0) {
                processes_queue[processes_queue_size] = i;
                processes_queue_size++;
            }
        }
    }

    // location of next process in {data} array
    int location = -1;
    if (processes_queue_size > 0) {
        // location of the next process to run from the {data} array
        location = processes_queue[0];
        // shift queue
        for (int i = 0; i < processes_queue_size - 1; i++) {
            processes_queue[i] = processes_queue[i + 1];
        }
        processes_queue_size--;
    }
    // if we didn't find process to run we recursively call this function after incrementing total_time
    if (location == -1) {
        for (int i = 0; i < data_size; i++) {
            if (data[i].burst > 0) {
                location = i;
                break;
            }
        }
        for (int i = 0; i < data_size; i++) {
            if (data[i].at < data[location].at && data[i].burst > 0) {
                location = i;
            }
        }
        printf("Scheduler: Runtime: %u seconds.\nProcess %d: has not arrived yet.\n", total_time, location);
        total_time++;
        return find_next_process();
    } else {
        // return the data of next process
        return data[location];
    }
}


// reports the metrics and simulation results
void report() {
    printf("Simulation results.....\n");
    int sum_wt = 0;
    int sum_tat = 0;
    for (int i = 0; i < data_size; i++) {
        printf("process %d: \n", i);
        printf("	at=%d\n", data[i].at);
        printf("	bt=%d\n", data[i].bt);
        printf("	ct=%d\n", data[i].ct);
        printf("	wt=%d\n", data[i].wt);
        printf("	tat=%d\n", data[i].tat);
        printf("	rt=%d\n", data[i].rt);
        sum_wt += data[i].wt;
        sum_tat += data[i].tat;
    }

    printf("data size = %d\n", data_size);
    float avg_wt = (float) sum_wt / data_size;
    float avg_tat = (float) sum_tat / data_size;
    printf("Average results for this run:\n");
    printf("	avg_wt=%f\n", avg_wt);
    printf("	avg_tat=%f\n", avg_tat);
}

void check_burst() {
    for (int i = 0; i < data_size; i++)
        if (data[i].burst > 0)
            return;

    // report simulation results
    report();

    // terminate the scheduler :)
    exit(EXIT_SUCCESS);
}


// This function is called every one second as handler for SIGALRM signal
void schedule_handler(int signum) {
    // Increment the total time
    total_time++;
    // adding new process to queue
    for (int i = 0; i < data_size; i++) {
        if (data[i].at == total_time && data[i].burst > 0) {
            int in_queue = 0;
            for (int j = 0; j < processes_queue_size; j++) {
                if (processes_queue[j] == data[i].idx) {
                    in_queue = 1;
                    break;
                }
            }
            if (in_queue == 0) {
                processes_queue[processes_queue_size] = i;
                processes_queue_size++;
            }
        }
    }
    // 1. If there is a worker process running, then decrement its remaining burst and print messages
    if (running_process != -1) {
        data[running_process].rq--;
        data[running_process].burst--;
        printf("Scheduler: Runtime: %u seconds\n", total_time);
        printf("Process %d is running with %d seconds left\n", running_process, data[running_process].burst);

        // 1.A. If the worker process finished its burst time, then the scheduler terminate the running process and prints the message
        if (data[running_process].burst == 0) {
            terminate(ps[running_process]);
            printf("Scheduler: Terminating Process %d (Remaining Time: 0)\n", running_process);
            waitpid(ps[running_process], NULL, 0);
            data[running_process].ct = total_time;
            data[running_process].tat = data[running_process].ct - data[running_process].at;
            data[running_process].wt = data[running_process].tat - data[running_process].bt;
            running_process = -1;
        }
        // If the worker process finished its remaining quantum time, then the scheduler stop the running process and prints the message
        if (running_process != -1 && data[running_process].rq == 0) {
            processes_queue[processes_queue_size] = data[running_process].idx;
            processes_queue_size++;

            printf("Scheduler: Stopping Process %d (Remaining Time: %d)\n", running_process,
                   data[running_process].burst);
            suspend(ps[running_process]);
            running_process = -1;
        }
        check_burst();
    } else { // 2. After that, we need to find the next process to run
        // this call will check the bursts of all processes
        check_burst();
        ProcessData next_process = find_next_process();
        running_process = next_process.idx; // 3.B. set current process as the running process.
        if (ps[running_process] == 0) {
            // 3.C.1. then create a new process for {running_process} and print the message
            data[running_process].rq = quantum;
            create_process(running_process);
            printf("Scheduler: Starting Process %d (Remaining Time: %d)\n", running_process,
                   data[running_process].burst);
        } else {
            // 3.C.2. or resume the process {running_process} if it is stopped and print the message
            data[running_process].rq = quantum;
            resume(ps[running_process]);
            printf("Scheduler: Resuming Process %d (Remaining Time: %d)\n", running_process,
                   data[running_process].burst);
        }
        // 3 and 3.A omitted because we checked and stopped running process earlier
    }
}

int main(int argc, char *argv[]) {
    // read the data file
    FILE *in_file = fopen(argv[1], "r");
    if (in_file == NULL) {
        printf("File is not found or cannot open it!\n");
        exit(EXIT_FAILURE);
    } else {
        read_file(in_file);
    }

    printf("Enter quantum: ");
    scanf("%d", &quantum);

    // set a timer
    struct itimerval timer;

    // the timer goes off 1 second after reset
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    // timer increments 1 second at a time
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    // this counts down and sends SIGALRM to the scheduler process after expiration.
    setitimer(ITIMER_REAL, &timer, NULL);

    // register the handler for SIGALRM signal
    signal(SIGALRM, schedule_handler);

    // Wait till all processes finish
    while (1); // infinite loop
}