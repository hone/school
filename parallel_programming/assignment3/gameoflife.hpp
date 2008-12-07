#ifndef _LIFE_H
#define _LIFE_H

#include <utility>
#include <vector>
#include <mpi.h>

class Life
{
	static const int DIMENSIONS = 16;
	static const int LENGTH = DIMENSIONS * DIMENSIONS;
	static const int DEAD = 0;
	static const int ALIVE = 1;
	static int global_grid[LENGTH];

	private:
		// MPI vars
		int num_procs; // number of processes
		int id; // process (or node) id
		int tag1;
		int tag2;
		MPI_Status stat;	// MPI status parameter

		int rows_per_process;
		std::vector< std::pair< int, int > > indexes;

		int * local_grid;
		int local_grid_size;
		int two_row_size;

		// calculate the indexes for all the processes
		void calculate_indexes();
		// joins together the top/bottom rows with the main local grid and saves it into destination
		void join_rows( int * two_rows, int * own_rows, int * destination );
		// calculates the offset of the array and if bound, will bound by DIMENSIONS
		int offset( int x, int y, bool bound = true );
		// returns the number of alive cells in the neighboring cells
		int count_alive_neighbors( int x, int y, int * rows );
		// checks surrounding neighboord and returns true or false if it should be flipped
		bool check_neighborhood( int x, int y, int * rows );
		// process the rows of the grid for an interation
		int * process_rows( int * rows );
		// create a grid of the sums in each cell
		int * grid_sums( int * rows );
		// send the row above and below
		// returns a DIMENSIONS x 2
		// first row is the row above
		// last row is the row below
		int * setup_rows( int * rows, int start_index, int end_index );
		// copy rows from source to destination
		void copy_rows( int * source, int * destination, int source_start_index, int source_end_index, int destination_start_index );
		// sets up the local grid on all the nodes
		void setup_local_grid();
		// processes all nodes
		void node_process( int * two_rows );

	public:
		Life( int id, int n_procs );
		~Life();
		// prints the grid
		void print_grid( int iteration, int * grid, int height = DIMENSIONS );
		// runs the game of life for the number of iterations
		void run( int iterations );
};

#endif
