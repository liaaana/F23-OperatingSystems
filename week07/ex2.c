#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>


int main() {
    long pageSize = sysconf(_SC_PAGESIZE);
    long chunkSize = 1024 * pageSize;
    FILE *randomFile = fopen("/dev/random", "r");
    FILE *textFile = fopen("text.txt", "w");
    if (textFile == NULL) {
        perror("ERROR: Could not open file text.txt.\n");
        return EXIT_FAILURE;
    }
    int ch;
    unsigned long long currentSize = 0;
    int characterCounter = 0;

    while (currentSize < 500 * 1024 * 1024) {
        ch = fgetc(randomFile);
        if (isprint(ch)) {
            fputc(ch, textFile);
            currentSize++;
            characterCounter++;
            if (characterCounter == 1024) {
                fputc('\n', textFile);
                characterCounter = 0;
            }
        }
    }
    fclose(textFile);
    fclose(randomFile);

    int textFileDescriptor = open("text.txt", O_RDWR);
    if (textFileDescriptor == -1) {
        perror("ERROR: Could not open file text.txt for memory mapping.\n");
        return EXIT_FAILURE;
    }
    off_t fileSize = 500 * 1024 * 1024 - 1;

    long capitalLettersCounter = 0;
    for (off_t i = 0; i < fileSize; i += chunkSize) {
        long chunkBegin = i;
        long chunkEnd = i + chunkSize;
        if (chunkEnd > fileSize) {
            chunkEnd = fileSize;
        }
        char *data = mmap(NULL, chunkEnd - chunkBegin, PROT_READ | PROT_WRITE, MAP_SHARED, textFileDescriptor, chunkBegin);
        if (data == MAP_FAILED) {
            perror("ERROR: mmap failed.\n");
            return EXIT_FAILURE;
        }
        int capitalLettersCounterChunk = 0;
        for (int j = 0; j < chunkEnd - chunkBegin; j++) {
            if (isupper(data[j])) {
                capitalLettersCounterChunk++;
                data[j] = tolower(data[j]);
            }
        }
        capitalLettersCounter += capitalLettersCounterChunk;
        munmap(data, chunkEnd - chunkBegin);
    }
    printf("Number of capital letters is: %lu\n", capitalLettersCounter);
    close(textFileDescriptor);
    return 0;
}
