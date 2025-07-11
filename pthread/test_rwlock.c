#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
static int shared_data = 0;
static int read_count = 0;

void *reader_thread(void *arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < 3; i++) {
        pthread_rwlock_rdlock(&rwlock);
        
        printf("Reader %d: reading data = %d (read #%d)\n", thread_id, shared_data, ++read_count);
        usleep(200000); // 200ms
        
        pthread_rwlock_unlock(&rwlock);
        usleep(100000); // 100ms
    }
    
    printf("Reader %d: finished\n", thread_id);
    return NULL;
}

void *writer_thread(void *arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < 2; i++) {
        usleep(300000); // 300ms待機してからwrite
        
        pthread_rwlock_wrlock(&rwlock);
        
        shared_data += 10;
        printf("Writer %d: wrote data = %d\n", thread_id, shared_data);
        usleep(400000); // 400ms
        
        pthread_rwlock_unlock(&rwlock);
        usleep(200000); // 200ms
    }
    
    printf("Writer %d: finished\n", thread_id);
    return NULL;
}

void *trylock_thread(void *arg) {
    int thread_id = *(int*)arg;
    
    printf("Trylock %d: attempting read trylock\n", thread_id);
    
    if (pthread_rwlock_tryrdlock(&rwlock) == 0) {
        printf("Trylock %d: got read lock, data = %d\n", thread_id, shared_data);
        usleep(100000);
        pthread_rwlock_unlock(&rwlock);
    } else {
        printf("Trylock %d: read trylock failed\n", thread_id);
    }
    
    usleep(500000); // 500ms
    
    printf("Trylock %d: attempting write trylock\n", thread_id);
    
    if (pthread_rwlock_trywrlock(&rwlock) == 0) {
        printf("Trylock %d: got write lock, incrementing data\n", thread_id);
        shared_data++;
        usleep(100000);
        pthread_rwlock_unlock(&rwlock);
    } else {
        printf("Trylock %d: write trylock failed\n", thread_id);
    }
    
    printf("Trylock %d: finished\n", thread_id);
    return NULL;
}

int test_rwlock() {
    printf("=== Testing pthread_rwlock ===\n");
    
    pthread_t readers[3], writers[2], trylock;
    int reader_ids[3] = {1, 2, 3};
    int writer_ids[2] = {1, 2};
    int trylock_id = 1;
    
    // リーダースレッド作成
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]) != 0) {
            perror("pthread_create reader");
            return -1;
        }
    }
    
    // ライタースレッド作成
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&writers[i], NULL, writer_thread, &writer_ids[i]) != 0) {
            perror("pthread_create writer");
            return -1;
        }
    }
    
    // トライロックスレッド作成
    if (pthread_create(&trylock, NULL, trylock_thread, &trylock_id) != 0) {
        perror("pthread_create trylock");
        return -1;
    }
    
    // スレッド終了を待機
    for (int i = 0; i < 3; i++) {
        pthread_join(readers[i], NULL);
    }
    
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }
    
    pthread_join(trylock, NULL);
    
    printf("Final data value: %d\n", shared_data);
    printf("Total reads: %d\n", read_count);
    printf("RWLock test PASSED\n\n");
    
    return 0;
}

#ifdef UNIT_TEST
int main() {
    return test_rwlock();
}
#endif
