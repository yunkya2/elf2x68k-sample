#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#ifdef _POSIX_SPIN_LOCKS

static pthread_spinlock_t spinlock;
static int spin_counter = 0;
static int spin_iterations = 0;

void *spin_worker(void *arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < 1000; i++) {
        pthread_spin_lock(&spinlock);
        
        // 短い作業をシミュレート
        int temp = spin_counter;
        for (int j = 0; j < 10; j++) {
            temp++;
            spin_iterations++;
        }
        spin_counter = temp;
        
        pthread_spin_unlock(&spinlock);
    }
    
    printf("Spin worker %d: finished\n", thread_id);
    return NULL;
}

void *spin_trylock_thread(void *arg) {
    int thread_id = *(int*)arg;
    int successful_locks = 0;
    int failed_attempts = 0;
    
    for (int i = 0; i < 100; i++) {
        if (pthread_spin_trylock(&spinlock) == 0) {
            successful_locks++;
            
            // 短い作業
            spin_counter++;
            spin_iterations++;
            
            pthread_spin_unlock(&spinlock);
        } else {
            failed_attempts++;
        }
        
        usleep(1000); // 1ms
    }
    
    printf("Spin trylock %d: successful locks = %d, failed attempts = %d\n", 
           thread_id, successful_locks, failed_attempts);
    return NULL;
}

int test_spinlock() {
    printf("=== Testing pthread_spinlock ===\n");
    
    // スピンロックを初期化
    if (pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE) != 0) {
        perror("pthread_spin_init");
        return -1;
    }
    
    pthread_t workers[3], trylock;
    int worker_ids[3] = {1, 2, 3};
    int trylock_id = 4;
    
    // ワーカースレッド作成
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&workers[i], NULL, spin_worker, &worker_ids[i]) != 0) {
            perror("pthread_create worker");
            pthread_spin_destroy(&spinlock);
            return -1;
        }
    }
    
    // トライロックスレッド作成
    if (pthread_create(&trylock, NULL, spin_trylock_thread, &trylock_id) != 0) {
        perror("pthread_create trylock");
        pthread_spin_destroy(&spinlock);
        return -1;
    }
    
    // スレッド終了を待機
    for (int i = 0; i < 3; i++) {
        pthread_join(workers[i], NULL);
    }
    
    pthread_join(trylock, NULL);
    
    // スピンロックを破棄
    pthread_spin_destroy(&spinlock);
    
    printf("Final spin counter: %d\n", spin_counter);
    printf("Total iterations: %d\n", spin_iterations);
    printf("Spinlock test PASSED\n\n");
    
    return 0;
}

#else

int test_spinlock() {
    printf("=== Testing pthread_spinlock ===\n");
    printf("pthread_spinlock is not supported on this system\n");
    printf("Spinlock test SKIPPED\n\n");
    return 0;
}

#endif

#ifdef UNIT_TEST
int main() {
    return test_spinlock();
}
#endif
