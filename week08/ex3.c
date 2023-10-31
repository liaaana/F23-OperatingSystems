#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

int main() {
    struct rusage usage;
    void *ptr[10];
    printf("%-8s\t%-20s\t%-18s\t%-18s\n", "Time (s)", "Memory Usage (KB)", "Minor Page Faults", "Major Page Faults");

    for (int i = 0; i < 10; i++) {
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            printf("%-8d\t%-20ld\t%-18ld\t%-18ld\n",
                   i,
                   usage.ru_maxrss,
                   usage.ru_minflt,
                   usage.ru_majflt);
        }
        ptr[i] = malloc(10 * 1024 * 1024);
        memset(ptr[i], 0, 10 * 1024 * 1024);
        sleep(1);
    }

    for (int i = 0; i < 10; i++) {
        free(ptr[i]);
    }

    return 0;
}
