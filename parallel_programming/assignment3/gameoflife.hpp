// prints the grid
void print_grid( int iteration, int * grid, int height );
// calculates the offset of the array and if bound, will bound by DIMENSIONS
int offset( int x, int y, bool bound = true );
// returns the number of alive cells in the neighboring cells
int count_alive_neighbors( int x, int y, int * rows );
// checks surrounding neighboord and returns true or false if it should be flipped
bool check_neighborhood( int x, int y, int * rows );
// process the rows of the grid for an interation
int * process_rows( int * rows, int * local_grid, int amount_to_process );
// create a grid of the sums in each cell
int * grid_sums( int * rows, int amount_to_process );
// send the row above and below
// returns a DIMENSIONS x 2
// first row is the row above
// last row is the row below
int * setup_rows( int * rows, int start_index, int end_index );
// joins together the top/bottom rows with the main local grid and saves it into destination
void join_rows( int * two_rows, int * own_rows, int * destination, int rows_per_process );
// copy rows from source to destination
void copy_rows( int * source, int * destination, int source_start_index, int source_end_index, int destination_start_index );
// calculate the indexes for all the processes
void calculate_indexes( std::vector< std::pair< int, int > > &indexes, int rows_per_process, int num_procs );
// sets up the local grid on all the nodes
void setup_local_grid( int * global_grid, int * local_grid, std::vector< std::pair< int, int > > indexes, int local_grid_size, int num_procs, int tag );
// processes all nodes
void node_process( int * two_rows, int * local_grid, int rows_per_process );
