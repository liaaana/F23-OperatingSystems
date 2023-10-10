#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

bool is_prime(int n) {
    if (n <= 1) return false;
    int i = 2;
    for (; i * i <= n; i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}

int primes_count(int a, int b) {
    int count = 0;
    for (int i = a; i < b; i++)
        if (is_prime(i))
            count++;
    return count;
}

typedef struct prime_request {
    int a, b;
} prime_request;

void *prime_counter(void *arg) {
    prime_request *req = (prime_request *) arg;
    int count = primes_count(req->a, req->b);

    int *result = (int *) malloc(sizeof(int));
    if (result == NULL) {
        printf("ERROR: Memory allocation failed\n");
        return (void *) result;
    }

    *result = count;
    return (void *) result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("ERROR: Only 2 arguments needed (n and m)");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    pthread_t threads[m];
    prime_request requests[m];
    int elementsInThread = n / m;
    int elementsInLastThread = elementsInThread + (n % m);

    for (int i = 0; i < m; i++) {
        requests[i].a = i * elementsInThread;
        if (i == m - 1) {
            requests[i].b = (i * elementsInThread + elementsInLastThread);
        } else {
            requests[i].b = (i + 1) * elementsInThread;
        }
        if (pthread_create(&threads[i], NULL, prime_counter, (void *) &requests[i]) > 0) {
            printf("ERROR: Thread creation failed\n");
            return EXIT_FAILURE;
        }
    }

    int result = 0;
    for (int i = 0; i < m; i++) {
        int *partialResult;
        if (pthread_join(threads[i], (void **) &partialResult) > 0) {
            printf("ERROR: Thread join failed\n");
            return EXIT_FAILURE;
        }
        result += *partialResult;
        free(partialResult);
    }
    printf("Number of primes in the range [0, %d) is %d (calculated by %d thread(s))\n", n, result, m);
    return EXIT_SUCCESS;
}
