#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#if defined(_POSIX_BARRIERS) && _POSIX_BARRIERS > 0

static pthread_barrier_t barrier;
static int thread_count = 4;

void *barrier_worker(void *arg) {
    int thread_id = *(int*)arg;
    
    printf("Thread %d: doing some work...\n", thread_id);
    
    // 各スレッドが異なる時間作業をシミュレート
    usleep(thread_id * 100000); // thread_id * 100ms
    
    printf("Thread %d: waiting at barrier\n", thread_id);
    
    int result = pthread_barrier_wait(&barrier);
    
    if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("Thread %d: I am the serial thread!\n", thread_id);
    } else if (result == 0) {
        printf("Thread %d: barrier passed\n", thread_id);
    } else {
        printf("Thread %d: barrier error\n", thread_id);
    }
    
    printf("Thread %d: continuing after barrier\n", thread_id);
    
    return NULL;
}

int test_barrier() {
    printf("=== Testing pthread_barrier ===\n");
    
    // バリアを初期化
    if (pthread_barrier_init(&barrier, NULL, thread_count) != 0) {
        perror("pthread_barrier_init");
        return -1;
    }
    
    pthread_t threads[thread_count];
    int thread_ids[thread_count];
    
    // スレッド作成
    for (int i = 0; i < thread_count; i++) {
        thread_ids[i] = i + 1;
        if (pthread_create(&threads[i], NULL, barrier_worker, &thread_ids[i]) != 0) {
            perror("pthread_create");
            pthread_barrier_destroy(&barrier);
            return -1;
        }
    }
    
    // スレッド終了を待機
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // バリアを破棄
    pthread_barrier_destroy(&barrier);
    
    printf("Barrier test PASSED\n\n");
    return 0;
}

#else

int test_barrier() {
    printf("=== Testing pthread_barrier ===\n");
    printf("pthread_barrier is not supported on this system\n");
    printf("Barrier test SKIPPED\n\n");
    return 0;
}

#endif

#ifdef UNIT_TEST
int main() {
    return test_barrier();
}
#endif
