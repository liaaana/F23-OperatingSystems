#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>

typedef struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
    int counter_nfu;
    unsigned char counter_aging;
} PTE;

struct TLB_entry {
    int page;
    int frame;
};

struct PTE *page_table;
struct TLB_entry *tlb;
int tlb_size;
int num_pages;
pid_t pagerPID;
int pageTableFile;
int need_pause = 0;


void replacement_helper(int chosen_page) {
    page_table[chosen_page].counter_nfu++;
    for (int i = 0; i < num_pages; i++) {
        if (i == chosen_page) {
            page_table[i].counter_aging = (page_table[i].counter_aging >> 1) | (1 << 7);
        } else {
            page_table[i].counter_aging = (page_table[i].counter_aging >> 1) | (0 << 7);
        }
    }
}

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


void print_tlb() {
    for (int i = 0; i < tlb_size; i++) {
        printf("Page %d ---> frame=%d\n", tlb[i].page, tlb[i].frame);
    }
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


    tlb = (struct TLB_entry *) malloc(sizeof(struct TLB_entry) * tlb_size);
    for (int i = 0; i < tlb_size; i++) {
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }
    printf("-------------------------\n");
    printf("Initialized TLB\n");
    print_tlb();
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

    tlb_size = num_pages * 0.2;
    initialize_page_table();
    int tlb_hits = 0;
    int idx;
    for (int i = 2; i < argc - 1; i++) {
        printf("-------------------------\n");
        char type;
        int page;
        char *reference_string_item = argv[i];
        sscanf(reference_string_item, "%c%d", &type, &page);
        if (type == 'W') {
            printf("Write Request for page %d\n", page);
        } else if (type == 'R') {
            printf("Read Request for page %d\n", page);
        }

        bool tlb_hit = false;
        for (int j = 0; j < tlb_size; j++) {
            if (tlb[j].page == page) {
                tlb_hits++;
                for (int d = j; d > 0; d--) {
                    tlb[d] = tlb[d - 1];
                }
                tlb[0].page = page;
                tlb[0].frame = page_table[page].frame;
                tlb_hit = true;
                printf("TLB hit: Page %d found in TLB and frame=%d\n", page, tlb[j].frame);
                break;
            }
        }

        if (!tlb_hit) {
            printf("TLB miss: Page %d not found in TLB\n", page);
            if (page_table[page].valid) {
                replacement_helper(page);
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

            for (int d = tlb_size - 1; d > 0; d--) {
                tlb[d] = tlb[d - 1];
            }
            tlb[0].page = page;
            tlb[0].frame = page_table[page].frame;

            idx = -1;
            for (int d = 1; d < tlb_size; d++) {
                if (tlb[d].frame == tlb[0].frame) {
                    tlb[d].page = -2;
                    tlb[d].frame = -2;
                    idx = d;
                    break;
                }
            }
            if (idx != -1) {
                for (int d = idx; d < tlb_size - 1; d++) {
                    tlb[d] = tlb[d + 1];
                }
            }

            printf("Updated TLB:\n");
            print_tlb();
        }


        if (type == 'W') {
            printf("It is a write request, setting the dirty field\n");
            page_table[page].dirty = true;
        }

        printf("Updated page table:\n");
        print_page_table();
    }

    printf("Done all requests.\n");
    printf("TLB Miss Ratio: %.4f\n", 1 - ((float) tlb_hits / (float) (argc - 3)));

    printf("MMU sends SIGUSR1 to the pager.\n");
    kill(pagerPID,
         SIGUSR1);

    printf("MMU terminates.\n");
    munmap(page_table,
           sizeof(struct PTE) * num_pages);
    close(pageTableFile);
    return 0;
}
