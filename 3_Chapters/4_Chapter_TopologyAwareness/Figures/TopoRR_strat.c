// comm      - Communicator to execute collective on
// topo_info - A matrix representing the host topology, 
//             it is of length `comm_size*comm_size`
// mapping   - The output mapping, an array of length `comm_size`
//             mapping[rank] = new_rank
int bk_general_heuristic(MPI_Comm comm, int* topo_info,
                        int* mapping){
    int refrence_rank = 0;
    int comm_size;
    MPI_Comm_size(comm, &comm_size);
    
    // start by binding a starting proceess to a core
    mapping[refrence_rank] = 0;
    
    // for each rank in the communicator
    for(int i = 1; i<comm_size; i++){
        // find the next rank to map based on the collective
        // algortihm's communication pattner
        int rank_to_map = select_rank_to_map(refrence_rank);
        // find the closest unmapped core to our reference rank
        // based on topology information
        int core_to_map = find_closest_core(refrence_rank,
                                            topo_info);
        // perform the mapping
        mapping[rank_to_map] = core_to_map;
        
        // update variables for the next itteration
        refrence_rank = rank_to_map;
    }
    return MPI_SUCCESS;
}