#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int shared_counter = 0;

void *worker_thread(void *arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&mutex);
        
        int temp = shared_counter;
        printf("Thread %d: reading counter = %d\n", thread_id, temp);
        usleep(100000); // 100ms
        shared_counter = temp + 1;
        printf("Thread %d: incremented counter to %d\n", thread_id, shared_counter);
        
        pthread_mutex_unlock(&mutex);
        usleep(50000); // 50ms
    }
    
    return NULL;
}

int test_mutex() {
    printf("=== Testing pthread_mutex ===\n");
    
    pthread_t threads[3];
    int thread_ids[3] = {1, 2, 3};
    
    // スレッド作成
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, worker_thread, &thread_ids[i]) != 0) {
            perror("pthread_create");
            return -1;
        }
    }
    
    // スレッド終了を待機
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Final counter value: %d (expected: 15)\n", shared_counter);
    printf("Mutex test %s\n\n", (shared_counter == 15) ? "PASSED" : "FAILED");
    
    return (shared_counter == 15) ? 0 : -1;
}

#ifdef UNIT_TEST
int main() {
    return test_mutex();
}
#endif
