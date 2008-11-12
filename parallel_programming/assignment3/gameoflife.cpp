#include <mpi.h>
#include <iostream>

const int DIMENSIONS = 16;
const int DEAD = 0;
const int ALIVE = 1;
int empty_row[ DIMENSIONS ] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int global_grid[ DIMENSIONS * DIMENSIONS ] = {
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

// calculates the offset of the array
int offset( int x, int y )
{
	return x + ( y * DIMENSIONS );
}

// returns the number of alive cells in the neighboring cells
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
			if( new_x < 0 || new_y < 0 || new_x >= DIMENSIONS || new_y >= DIMENSIONS || ( i == 0 && j == 0 ) )
				value = 0;
			else
				value = rows[offset( new_x, new_y )];

			count += value;
		}
	}

	return count;
}

// checks surrounding neighboord and returns true or false if it should be flipped
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

// process the rows of the grid for an interation
int * process_rows( int * rows, int amount_to_process )
{
	int * new_rows = new int[amount_to_process * DIMENSIONS];

	for( int i = 1; i < amount_to_process; i++ )
	{
		for( int x = 0; x < DIMENSIONS; x++ )
		{
			if( check_neighborhood( x, i, rows ) )
				new_rows[offset( x, i - 1 )] = ALIVE;
			else
				new_rows[offset( x, i - 1 )] = DEAD;
		}
	}

	return new_rows;
}

// prints the grid
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

int main( int argc, char ** argv )
{
	const int NUM_SEND_ROWS = 5;
	int num_procs = 0; // number of processes
	int ID; // process (or node) id
	MPI_Status stat; // MPI status parameter
	int * buffer = new int[NUM_SEND_ROWS * DIMENSIONS];

	MPI_Init( & argc, &argv ); // initialize MPI environment
	MPI_Comm_rank( MPI_COMM_WORLD, &ID ); // find rank of this process in the group
	MPI_Comm_size( MPI_COMM_WORLD, &num_procs ); // find number of processes in the group

	if( ID == 0 )
	{
		print_grid( 0, global_grid );
		int * rows = new int[DIMENSIONS * ( DIMENSIONS + 2 )];
		// prepare global grid
		for( int i = 0; i < DIMENSIONS; i++ )
		{
			for( int x = 0; x < DIMENSIONS; x++ )
			{
				rows[offset( x, i + 1 )] = global_grid[offset( x, i )];
			}
		}
		// set first and last rows as empty
		for( int i = 0 ; i < 2; i++ )
		{
			int y = 0;
			if( i == 1 )
				y = DIMENSIONS + 1;

			for( int x = 0; x < DIMENSIONS; x++ )
			{
				rows[offset( x, y )] = empty_row[x];
			}
		}

		int * new_rows = process_rows( rows, DIMENSIONS );
		print_grid( 1, new_rows );
		delete[] rows;
		rows = NULL;
		delete[] new_rows;
		new_rows = NULL;
	}

	MPI_Finalize();

	delete[] buffer;
	buffer = NULL;
}
