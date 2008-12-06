#include <mpi.h>
#include <iostream>
#include <string.h>
#include <gameoflife.hpp>
#include <vector>
#include <utility>

const int DIMENSIONS = 16;
const int DEAD = 0;
const int ALIVE = 1;
const int ITERATIONS = 64;
const int LENGTH = DIMENSIONS * DIMENSIONS;

void print_grid( int iteration, int * grid, int height )
{
	std::cout << "Iteration " << iteration << ":\n";

	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < DIMENSIONS; x++ )
		{
			std::cout << grid[offset( x, y, false )] << " ";
		}
		std::cout << std::endl;
	}
}

int offset( int x, int y, bool bound )
{
	int new_x = x;
	int new_y = y;

	// handle wrap arounds
	if( bound )
	{
		new_x = ( x + DIMENSIONS ) % DIMENSIONS;
		new_y = ( y + DIMENSIONS ) % DIMENSIONS;
	}

	return new_x + ( new_y * DIMENSIONS );
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

int * process_rows( int * rows, int * local_grid, int amount_to_process )
{
	// don't process first and last rows (only need for neighbor counting)
	for( int i = 1; i <= amount_to_process; i++ )
	{
		for( int x = 0; x < DIMENSIONS; x++ )
		{
			// rows x by y
			// local_grid is x by (y-2), offset is 1 from top and bottom
			if( check_neighborhood( x, i, rows ) )
				local_grid[offset( x, i - 1 )] = ALIVE;
			else
				local_grid[offset( x, i - 1 )] = DEAD;
		}
	}
}

int * grid_sums( int * rows, int amount_to_process )
{
	int * new_rows = new int[amount_to_process * DIMENSIONS];

	// don't process first and last rows (only need for neighbor counting)
	for( int i = 1; i <= amount_to_process; i++ )
	{
		for( int x = 0; x < DIMENSIONS; x++ )
		{
			new_rows[offset( x, i - 1 )] = count_alive_neighbors( x, i, rows );
		}
	}

	return new_rows;
}

// send the row above and below
// returns a DIMENSIONS x 2
// first row is the row above
// last row is the row below
int * setup_rows( int * rows, int start_index, int end_index )
{
    int first_row = start_index - 1;
    int last_row = end_index + 1;
    int * new_rows = new int[DIMENSIONS * 2];

    for( int i = 0; i < DIMENSIONS; i++ )
    {
        new_rows[offset( i, 0 )] = rows[offset( i, first_row )];
        new_rows[offset( i, 1 )] = rows[offset( i, last_row )];
    }

	return new_rows;
}

void join_rows( int * two_rows, int * own_rows, int * destination, int rows_per_process )
{
    for( int i = 0; i < DIMENSIONS; i++ )
    {
        destination[offset( i, 0 )] = two_rows[offset( i, 0 )];
        destination[offset( i, rows_per_process + 1, false )] = two_rows[offset( i, 1 )];

        for( int j = 1; j <= rows_per_process; j++ )
            destination[offset( i, j )] = own_rows[offset( i, j - 1 )];
    }
}

void copy_rows( int * source, int * destination, int source_start_index, int source_end_index, int destination_start_index )
{
	int length = source_end_index - source_start_index + 1;
	for( int j = 0; j < length; j++ )
		for( int i = 0; i < DIMENSIONS; i++ )
			destination[offset( i, j + destination_start_index )] = source[offset( i, j + source_start_index )];
}

void calculate_indexes( std::vector< std::pair< int, int > > &indexes, int rows_per_process, int num_procs )
{
    int start_index = 0;
    int end_index = rows_per_process - 1;
    indexes.push_back( std::make_pair( start_index, end_index ) );

    for( int j = 1; j < num_procs; j++ )
    {
        start_index = end_index + 1;
        end_index = start_index + rows_per_process - 1;
        indexes.push_back( std::make_pair( start_index, end_index ) );
    }
}

void setup_local_grid( int * global_grid, int * local_grid, std::vector< std::pair< int, int > > indexes, int local_grid_size, int num_procs, int tag )
{
      copy_rows( global_grid, local_grid, indexes[0].first, indexes[0].second, 0 );
      int * rows_setup = new int[local_grid_size];

      for( int j = 1; j < num_procs; j++ )
      {
          copy_rows( global_grid, rows_setup, indexes[j].first, indexes[j].second, 0 );
          MPI_Send( rows_setup, local_grid_size, MPI_INT, j, tag, MPI_COMM_WORLD );
      }

      delete[] rows_setup;
	  rows_setup = NULL;
}

void node_process( int * two_rows, int * local_grid, int rows_per_process )
{
	int * rows = new int[DIMENSIONS * ( rows_per_process + 2 )];

	join_rows( two_rows, local_grid, rows, rows_per_process );
	process_rows( rows, local_grid, rows_per_process );
	delete[] rows;
	rows = NULL;
}

int main( int argc, char ** argv )
{
	int num_procs = 0; // number of processes
	int ID; // process (or node) id
	int tag1 = 1;
	int tag2 = 2;
	MPI_Status stat; // MPI status parameter

	MPI_Init( & argc, &argv ); // initialize MPI environment
	MPI_Comm_rank( MPI_COMM_WORLD, &ID ); // find rank of this process in the group
	MPI_Comm_size( MPI_COMM_WORLD, &num_procs ); // find number of processes in the group

	int rows_per_process = DIMENSIONS / num_procs;
	std::vector< std::pair< int, int > > indexes;
    int test_grid[LENGTH] = {
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
    int local_grid[rows_per_process * DIMENSIONS];
    int local_grid_size = rows_per_process * DIMENSIONS;

    // need special prep before the first iteration
	if( ID == 0 )
    {
		print_grid( 0, test_grid, DIMENSIONS );

        calculate_indexes( indexes, rows_per_process, num_procs );
        setup_local_grid( test_grid, local_grid, indexes, local_grid_size, num_procs, tag1 );
    }
    else
    {
        MPI_Recv( local_grid, local_grid_size, MPI_INT, 0, tag1, MPI_COMM_WORLD, &stat );
    }

    // do the iterations
	for( int i = 1; i <= ITERATIONS; i++ )
	{
		if( ID == 0 )
		{
			//int * rows = setup_rows( start_index, end_index );
			// send
			for( int j = 1; j < num_procs; j++ )
			{
				int * rows_setup = setup_rows( test_grid, indexes[j].first, indexes[j].second );
                int send_length = 2 * DIMENSIONS;

				MPI_Send( rows_setup, send_length, MPI_INT, j, tag1, MPI_COMM_WORLD );

				delete[] rows_setup;
				rows_setup = NULL;
			}

			// process master node
            int * two_rows = setup_rows( test_grid, indexes[0].first, indexes[0].second );

			node_process( two_rows, local_grid, rows_per_process );
			delete[] two_rows;
			two_rows = NULL;

			// copy back to global grid
			copy_rows( local_grid, test_grid, 0, rows_per_process - 1, indexes[0].first );

			int * rows_receive = new int[local_grid_size];
			// receive
			for( int j = 1; j < num_procs; j++ )
			{
				MPI_Recv( rows_receive, local_grid_size, MPI_INT, j, tag2, MPI_COMM_WORLD, &stat );
				copy_rows( rows_receive, test_grid, 0, rows_per_process - 1, indexes[j].first );
			}
			delete[] rows_receive;
			rows_receive = NULL;

			print_grid( i, test_grid, DIMENSIONS );
		}
		else
		{
			int send_length = rows_per_process * DIMENSIONS;
			int receive_length = 2 * DIMENSIONS;
			int * rows_receive = new int[receive_length];
			MPI_Recv( rows_receive, receive_length, MPI_INT, 0, tag1, MPI_COMM_WORLD, &stat );

			node_process( rows_receive, local_grid, rows_per_process );

			MPI_Send( local_grid, local_grid_size, MPI_INT, 0, tag2, MPI_COMM_WORLD );
		}
	}

	MPI_Finalize();
}
