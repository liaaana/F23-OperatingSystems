#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CELLS 10000000
unsigned int memory[MAX_CELLS];


void memoryInit() {
    for (int i = 0; i < MAX_CELLS; i++) {
        memory[i] = 0;
    }
}

void allocateFirstFit(int adrs, int size) {
    int foundSize = 0;
    int foundSizeIndex = -1;

    for (int i = 0; i < MAX_CELLS; i++) {
        if (memory[i] == 0) {
            if (foundSize == 0) {
                foundSizeIndex = i;
            }
            foundSize++;
            if (foundSize == size) {
                for (int j = foundSizeIndex; j < foundSizeIndex + size; j++) {
                    memory[j] = adrs;
                }
                return;
            }
        } else {
            foundSize = 0;
        }
    }
}


void allocateBestFit(int adrs, int size) {
    int foundSize = 0;
    int foundSizeIndex = -1;
    int bestFitSize = MAX_CELLS;
    int bestFitStart = -1;

    for (int i = 0; i < MAX_CELLS; i++) {
        if (memory[i] == 0) {
            if (foundSize == 0) {
                foundSizeIndex = i;
            }
            foundSize++;
        } else {
            if (foundSize >= size) {
                if (foundSize < bestFitSize) {
                    bestFitSize = foundSize;
                    bestFitStart = foundSizeIndex;
                }
            }
            foundSize = 0;
        }
    }

    if (bestFitStart != -1) {
        for (int i = bestFitStart; i < bestFitStart + size; i++) {
            memory[i] = adrs;
        }
    }
}


void allocateWorstFit(int adrs, int size) {
    int foundSize = 0;
    int foundSizeIndex = -1;
    int worstFitSize = 0;
    int worstFitStart = -1;

    for (int i = 0; i < MAX_CELLS; i++) {
        if (memory[i] == 0) {
            if (foundSize == 0) {
                foundSizeIndex = i;
            }
            foundSize++;
        } else {
            if (foundSize >= size) {
                if (foundSize > worstFitSize) {
                    worstFitSize = foundSize;
                    worstFitStart = foundSizeIndex;
                }
            }
            foundSize = 0;
        }
    }

    if (worstFitStart != -1) {
        for (int i = worstFitStart; i < worstFitStart + size; i++) {
            memory[i] = adrs;
        }
    }
}


void clear(int adrs) {
    for (int i = 0; i < MAX_CELLS; i++) {
        if (memory[i] == adrs) {
            memory[i] = 0;
        }
    }
}


float calculateThroughput(clock_t start, clock_t end, int queryCount) {
    return (float) queryCount / ((float) (end - start) / CLOCKS_PER_SEC);
}

int main() {
    const char *allocationAlgorithms[] = {"First fit", "Best fit", "Worst fit"};
    void (*allocationFunctions[])(int, int) = {allocateFirstFit, allocateBestFit, allocateWorstFit};

    for (int algorithm = 0; algorithm < 3; algorithm++) {
        memoryInit();
        FILE *inputFile = fopen("queries.txt", "r");
        if (inputFile == NULL) {
            printf("ERROR: Could not open the input file.\n");
            return EXIT_FAILURE;
        }

        clock_t start, end;
        int adrs;
        int size;
        int queryCount = 0;
        start = clock();
        while (1) {
            char queryType[20];
            if (fscanf(inputFile, "%s", queryType) == EOF || strcmp(queryType, "end") == 0) {
                break;
            }
            if (strcmp(queryType, "allocate") == 0) {
                fscanf(inputFile, "%d %d", &adrs, &size);
                allocationFunctions[algorithm](adrs, size);
            } else if (strcmp(queryType, "clear") == 0) {
                fscanf(inputFile, "%d", &adrs);
                clear(adrs);
            }
            queryCount++;
        }
        end = clock();
        printf("%s throughput: %f queries/second\n", allocationAlgorithms[algorithm],
               calculateThroughput(start, end, queryCount));
        fclose(inputFile);
    }
    return 0;
}