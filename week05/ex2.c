#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define MESSAGE_SIZE 256


struct Thread {
    pthread_t id;
    char message[MESSAGE_SIZE];
    int i;
};


void *get_thread_info(void *arg) {
    struct Thread *thread_info = (struct Thread *) arg;
    printf("%s with ID: %lu\n", thread_info->message, (unsigned long) thread_info->id);
    pthread_exit(NULL);
}

int main() {
    int n;
    printf("Enter number of threads: ");
    scanf("%d", &n);

    struct Thread threads[n];
    for (int index = 0; index < n; index++) {
        threads[index].i = index;
        sprintf(threads[index].message, "Hello from thread %d", threads[index].i);

        printf("Thread %d is created\n", threads[index].i);
        pthread_create(&threads[index].id, NULL, get_thread_info, &threads[index]);

        pthread_join(threads[index].id, NULL);
        printf("Thread %d is finished\n", threads[index].i);
    }
    return 0;
}
