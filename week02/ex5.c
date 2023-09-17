#include <stdio.h>

long long tribonacci(int n) {
    if (n == 0) {
        return 0;
    } else if (n == 1 || n == 2) {
        return 1;
    }

    long long trib0 = 0;
    long long trib1 = 1;
    long long trib2 = 1;
    long long tribN = 0;
    for (int i = 3; i < n + 1; i++) {
        tribN = trib0 + trib1 + trib2;
        trib0 = trib1;
        trib1 = trib2;
        trib2 = tribN;
    }
    return tribN;
}

int main() {
    long long tribonacci4 = tribonacci(4);
    long long tribonacci36 = tribonacci(36);
    printf("Tribonacci(4) = %llu\n", tribonacci4);
    printf("Tribonacci(36) = %llu\n", tribonacci36);
    return 0;
}
