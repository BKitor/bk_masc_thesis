// sendbuf/recvbuf  - Allocated memory, can be on Host or GPU
// msgsize          - Message size to test
// p2p_lat          - Latency in seconds for sending a p2p messge 
//                    of size msgsize
int bk_mif_allreduce(float *sendbuf, float *recvbuf, size_t msgsize 
                     double p2p_lat, int itterations, int warmups,
                     int mpi_mpi_rank, int mpi_size){
    double latency = 0.0, t_start = 0.0, t_stop = 0.0, timer=0.0;
    double avg_time = 0.0, max_time = 0.0, min_time = 0.0;
    double imbalance_factor = 0.0, im_s_time = 0.0, im_f_time = 0.0; 
    
    srand((int)MPI_Wtime() + mpi_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    for (int i=0; i < iterations + warmups; i++) {
        if(0 == mpi_rank)
            imbalance_factor = 0;
        else if(mpi_size - 1 == mpi_rank)
            imbalance_factor = max_imbalance_factor;
        else
            imbalance_factor = ((double)rand()/(double)RAND_MAX)
                                * max_imbalance_factor;
        int64_t sleep_time = (p2p_lat * imbalance_factor * 1e6);
        
        MPI_Barrier(MPI_COMM_WORLD);
        usleep(sleep_time);
        
        t_start = MPI_Wtime();
        MPI_Allreduce(sendbuf, recvbuf, msgsize, MPI_FLOAT,
                      MPI_SUM, MPI_COMM_WORLD );
        t_stop=MPI_Wtime();
        
        if (i>=warmups)
            timer+=t_stop-t_start;
    }
    latency = (double)(timer * 1e6) / iterations;
    
    MPI_Reduce(&latency, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0,
               MPI_COMM_WORLD);
    MPI_Reduce(&latency, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);
    MPI_Reduce(&latency, &avg_time, 1, MPI_DOUBLE, MPI_SUM, 0,
               MPI_COMM_WORLD);
    avg_time = avg_time/mpi_size;
    printf("%f %f %f\n", avg_time, min_time, max_time);
}