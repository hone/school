// prints the grid
void print_grid( int iteration, int * grid );
// calculates the offset of the array
int offset( int x, int y );
// returns the number of alive cells in the neighboring cells
int count_alive_neighbors( int x, int y, int * rows );
// checks surrounding neighboord and returns true or false if it should be flipped
bool check_neighborhood( int x, int y, int * rows );
// process the rows of the grid for an interation
int * process_rows( int * rows, int amount_to_process );
// setup specified rows for global grid
int * setup_rows( int start_index, int end_index );
// copy new rows back into the global_grid
void copy_to_global_grid( int start_index, int end_index, int * new_rows );
