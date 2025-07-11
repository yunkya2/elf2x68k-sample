#include <stdio.h>
#include <stdlib.h>

extern int test_mutex();
extern int test_cond();
extern int test_barrier();
extern int test_rwlock();
extern int test_spinlock();

int main() {
    printf("=== pthread Synchronization Primitives Test Suite ===\n\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Mutex test
    total_tests++;
    if (test_mutex() == 0) {
        passed_tests++;
    }
    
    // Condition variable test
    total_tests++;
    if (test_cond() == 0) {
        passed_tests++;
    }
    
    // Barrier test
    total_tests++;
    if (test_barrier() == 0) {
        passed_tests++;
    }
    
    // RWLock test
    total_tests++;
    if (test_rwlock() == 0) {
        passed_tests++;
    }
    
    // Spinlock test
    total_tests++;
    if (test_spinlock() == 0) {
        passed_tests++;
    }
    
    printf("=== Test Summary ===\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", total_tests - passed_tests);
    printf("Success rate: %.1f%%\n", (float)passed_tests / total_tests * 100);
    
    return (passed_tests == total_tests) ? 0 : 1;
}
