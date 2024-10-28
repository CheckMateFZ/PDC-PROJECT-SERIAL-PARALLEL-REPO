#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <time.h>

#define DATA_SIZE 500000000 // 1 billion bytes (1 GB)
#define ENCRYPTION_ITERATIONS 1000 // Number of iterations
#define AES_KEY_SIZE 16 // 128-bit AES key size
#define PRINT_INTERVAL 100 // Print status every 100 iterations

void encryptAES(unsigned char *data, size_t size, unsigned char *key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new(); // Create a new EVP context
    unsigned char iv[AES_KEY_SIZE] = {0}; // Initialization vector (all zeros)

    // Initialize the context for AES encryption
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, iv);

    // Encrypt data in chunks of AES block size
    int out_len;
    unsigned char outbuf[AES_KEY_SIZE];

    for (size_t i = 0; i < size; i += AES_KEY_SIZE) {
        EVP_EncryptUpdate(ctx, outbuf, &out_len, data + i, AES_KEY_SIZE);
        memcpy(data + i, outbuf, out_len); // Copy encrypted data back
    }

    // Finalize the encryption
    EVP_EncryptFinal_ex(ctx, outbuf, &out_len);

    // Clean up the context
    EVP_CIPHER_CTX_free(ctx);
}

int main() {
    // Allocate 1 GB of data
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

    // Measure the time for serial AES encryption
    clock_t start_time = clock();
    for (int i = 0; i < ENCRYPTION_ITERATIONS; i++) {
        encryptAES(data, DATA_SIZE, key);

        // Print progress every PRINT_INTERVAL iterations
        if ((i + 1) % PRINT_INTERVAL == 0) {
            printf("Completed %d out of %d iterations...\n", i + 1, ENCRYPTION_ITERATIONS);
        }
    }
    clock_t end_time = clock();

    double total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Serial AES encryption of 1 GB data took: %.2f seconds.\n", total_time);

    // Free allocated memory
    free(data);

    return 0;
}
