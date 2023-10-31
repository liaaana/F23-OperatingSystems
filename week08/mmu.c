#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_REFERENCE_STRING_ELEMENT 5

typedef struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
} PTE;

struct PTE *page_table;
int num_pages;
pid_t pagerPID;
int pageTableFile;
int need_pause = 0;

void handle_sigcont(int signum) {
    need_pause = 0;
    printf("MMU resumed by SIGCONT signal from pager\n");
}

void print_page_table() {
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
               i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);
    }
    printf("\n");
}

void initialize_page_table() {
    while (access("/tmp/ex2/pagetable", F_OK) == -1) {
        usleep(100000);
    }

    pageTableFile = open("/tmp/ex2/pagetable", O_RDWR);
    if (pageTableFile == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    off_t pageSize = num_pages * sizeof(PTE);
    if (ftruncate(pageTableFile, pageSize) == -1) {
        perror("ftruncate");
        close(pageTableFile);
        exit(EXIT_FAILURE);
    }

    page_table = mmap(NULL, pageSize, PROT_READ | PROT_WRITE, MAP_SHARED, pageTableFile, 0);
    if (page_table == MAP_FAILED) {
        perror("mmap");
        close(pageTableFile);
        exit(EXIT_FAILURE);
    }

    printf("-------------------------\n");
    printf("Initialized page table:\n");
    print_page_table();
    printf("-------------------------\n");
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("ERROR!\nPlease provide:\n"
               "- number of pages in the process\n"
               "- reference string of memory accesses\n"
               "- PID of the pager process");
        return EXIT_FAILURE;
    }

    signal(SIGCONT, handle_sigcont);

    num_pages = atoi(argv[1]);
    pagerPID = atoi(argv[argc - 1]);
    int reference_string_length = argc - 3;
    char reference_string[reference_string_length][MAX_REFERENCE_STRING_ELEMENT];

    for (int i = 2; i < argc - 1; i++) {
        strcpy(reference_string[i - 2], argv[i]);
    }

    initialize_page_table();

    for (int item = 0; item < reference_string_length; item++) {
        printf("-------------------------\n");
        char type = reference_string[item][0];
        int page = atoi(reference_string[item] + 1);
        if (type == 'W') {
            printf("Write Request for page %d\n", page);
        } else if (type == 'R') {
            printf("Read Request for page %d\n", page);
        }

        if (page_table[page].valid) {
            printf("It is a valid page\n");
        } else {
            printf("It is not a valid page --> page fault\n"
                   "Ask pager to load it from disk (SIGUSR1 signal) and wait\n");
            page_table[page].referenced = getpid();
            need_pause = 1;
            kill(pagerPID, SIGUSR1);
            while (need_pause == 1) {
                sleep(1);
            }
        }

        if (type == 'W') {
            printf("It is a write request, setting the dirty field\n");
            page_table[page].dirty = true;
        }

        printf("Updated page table:\n");
        print_page_table();
    }

    printf("Done all requests.\n");
    printf("MMU sends SIGUSR1 to the pager.\n");
    kill(pagerPID, SIGUSR1);

    printf("MMU terminates.\n");
    munmap(page_table, sizeof(struct PTE) * num_pages);
    close(pageTableFile);
    return 0;
}
