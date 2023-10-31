#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_PASSWORD_LENGTH 13

int main() {
    pid_t pid = getpid();
    FILE *filePID = fopen("/tmp/ex1.pid", "w");
    fprintf(filePID, "%d\n", pid);
    fclose(filePID);

    FILE *randomFile = fopen("/dev/random", "r");
    char password[MAX_PASSWORD_LENGTH + 1];
    char ch;

    strncpy(password, "pass:", strlen("pass:"));
    int current_password_length = strlen("pass:");

    while (current_password_length < MAX_PASSWORD_LENGTH) {
        ch = fgetc(randomFile);
        if (isprint(ch)) {
            password[current_password_length] = ch;
            current_password_length++;
        }
    }
    fclose(randomFile);

    password[MAX_PASSWORD_LENGTH] = '\0';
    printf("%s\n", password);

    char *password_in_heap = (char *)mmap(NULL, sizeof(password), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (password_in_heap == MAP_FAILED) {
        perror("Error mapping shared memory");
        exit(EXIT_FAILURE);
    }
    strcpy(password_in_heap, password);

    while (1) {
        sleep(1);
    }

    munmap(password_in_heap, MAX_PASSWORD_LENGTH + 1);
    return 0;
}
