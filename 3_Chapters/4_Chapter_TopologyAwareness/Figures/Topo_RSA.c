// comm      - Communicator to execute collective on
// topo_info - A matrix representing the host topology, 
//             it is of length `comm_size*comm_size`
// mapping   - The output mapping, an array of length `comm_size`
//             the array is initialized to all -1,
//             mapping[rank] = new_rank
int bk_rsal_heuristic(MPI_Comm comm, int* topo_info,
                        int* mapping){
    int mask = 1, num_mappings = 0;
    int refrence_rank = 0, refrence_core = 0;
    int comm_size;
    MPI_Comm_size(comm, &comm_size);
    // start by mapping rank 0 to core 0
    mapping[refrence_rank] = refrence_core;
    for(int i = 1; i<comm_size; i++){ // for each remining rank
        // loop over processes that are a power of 2 distance away
        // and find one that hasn't been mappped
        while(-1 == mapping[ref_rank ^ mask]){ 
            mask *= 2;
        }
        int rank_to_map = mask ^ ref_rank;
        int core_to_map = find_closest_core(reference_rank,
                                            topo_info);
        mapping[rank_to_map] = core_to_map;
        num_mappings++;
        // update the reference rank after 2 mappings
        if(2 >= num_mappings){
            refrence_rank = rank_to_map;
            mask = 1;
            num_mappings = 0;
        }
    }
    return MPI_SUCCESS;
}