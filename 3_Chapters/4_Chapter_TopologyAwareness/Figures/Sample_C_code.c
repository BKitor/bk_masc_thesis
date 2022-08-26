#include <stdio.h>
#include "mpi.h"

// convenience macro
#define BK_OUT(_str, ...) printf(_str"\n", ##__VA_ARGS__)

int main(int argc, char *argv[]){
    // initalize 
    int rank, size, prov;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &prov);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Main functionality
    BK_OUT("Hello, World, rank: %d of %d", rank, size);
    
    // cleanup
    MPI_Finalize();
    return 0;
}