#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_SIZE 1000000000 // 1 billion bytes (1 GB)
#define ENCRYPTION_ITERATIONS 2000 // Number of iterations of encryption to ensure prolonged execution

void encryptData(unsigned char* data, size_t size, unsigned char key) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key; // Simple XOR encryption
    }
}

int main() {
    // Allocate a large block of data
    unsigned char* data = (unsigned char*)malloc(DATA_SIZE * sizeof(unsigned char));
    if (!data) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // Fill data with random values for simulation
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % 256;
    }

    unsigned char key = 0xAA; // Example encryption key

    // Measure the time for single-core encryption
    clock_t start_time = clock();
    for (int i = 0; i < ENCRYPTION_ITERATIONS; i++) {
        encryptData(data, DATA_SIZE, key);
    }
    clock_t end_time = clock();

    double single_core_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Single-core encryption took: %.2f seconds.\n", single_core_time);

    // Free allocated memory
    free(data);

    return 0;
}
