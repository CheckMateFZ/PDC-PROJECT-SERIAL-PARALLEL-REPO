# PDC-PROJECT-SERIAL-REPO

## DISTRIBUTED AES DATA ENCRYPTION SYSTEM
TO RUN ON YOUR SYSTEM(KALI LINUX)

### SERIAL CODE [PDCAES]
COMPILE FIRST USING
`gcc -o PDCAES PDCAES.c -lssl -lcrypto`

THEN EXECUTE USING
`./PDCAES`

### OPENMP CODE [PDCAESOPENMP]
COMPILE FIRST USING
`gcc -o PDCAESOPENMP PDCAESOPENMP.c -lssl -lcrypto -fopenmp`

THEN EXECUTE USING
`./PDCAESOPENMP`

### MPI CODE [mpi_aes]
COMPILE FIRST USING
`mpicc -o mpi_aes mpi_aes.c -lssl -lcrypto`

THEN EXECUTE USING
`mpirun -np 6 ./mpi_aes`

### HYBRID CODE [hybrid_aes]
COMPILE FIRST USING
`mpicc -o hybrid_aes hybrid_aes.c -lssl -lcrypto -fopenmp`

THEN EXECUTE USING
`mpirun -np 6 ./hybrid_aes`
