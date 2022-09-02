// recursive_bintree_map() - A recursive function used to calculate
//                           the mapping
// ref_rank  - Refrence rank to map other processes close to
// comm_size - Size of the communicator to calculate the mapping for
// topo_info - A matrix representing the host topology, 
//             it is of length `comm_size*comm_size`
// mapping   - The output mapping, an array of length `comm_size`
//             mapping[rank] = new_rank
void recursive_bintree_map(int ref_rank, int comm_size, 
                           int* topo_info, int* mapping){
    // calculate ref_rank's position in the tree, along with it's 
    // child ranks
    int height = log(ref_rank+1);
    int delta = pow(2, height);
    int child_1 = ref_rank + delta;
    int child_2 = ref_rakn + 2*delta;
    if(child_2 < comm_size){
        // if child_2 is in the tree,
        // map it and set it as the new ref_rank
        mapping[child_2] = find_closest_core(ref_rank, topo_info);
        recursive_bintree_map(child_2, comm_size,
                              topo_info, mapping);
    }
    if(child_1 < comm_size){
        // same as above, but for child_1
        mapping[child_1] = find_closest_core(ref_rank, topo_info);
        recursive_bintree_map(child_1, comm_size,
                              topo_info, mapping);
    }
}

int bk_remap_bintree(MPI_Comm comm, int* topo_info,
                        int* mapping){
    int comm_size;
    MPI_Comm_size(comm, comm_size);
    // start by binding rank 0 to itself, 
    // and then call the recursive function
    mapping[0] = 0;
    recursive_bintree_map(0, comm_size, topo_info, mapping);
    return MPI_SUCCESS;
}