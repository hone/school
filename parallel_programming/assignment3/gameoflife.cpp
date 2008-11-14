#include <mpi.h>
#include <iostream>
#include <string.h>
#include <gameoflife.hpp>

const int DIMENSIONS = 16;
const int DEAD = 0;
const int ALIVE = 1;
const int ITERATIONS = 64;
const int LENGTH = DIMENSIONS * DIMENSIONS;
int global_grid[ LENGTH ] = {
	0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void print_grid( int iteration, int * grid )
{
	std::cout << "Iteration " << iteration << ":\n";

	for( int y = 0; y < DIMENSIONS; y++ )
	{
		for( int x = 0; x < DIMENSIONS; x++ )
		{
			std::cout << grid[offset( x, y )] << " ";
		}
		std::cout << std::endl;
	}
}

int offset( int x, int y )
{
	return x + ( y * DIMENSIONS );
}

int count_alive_neighbors( int x, int y, int * rows )
{
	int count = 0;

	for( int i = -1; i <= 1; i++ )
	{
		for( int j = -1; j <= 1; j++ )
		{
			int new_x = x + i;
			int new_y = y + j;
			int value;

			// wrap around for x
			if( new_x < 0 )
				new_x = DIMENSIONS - 1;
			if( new_x >= DIMENSIONS )
				new_x = 0;

			// don't count itself
			if( i == 0 && j == 0 )
				value = 0;
			else
				value = rows[offset( new_x, new_y )];

			count += value;
		}
	}

	return count;
}

bool check_neighborhood( int x, int y, int * rows )
{
	int alive_count = count_alive_neighbors( x, y, rows );
	if( rows[offset( x, y )] == DEAD )
	{
		if( alive_count == 3 )
			return true;
	}
	else
	{
		if( alive_count == 2 || alive_count == 3 )
			return true;
	}

	return false;
}

int * process_rows( int * rows, int amount_to_process )
{
	int * new_rows = new int[amount_to_process * DIMENSIONS];

	// don't process first and last rows (only need for neighbor counting)
	for( int i = 1; i <= amount_to_process; i++ )
	{
		for( int x = 0; x < DIMENSIONS; x++ )
		{
			// rows x by y
			// new_rows is x by (y-2), offset is 1 from top and bottom
			if( check_neighborhood( x, i, rows ) )
				new_rows[offset( x, i - 1 )] = ALIVE;
			else
				new_rows[offset( x, i - 1 )] = DEAD;
		}
	}

	return new_rows;
}

int * setup_rows( int start_index, int end_index )
{
	// send actual + 2 rows
	int start_offset = 0;
	int length = end_index - start_index + 3;
	int * rows = new int[length * DIMENSIONS];

	// copy first row (wrap around) and copy 1 less in cell size
	if( end_index == DIMENSIONS - 1 )
	{
		length--;
		for( int i = 0; i < DIMENSIONS; i++ )
			rows[offset( i, length )] = global_grid[offset( i, 0 )];
	}
	// copy last row (wrap around) and copy 1 less in cell size
	if( start_index == 0 )
	{
		start_offset = 1;
		length--;
		for( int i = 0; i < DIMENSIONS; i++ )
			rows[offset( i, 0 )] = global_grid[offset( i, DIMENSIONS - 1)];
	}

	// copy rest of the rows
	for( int j = 0; j < length; j++ )
		for( int i = 0; i < DIMENSIONS; i++ )
			rows[offset( i, j + start_offset )] = global_grid[offset( i, j )];

	return rows;
}

int main( int argc, char ** argv )
{
	const int NUM_SEND_ROWS = 5;
	int num_procs = 0; // number of processes
	int ID; // process (or node) id
	MPI_Status stat; // MPI status parameter

	MPI_Init( & argc, &argv ); // initialize MPI environment
	MPI_Comm_rank( MPI_COMM_WORLD, &ID ); // find rank of this process in the group
	MPI_Comm_size( MPI_COMM_WORLD, &num_procs ); // find number of processes in the group

	if( ID == 0 )
	{
		print_grid( 0, global_grid );

		for( int i = 1; i <= ITERATIONS; i++ )
		{
			// temp grid to pass in
			int * rows = setup_rows( 0, DIMENSIONS - 1 );
			int * new_rows = process_rows( rows, DIMENSIONS );

			// copy back to global grid
			memcpy( global_grid, new_rows, sizeof( global_grid ) );

			print_grid( i, global_grid );

			delete[] new_rows;
			new_rows = NULL;
			delete[] rows;
			rows = NULL;
		}
	}

	MPI_Finalize();
}
