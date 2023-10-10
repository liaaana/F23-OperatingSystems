#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>


pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
int k = 0;
int c = 0;
int n = 0;

bool is_prime(int num) {
    if (num < 2)
        return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0)
            return false;
    }
    return true;
}

int get_number_to_check() {
    pthread_mutex_lock(&global_lock);
    int num = k;
    if (k < n)
        k++;
    pthread_mutex_unlock(&global_lock);
    return num;
}

void increment_primes() {
    pthread_mutex_lock(&global_lock);
    c++;
    pthread_mutex_unlock(&global_lock);
}

void *check_primes(void *arg) {
    while (1) {
        int candidate = get_number_to_check();
        if (candidate >= n)
            break;
        if (is_prime(candidate))
            increment_primes();
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("ERROR: Only 2 arguments needed (n and m)\n");
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);
    if (n < 0) {
        printf("ERROR: n should be greater than zero\n");
        return EXIT_FAILURE;
    }
    int m = atoi(argv[2]);
    pthread_t threads[m];


    for (int i = 0; i < m; i++) {
        if (pthread_create(&threads[i], NULL, check_primes, NULL) != 0) {
            printf("ERROR: Thread creation failed.\n");
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < m; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("ERROR: Thread join failed.\n");
            return EXIT_FAILURE;
        }
    }

    printf("Number of primes in the range [0, %d) is %d (calculated by %d thread(s))\n", n, c, m);
    return EXIT_SUCCESS;
}
