#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int ready = 0;
static int processed = 0;
static int producer_finished = 0;

void *producer_thread(void *arg) {
    printf("Producer: starting\n");
    
    for (int i = 1; i <= 5; i++) {
        pthread_mutex_lock(&mutex);
        
        ready = i;
        printf("Producer: produced item %d\n", i);
        pthread_cond_broadcast(&cond);  // broadcastを使用して全スレッドに通知
        
        pthread_mutex_unlock(&mutex);
        usleep(200000); // 200ms
    }
    
    // プロデューサー完了を通知
    pthread_mutex_lock(&mutex);
    producer_finished = 1;
    pthread_cond_broadcast(&cond);  // 全スレッドに完了を通知
    pthread_mutex_unlock(&mutex);
    
    printf("Producer: finished\n");
    return NULL;
}

void *consumer_thread(void *arg) {
    int thread_id = *(int*)arg;
    printf("Consumer %d: starting\n", thread_id);
    
    while (1) {
        pthread_mutex_lock(&mutex);
        
        // アイテムが準備されるまで待機、またはプロデューサーが完了するまで待機
        while (ready == processed && !producer_finished) {
            printf("Consumer %d: waiting for item\n", thread_id);
            pthread_cond_wait(&cond, &mutex);
        }
        
        // まだ処理していないアイテムがあるかチェック
        if (processed < ready) {
            processed++;
            printf("Consumer %d: consumed item %d\n", thread_id, processed);
            pthread_mutex_unlock(&mutex);
            usleep(100000); // 100ms
        } else {
            // 処理するアイテムがないか、プロデューサーが完了した場合は終了
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
    
    printf("Consumer %d: finished\n", thread_id);
    return NULL;
}

void *signal_test_thread(void *arg) {
    int thread_id = *(int*)arg;
    printf("Signal Test %d: starting\n", thread_id);
    
    pthread_mutex_lock(&mutex);
    
    // 条件を待機（プロデューサーからのシグナルを待つ）
    while (ready == 0 && !producer_finished) {
        printf("Signal Test %d: waiting for first signal\n", thread_id);
        pthread_cond_wait(&cond, &mutex);
    }
    
    if (ready > 0) {
        printf("Signal Test %d: received signal, ready = %d\n", thread_id, ready);
    }
    
    pthread_mutex_unlock(&mutex);
    
    printf("Signal Test %d: finished\n", thread_id);
    return NULL;
}

int test_cond() {
    printf("=== Testing pthread_cond ===\n");
    
    // グローバル変数を初期化
    ready = 0;
    processed = 0;
    producer_finished = 0;
    
    pthread_t producer, consumer1, consumer2, signal_test;
    int consumer1_id = 1, consumer2_id = 2, signal_id = 3;
    
    // シグナルテスト用のスレッドを先に作成
    if (pthread_create(&signal_test, NULL, signal_test_thread, &signal_id) != 0) {
        perror("pthread_create signal_test");
        return -1;
    }
    
    usleep(100000); // 100ms待機してからプロデューサー/コンシューマーを開始
    
    // スレッド作成
    if (pthread_create(&producer, NULL, producer_thread, NULL) != 0) {
        perror("pthread_create producer");
        return -1;
    }
    
    if (pthread_create(&consumer1, NULL, consumer_thread, &consumer1_id) != 0) {
        perror("pthread_create consumer1");
        return -1;
    }
    
    if (pthread_create(&consumer2, NULL, consumer_thread, &consumer2_id) != 0) {
        perror("pthread_create consumer2");
        return -1;
    }
    
    // スレッド終了を待機
    pthread_join(signal_test, NULL);
    pthread_join(producer, NULL);
    pthread_join(consumer1, NULL);
    pthread_join(consumer2, NULL);
    
    printf("Total items processed: %d (expected: 5)\n", processed);
    printf("Condition variable test %s\n\n", (processed == 5) ? "PASSED" : "FAILED");
    
    return (processed == 5) ? 0 : -1;
}

#ifdef UNIT_TEST
int main() {
    return test_cond();
}
#endif
