#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>

typedef struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
} PTE;

PTE *page_table;
int num_frames;
int num_pages;
int num_disk_accesses;
int pageTableFile;
char **ram;
char **disk;

void print_page_table() {
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
               i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);
    }
    printf("\n");
}

void print_ram() {
    for (int i = 0; i < num_frames; i++) {
        printf("Frame %d: %s\n", i, ram[i]);
    }
}

void print_disk() {
    for (int i = 0; i < num_pages; i++) {
        printf("Page %d: %s\n", i, disk[i]);
    }
}

void handle_SIGUSR1(int signum) {
    int chosen_page = -1;

    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].referenced != 0) {
            chosen_page = i;
            break;
        }
    }

    if (chosen_page != -1) {
        pid_t mmuPID = page_table[chosen_page].referenced;
        printf("A disk access request from MMU Process (pid=%d)\n", mmuPID);
        printf("Page %d is referenced\n", chosen_page);
        int ff_index = -1;
        for (int j = 0; j < num_frames; j++) {
            if (strcmp(ram[j], "") == 0) {
                ff_index = j;
                break;
            }
        }
        if (ff_index == -1) {
            printf("We do not have free frames in RAM\n");
            int rf_index = rand() % num_frames;
            printf("Chose a random victim page %d for replacement\n", rf_index);
            printf("Replace/Evict it with page %d to be allocated to frame %d\n", chosen_page, rf_index);
            printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", chosen_page, rf_index);

            for (int i = 0; i < num_pages; i++) {
                if (page_table[i].frame == rf_index) {
                    if (page_table[i].dirty) {
                        printf("Victim frame %d is dirty. Writing to disk.\n", rf_index);
                        num_disk_accesses++;
                        strcpy(disk[i], ram[rf_index]);
                    }

                    page_table[i].valid = false;
                    page_table[i].frame = -1;
                    page_table[i].dirty = false;
                    page_table[i].referenced = 0;

                    strcpy(ram[rf_index], disk[chosen_page]);
                    page_table[chosen_page].valid = true;
                    page_table[chosen_page].frame = rf_index;
                    page_table[chosen_page].dirty = false;
                    page_table[chosen_page].referenced = 0;

                    printf("RAM array\n");
                    print_ram();
                    num_disk_accesses++;
                    printf("Disk accesses is %d\n\n", num_disk_accesses);

                    kill(mmuPID, SIGCONT);
                    printf("Resume MMU process\n-------------------------\n");
                    return;
                }
            }
        } else {
            printf("We can allocate it to free frame %d\n", ff_index);
            page_table[chosen_page].valid = true;
            page_table[chosen_page].frame = ff_index;
            page_table[chosen_page].dirty = false;
            page_table[chosen_page].referenced = 0;

            printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", chosen_page, ff_index);
            strcpy(ram[ff_index], disk[chosen_page]);

            printf("RAM array\n");
            print_ram();

            num_disk_accesses++;
            printf("Disk accesses is %d\n\n", num_disk_accesses);

            kill(mmuPID, SIGCONT);
            printf("Resume MMU process\n-------------------------\n");
            return;
        }
    } else {
        printf("No pages need loading. Pager terminating. Disk accesses: %d\n", num_disk_accesses);
        munmap(page_table, sizeof(PTE) * num_pages);
        shm_unlink("/tmp/ex2/pagetable");
        remove("/tmp/ex2/pagetable");
        close(pageTableFile);
        for (int i = 0; i < num_frames; i++) {
            free(ram[i]);
        }
        free(ram);
        for (int i = 0; i < num_pages; i++) {
            free(disk[i]);
        }
        free(disk);
        exit(EXIT_SUCCESS);
    }
}

char random_char() {
    char c;
    FILE *file = fopen("/dev/urandom", "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        fread(&c, sizeof(char), 1, file);
        if (isprint(c)) {
            fclose(file);
            return c;
        }
    }
}

void init_table_ram_disk() {
    pageTableFile = open("/tmp/ex2/pagetable", O_CREAT | O_RDWR);
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

    ram = (char **) malloc(num_frames * sizeof(char *));
    disk = (char **) malloc(num_pages * sizeof(char *));

    for (int i = 0; i < num_frames; i++) {
        ram[i] = (char *) malloc(9 * sizeof(char));
    }
    for (int i = 0; i < num_frames; i++) {
        strcpy(ram[i], "");
    }

    for (int i = 0; i < num_pages; i++) {
        disk[i] = (char *) malloc(9 * sizeof(char));
        for (int j = 0; j < 8; j++) {
            disk[i][j] = random_char();
        }
        disk[i][8] = '\0';
    }

    for (int i = 0; i < num_pages; i++) {
        page_table[i].valid = false;
        page_table[i].frame = -1;
        page_table[i].dirty = false;
        page_table[i].referenced = 0;
    }

    printf("-------------------------\n");
    printf("Initialized page table\n");
    print_page_table();
    printf("-------------------------\n");

    printf("-------------------------\n");
    printf("Initialized RAM\n");
    printf("RAM array\n");
    print_ram();
    printf("-------------------------\n");

    printf("-------------------------\n");
    printf("Initialized disk\n");
    printf("Disk array\n");
    print_disk();
    printf("-------------------------\n");
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("ERROR!\nPlease provide:\n"
               "- number of pages\n"
               "- number of frames\n");
        return EXIT_FAILURE;
    }
    num_pages = atoi(argv[1]);
    num_frames = atoi(argv[2]);

    init_table_ram_disk();
    signal(SIGUSR1, handle_SIGUSR1);

    while (1) {
        sleep(1);
    }
    return 0;
}
