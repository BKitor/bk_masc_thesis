// recursive_knomial_map() - A recursive function used to calculate
//                           the mapping
// ref_rank  - Refrence rank to map other processes close to
// upper_lim - Upper limit for rank mapping, used as exit condition
//             of recursive function
// k         - Order of the knomial tree
// topo_info - A matrix representing the host topology, 
//             it is of length `comm_size*comm_size`
// mapping   - The output mapping, an array of length `comm_size`
//             mapping[rank] = new_rank
void recursive_knomial_map(int ref_rank, int upper_lim, int k,
                           int* topo_info, int* mapping){
    // start by mapping subtrees of hight 1 and distance 1
   for(int i = (ref_rank + 1); i < (ref_rank + k - 1); i++){
       if (i >= upper_lim) break;
       mapping[i] = find_closest_core(ref_rank, topo_info);
   } 
   int mask = 1;
   // Calculate and bind subtrees that are furthur than distance 1 
   // This while loop is the exit condition
   while (mask * k < upper_lim){
       // there are (k-1) subtrees to bind
       for(int i = 1; i <= k; i++){
           // calculate the node vaule, make sure it is valid,
           // bind it, and recurse on it
           int new_rank = ref_rank + (mask * k * i);
           if (new_rank >= upper_lim) break;
           mapping[new_rank] = find_closest_core(ref_rank,
                                                 topo_info);
           recusive_knomial_map(new_rank, (ref_rank + k * mask),
                                k, topo_inof, mapping);
       }
       mask *= k;
   }
}
int bk_remap_knomial(MPI_Comm comm, int* topo_info, int k,
                        int* mapping){
    int comm_size;
    MPI_Comm_size(comm, comm_size);
    // start by binding rank 0 to itself, 
    // and then call the recursive function
    mapping[0] = 0;
    recursive_knomial_map(0, comm_size, k, topo_info, mapping);
    return MPI_SUCCESS;
}