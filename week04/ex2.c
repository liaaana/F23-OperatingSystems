#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main() {
    int vectorU[120];
    int vectorV[120];


    for (int i = 0; i < 120; i++) {
        vectorU[i] = rand() % 100;
        vectorV[i] = rand() % 100;
    }

    int n;
    printf("Enter number of processes to calculate dot product of u and v: ");
    scanf("%d", &n);
    if (n % 2 != 0 || n < 2 || n > 10) {
        printf("ERROR: Invalid value for the number of processes. Should be 2, 4, 6, 8, or 10.\n");
        return 1;
    }
    pid_t processPIDs[n];


    FILE *writeToFile = fopen("temp.txt", "w");
    if (writeToFile == NULL) {
        printf("ERROR: Impossible to open a file temp.txt.\n");
        return 1;
    }


    int elements = 120 / n;
    for (int i = 0; i < n; i++) {
        processPIDs[i] = fork();
        if (processPIDs[i] == 0) {
            int n1 = i * elements;
            int n2 = n1 + elements;
            int res = 0;
            for (int index = n1; index < n2; index++) {
                res += vectorU[index] * vectorV[index];
            }
            fprintf(writeToFile, "%d\n", res);
            fclose(writeToFile);
            return 0;
        } else if (processPIDs[i] < 0) {
            printf("ERROR: fork failed.\n");
            fclose(writeToFile);
            return 1;
        }
    }

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }


    int totalResult = 0;
    int res;

    FILE *readFile;
    readFile = fopen("temp.txt", "r");

    for (int i = 0; i < n; i++) {
        if (fscanf(readFile, "%d", &res) != 1) {
            printf("ERROR: Not enough information to calculate final result.\n");
            return 1;

        }
        totalResult += res;
    }

    fclose(writeToFile);
    fclose(readFile);
    printf("Total result: %d\n", totalResult);

    return 0;
}
