#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <omp.h>
#include <time.h>

#define DATA_SIZE 500000000 // 500 MB
#define ENCRYPTION_ITERATIONS 1000 // Number of iterations
#define AES_KEY_SIZE 16 // 128-bit AES key size
#define PRINT_INTERVAL 100 // Print status every 100 iterations

void parallelEncryptAES(unsigned char *data, size_t size, unsigned char *key) {
    unsigned char iv[AES_KEY_SIZE] = {0}; // Initialization vector (all zeros)
    
    // Parallelized AES encryption using OpenMP
    #pragma omp parallel
    {
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new(); // Create a new EVP context

        // Initialize the context for AES encryption
        EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, iv);

        // Encrypt data in chunks of AES block size
        int out_len;
        unsigned char outbuf[AES_KEY_SIZE];

        #pragma omp for
        for (size_t i = 0; i < size; i += AES_KEY_SIZE) {
            EVP_EncryptUpdate(ctx, outbuf, &out_len, data + i, AES_KEY_SIZE);
            memcpy(data + i, outbuf, out_len); // Copy encrypted data back
        }

        // Finalize the encryption for each thread
        EVP_EncryptFinal_ex(ctx, outbuf, &out_len);

        // Clean up the context
        EVP_CIPHER_CTX_free(ctx);
    }
}

int main() {
	omp_set_num_threads(2); 
    // Allocate 500 MB of data
    unsigned char *data = (unsigned char *)malloc(DATA_SIZE);
    if (!data) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // Fill data with random values
    for (size_t i = 0; i < DATA_SIZE; i++) {
        data[i] = rand() % 256;
    }

    unsigned char key[AES_KEY_SIZE] = "1234567890abcdef"; // Example 128-bit key

    // Measure the time for parallel AES encryption
    double start_time = omp_get_wtime();
    for (int i = 0; i < ENCRYPTION_ITERATIONS; i++) {
        parallelEncryptAES(data, DATA_SIZE, key);

        // Print progress every PRINT_INTERVAL iterations
        if ((i + 1) % PRINT_INTERVAL == 0) {
            printf("Completed %d out of %d iterations...\n", i + 1, ENCRYPTION_ITERATIONS);
        }
    }
    double total_time = omp_get_wtime() - start_time;

    printf("Parallel AES encryption of 500 MB data took: %.2f seconds.\n", total_time);

    // Free allocated memory
    free(data);

    return 0;
}
