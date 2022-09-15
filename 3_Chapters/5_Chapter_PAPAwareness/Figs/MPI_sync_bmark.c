// bk_mpi_sync_bmark() - Benchmark for MPI one-sided operations
// num_iters           - Numer of iterations to measure
// num_warmups         - Number of iterations to warmup for
// mpi_size, mpi_rank  - Size / rank of mpi communicator
// counter_win         - Window to fetch and op on
// write_win           - Window to put data into
void bk_mpi_sync_bmark(int num_iters, int num_warmups, 
                       int mpi_size, int mpi_rank,
                       MPI_Win counter_win, MPI_Win write_win) {
    double total_time = 0.0, s_time, e_time, final_time;
    int64_t i64_rank = mpi_rank, one = 1, arrival;
    
    for (int iter = 0; iter < num_iters + num_warmupus; iter++) {
        
        MPI_Barrier(MPI_COMM_WORLD);
        s_time = MPI_Wtime();
        
        // Start access epoch and send a fetch and op
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, counter_win);
        MPI_Fetch_and_op(&one, &arrival, MPI_INT64_T, 0, 0,
                        MPI_SUM, counter_win);
        MPI_Win_unlock(0, counter_win);
        
        // Start access epoch and write to peer memory
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, write_win);
        MPI_Put(&i64_rank, 1, MPI_INT64_T, 0, arrival, write_win);
        MPI_Win_unlock(0, write_win);
        
        e_time = MPI_Wtime();
        
        if (iter - num_warmupus >= 0) // ignore warmup iterations
            total_time += ((e_time - s_time) * 1e6)/num_iters;
    }
    
    MPI_Reduce(&total_time, &final_time, 1, MPI_DOUBLE,
                MPI_SUM, 0, MPI_COMM_WORLD);
    final_time /= mpi_size;
    
    printf("%fus\n", final_time);
}