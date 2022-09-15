// bk_ucx_sync_bmark() - Benchmark for MPI one-sided operations
// num_iters           - Numer of iterations to measure
// num_warmups         - Number of iterations to warmup for
// mpi_size, mpi_rank  - Size of / rank within mpi communicator
// remote_info         - struct with remote address/rkey information 
void bk_ucx_sync_bmark(int num_iters, int num_warmups, 
                       int mpi_size, int mpi_rank,
                       bk_remote_info *rempote_info) {
    double total_time = 0, s_time, e_time, final_time;
    int64_t reply_buf, one = 1, put_buf = mpi_rank;
    ucs_status_ptr_t status_ptr = NULL;
    ucp_request_param_t req_params = {
        .datatype = ucp_dt_make_contig(sizeof(reply_buf)),
        .reply_buffer = &reply_buf,
    };
    for (int iter = 0; iter < num_warmups+num_iters; iter++) {
        MPI_Barrier(MPI_COMM_WORLD);
        s_time = MPI_Wtime();
        // Post atomic add to remote counter
        req_params.op_attr_mask = UCP_OP_ATTR_FIELD_REPLY_BUFFER |
                                  UCP_OP_ATTR_FIELD_DATATYPE;
        status_ptr = ucp_atomic_op_nbx(remote_info->ucp_ep,
            UCP_ATOMIC_OP_ADD, &one, 1, remote_info->counter_addr,
            remote_info->counter_rkey, &req_params);
        while(UCX_INPROGRESS == UCS_PTR_STATUS(status_ptr))
            ucp_worker_progress(remote_info->ucp_worker);
        // Perform ucp_put to remote address 
        req_params.op_attr_mask = 0x0;
        status_ptr = ucp_put_nbx(remote_info->ucp_ep, &put_buf,
            sizeof(put_buf), 
            remote_info->put_addr + ((reply_buf) * sizeof(int64_t)),
            remote_info->put_rkey, &req_params);
        while(UCX_INPROGRESS == UCS_PTR_STATUS(status_ptr))
            ucp_worker_progress(remote_info->ucp_worker);
        
        e_time = MPI_Wtime();
        if (iter - num_warmups >= 0) 
            total_time += ((e_time - s_time) * 1e6)/num_iters;
    }
    
    MPI_Reduce(&total_time, &final_time, 1, MPI_DOUBLE, MPI_SUM,
               0, MPI_COMM_WORLD);
    final_time /= mpi_size;
    printf("%fus\n", final_time);
}
