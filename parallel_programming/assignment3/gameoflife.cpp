#include <mpi.h>
#include <iostream>

const int DIMENSIONS = 16;
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

// prints the grid
void print_grid( int iteration, int * gloabl_grid )
{
	std::cout << "Iteration " << iteration << ":\n";

	for( int y = 0; y < DIMENSIONS; y++ )
	{
		for( int x = 0; x < DIMENSIONS; x++ )
		{
			std::cout << global_grid[ x + ( y * DIMENSIONS ) ] << " ";
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
	}

	MPI_Finalize();

	delete[] buffer;
}
