#include <iostream>
#include <string.h>
#include <gameoflife.hpp>

// static membes
int Life::global_grid[LENGTH] = {
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

Life::Life( int p_id, int p_num_procs )
{
	this->id = p_id;
	this->num_procs = p_num_procs;
	this->tag1 = 1;
	this->tag2 = 2;
	this->rows_per_process = DIMENSIONS / num_procs;
    this->local_grid = new int[rows_per_process * DIMENSIONS];
    this->local_grid_size = rows_per_process * DIMENSIONS;
	this->two_row_size = 2 * DIMENSIONS;
	this->calculate_indexes();
}

Life::~Life()
{
	delete[] this->local_grid;
	this->local_grid = NULL;
}

void Life::print_grid( int iteration, int * grid, int height )
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

int Life::offset( int x, int y, bool bound )
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

int Life::count_alive_neighbors( int x, int y, int * rows )
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

bool Life::check_neighborhood( int x, int y, int * rows )
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

int * Life::process_rows( int * rows )
{
	// don't process first and last rows (only need for neighbor counting)
	for( int i = 1; i <= this->rows_per_process; i++ )
	{
		for( int x = 0; x < DIMENSIONS; x++ )
		{
			// rows x by y
			// local_grid is x by (y-2), offset is 1 from top and bottom
			if( check_neighborhood( x, i, rows ) )
				this->local_grid[offset( x, i - 1 )] = ALIVE;
			else
				this->local_grid[offset( x, i - 1 )] = DEAD;
		}
	}
}

int * Life::grid_sums( int * rows )
{
	int * new_rows = new int[rows_per_process * DIMENSIONS];

	// don't process first and last rows (only need for neighbor counting)
	for( int i = 1; i <= this->rows_per_process; i++ )
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
int * Life::setup_rows( int * rows, int start_index, int end_index )
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

void Life::join_rows( int * two_rows, int * own_rows, int * destination )
{
    for( int i = 0; i < DIMENSIONS; i++ )
    {
        destination[offset( i, 0 )] = two_rows[offset( i, 0 )];
        destination[offset( i, this->rows_per_process + 1, false )] = two_rows[offset( i, 1 )];

        for( int j = 1; j <= this->rows_per_process; j++ )
            destination[offset( i, j )] = own_rows[offset( i, j - 1 )];
    }
}

void Life::copy_rows( int * source, int * destination, int source_start_index, int source_end_index, int destination_start_index )
{
	int length = source_end_index - source_start_index + 1;
	for( int j = 0; j < length; j++ )
		for( int i = 0; i < DIMENSIONS; i++ )
			destination[offset( i, j + destination_start_index )] = source[offset( i, j + source_start_index )];
}

void Life::calculate_indexes()
{
    int start_index = 0;
    int end_index = this->rows_per_process - 1;
    this->indexes.push_back( std::make_pair( start_index, end_index ) );

    for( int j = 1; j < num_procs; j++ )
    {
        start_index = end_index + 1;
        end_index = start_index + this->rows_per_process - 1;
        this->indexes.push_back( std::make_pair( start_index, end_index ) );
    }
}

void Life::setup_local_grid()
{
      copy_rows( global_grid, this->local_grid, this->indexes[0].first, this->indexes[0].second, 0 );
      int * rows_setup = new int[this->local_grid_size];

      for( int j = 1; j < this->num_procs; j++ )
      {
          copy_rows( global_grid, rows_setup, indexes[j].first, indexes[j].second, 0 );
          MPI_Send( rows_setup, this->local_grid_size, MPI_INT, j, this->tag1, MPI_COMM_WORLD );
      }

      delete[] rows_setup;
	  rows_setup = NULL;
}

void Life::node_process( int * two_rows )
{
	int * rows = new int[this->local_grid_size + this->two_row_size];

	join_rows( two_rows, this->local_grid, rows );
	process_rows( rows );
	delete[] rows;
	rows = NULL;
}

void Life::process_master_node()
{
	int * two_rows = setup_rows( global_grid, this->indexes[0].first, this->indexes[0].second );

	node_process( two_rows );
	delete[] two_rows;
	two_rows = NULL;

	// copy back to global grid
	copy_rows( this->local_grid, global_grid, 0, this->rows_per_process - 1, this->indexes[0].first );
}

void Life::run_setup()
{
	if( this->id == 0 )
    {
		print_grid( 0, global_grid );

        setup_local_grid();
    }
    else
    {
        MPI_Recv( local_grid, local_grid_size, MPI_INT, 0, tag1, MPI_COMM_WORLD, &stat );
    }
}

void Life::send_to_slave_nodes()
{
	for( int j = 1; j < num_procs; j++ )
	{
		int * rows_setup = setup_rows( global_grid, this->indexes[j].first, this->indexes[j].second );

		MPI_Send( rows_setup, this->two_row_size, MPI_INT, j, this->tag1, MPI_COMM_WORLD );

		delete[] rows_setup;
		rows_setup = NULL;
	}
}

void Life::receive_from_slave_nodes()
{
	int * rows_receive = new int[this->local_grid_size];
	// receive
	for( int j = 1; j < this->num_procs; j++ )
	{
		MPI_Recv( rows_receive, this->local_grid_size, MPI_INT, j, this->tag2, MPI_COMM_WORLD, &this->stat );
		copy_rows( rows_receive, global_grid, 0, this->rows_per_process - 1, this->indexes[j].first );
	}
	delete[] rows_receive;
	rows_receive = NULL;
}

void Life::slave_node_iterate()
{
	int * rows_receive = new int[this->two_row_size];
	MPI_Recv( rows_receive, this->two_row_size, MPI_INT, 0, this->tag1, MPI_COMM_WORLD, &this->stat );

	node_process( rows_receive );

	MPI_Send( this->local_grid, this->local_grid_size, MPI_INT, 0, this->tag2, MPI_COMM_WORLD );
}

void Life::run( int iterations )
{
    // need special prep before the first iteration
	run_setup();

    // do the iterations
	for( int i = 1; i <= iterations; i++ )
	{
		if( this->id == 0 )
		{
			send_to_slave_nodes();
			process_master_node();
			receive_from_slave_nodes();

			print_grid( i, global_grid );
		}
		else
		{
			slave_node_iterate();
		}
	}
}

int main( int argc, char ** argv )
{
	const int ITERATIONS = 64;
	int num_procs = 0; // number of processes
	int id; // process (or node) id
	MPI_Status stat; // MPI status parameter

	MPI_Init( & argc, &argv ); // initialize MPI environment
	MPI_Comm_rank( MPI_COMM_WORLD, &id ); // find rank of this process in the group
	MPI_Comm_size( MPI_COMM_WORLD, &num_procs ); // find number of processes in the group

	Life life( id, num_procs );
	life.run( ITERATIONS );

	MPI_Finalize();
}

