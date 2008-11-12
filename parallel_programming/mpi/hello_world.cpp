#include <stdio.h>
#include <mpi.h>

int main( int argc, char  **argv )
{
	int num_procs; // the number of processes in the group
	int ID; // a unique identifier ranging from 0 to (num_procs - 1)

	if( MPI_Init( &argc, &argv ) !=  MPI_SUCCESS )
	{
		// print error message and exit
	}

	MPI_Comm_size( MPI_COMM_WORLD, &num_procs );
	MPI_Comm_rank( MPI_COMM_WORLD, &ID );
	printf( "\n Never miss a good chance to shut up %d \n", ID );

	MPI_Finalize();
}
