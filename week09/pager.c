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
    int counter_nfu;
    unsigned char counter_aging;
} PTE;

PTE *page_table;
int num_frames;
int num_pages;
int num_disk_accesses;
int pageTableFile;
char **ram;
char **disk;
char *replacement_algorithm;

int (*replacement_function)();


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

int my_random() {
    int random_frame = rand() % num_frames;
    int random_page = -1;
    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].valid && page_table[i].frame == random_frame) {
            random_page = i;
            break;
        }
    }
    printf("Chose a victim page %d for replacement by random\n", random_page);
    return random_page;
};

int nfu() {
    int min_nfu = page_table[0].counter_nfu;
    int min_nfu_page;

    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].valid) {
            min_nfu = page_table[i].counter_nfu;
            min_nfu_page = i;
            break;
        }
    }

    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].counter_nfu < min_nfu && page_table[i].valid) {
            min_nfu = page_table[i].counter_nfu;
            min_nfu_page = i;
        }
    }
    printf("Chose a victim page %d for replacement by NFU\n", min_nfu_page);
    return min_nfu_page;
}


int aging() {
    int min_aging;
    int min_aging_page;

    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].valid == true) {
            min_aging = page_table[i].counter_aging;
            min_aging_page = i;
            break;
        }
    }


    for (int i = 0; i < num_pages; i++) {
        if (page_table[i].counter_aging < min_aging && page_table[i].valid) {
            min_aging = page_table[i].counter_aging;
            min_aging_page = i;
        }
    }

    printf("Chose a victim page %d for replacement by aging\n", min_aging_page);
    return min_aging_page;
}


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
            int evict_page_index = replacement_function();
            printf("Replace/Evict it with page %d to be allocated to frame %d\n", chosen_page,
                   page_table[evict_page_index].frame);
            if (page_table[evict_page_index].dirty) {
                printf("Victim frame %d is dirty. Writing to disk.\n", page_table[evict_page_index].frame);
                num_disk_accesses++;
                strcpy(disk[evict_page_index], ram[page_table[evict_page_index].frame]);
            }

            printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", chosen_page,
                   page_table[evict_page_index].frame);
            strcpy(ram[page_table[evict_page_index].frame], disk[chosen_page]);
            page_table[chosen_page].valid = true;
            page_table[chosen_page].frame = page_table[evict_page_index].frame;
            page_table[chosen_page].dirty = false;
            page_table[chosen_page].referenced = 0;

            page_table[evict_page_index].valid = false;
            page_table[evict_page_index].frame = -1;
            page_table[evict_page_index].dirty = false;
            page_table[evict_page_index].referenced = 0;

            printf("RAM array\n");
            print_ram();
            num_disk_accesses++;
            printf("Disk accesses is %d\n\n", num_disk_accesses);

            kill(mmuPID, SIGCONT);
            printf("Resume MMU process\n-------------------------\n");
            replacement_helper(chosen_page);
            return;
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
            replacement_helper(chosen_page);
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
        page_table[i].counter_aging = 0;
        page_table[i].counter_nfu = 1;
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
    printf("%d", getpid());
    if (argc != 4) {
        printf("ERROR!\nPlease provide:\n"
               "- number of pages\n"
               "- number of frames\n"
               "- replacement algorithm (random/nfu/aging)\n");
        return EXIT_FAILURE;
    }

    num_pages = atoi(argv[1]);
    num_frames = atoi(argv[2]);
    replacement_algorithm = argv[3];

    if (strcmp(replacement_algorithm, "random") == 0) {
        replacement_function = my_random;
    } else if (strcmp(replacement_algorithm, "nfu") == 0) {
        replacement_function = nfu;
    } else if (strcmp(replacement_algorithm, "aging") == 0) {
        replacement_function = aging;
    } else {
        printf("Invalid replacement algorithm specified: %s\n", replacement_algorithm);
        return EXIT_FAILURE;
    }

    printf("Selected replacement algorithm: %s\n", replacement_algorithm);


    init_table_ram_disk();
    signal(SIGUSR1, handle_SIGUSR1);


    while (1) {
        sleep(1);
    }
    return 0;
}
