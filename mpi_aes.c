#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <mpi.h>

#define DATA_SIZE 500000000 // 500 MB of data
#define ENCRYPTION_ITERATIONS 1000 // Number of iterations
#define AES_KEY_SIZE 16 // 128-bit AES key size
#define PRINT_INTERVAL 100 // Print status every 100 iterations

void encryptAES(unsigned char *data, size_t size, unsigned char *key) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new(); // Create encryption context
    unsigned char iv[AES_KEY_SIZE] = {0}; // Initialization vector (all zeros)
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, iv); // Initialize AES encryption

    int out_len;
    unsigned char outbuf[AES_KEY_SIZE];

    for (size_t i = 0; i < size; i += AES_KEY_SIZE) {
        EVP_EncryptUpdate(ctx, outbuf, &out_len, data + i, AES_KEY_SIZE);
        memcpy(data + i, outbuf, out_len); // Copy encrypted data back
    }

    EVP_EncryptFinal_ex(ctx, outbuf, &out_len); // Finalize encryption
    EVP_CIPHER_CTX_free(ctx); // Free the encryption context
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Get total processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Get current process rank

    size_t local_size = DATA_SIZE / world_size; // Divide data among processes
    unsigned char *local_data = (unsigned char *)malloc(local_size);
    if (!local_data) {
        fprintf(stderr, "Memory allocation failed on process %d\n", world_rank);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Process 0 generates random data and scatters it to all processes
    if (world_rank == 0) {
        unsigned char *global_data = (unsigned char *)malloc(DATA_SIZE);
        if (!global_data) {
            fprintf(stderr, "Memory allocation failed on root process!\n");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }

        // Fill data with random values
        for (size_t i = 0; i < DATA_SIZE; i++) {
            global_data[i] = rand() % 256;
        }

        // Scatter data to all processes
        MPI_Scatter(global_data, local_size, MPI_UNSIGNED_CHAR, local_data, local_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
        free(global_data);
    } else {
        // Other processes receive their chunk of data
        MPI_Scatter(NULL, local_size, MPI_UNSIGNED_CHAR, local_data, local_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    }

    // Broadcast AES key to all processes
    unsigned char key[AES_KEY_SIZE] = "1234567890abcdef";
    MPI_Bcast(key, AES_KEY_SIZE, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    // Start timing for the encryption
    double start_time = MPI_Wtime();

    // Perform encryption
    for (int i = 0; i < ENCRYPTION_ITERATIONS; i++) {
        encryptAES(local_data, local_size, key);

        // Print progress from rank 0
        if (world_rank == 0 && (i + 1) % PRINT_INTERVAL == 0) {
            printf("Completed %d out of %d iterations...\n", i + 1, ENCRYPTION_ITERATIONS);
        }
    }

    double end_time = MPI_Wtime();
    double total_time = end_time - start_time;

    // Gather encrypted data back at process 0
    if (world_rank == 0) {
        unsigned char *global_data = (unsigned char *)malloc(DATA_SIZE);
        MPI_Gather(local_data, local_size, MPI_UNSIGNED_CHAR, global_data, local_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

        printf("Parallel AES encryption of 500 MB data using %d processes took: %.2f seconds.\n", world_size, total_time);
        free(global_data);
    } else {
        MPI_Gather(local_data, local_size, MPI_UNSIGNED_CHAR, NULL, local_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
