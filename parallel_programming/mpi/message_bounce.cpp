#include <stdio.h> // standard I/O include file
#include <stdlib.h> // standard include file with function prototypes for memory management
#include <mpi.h> // MPI include file

int main( int argc, char ** argv )
{
	// message tags
	int Tag1 = 1;
	int Tag2 = 2;
	int num_procs; // the number of processes in the group
	int ID; // a unique identifier ranging from 0 to (num_procs - 1)
	int buffer_count = 100; // number of items in message to bounce
	long *buffer; // buffer to bounce between processes
	MPI_Status stat; // MPI status parameter

	MPI_Init( &argc, &argv ); // Initialize MPI environment
	MPI_Comm_rank( MPI_COMM_WORLD, &ID ); // Find Rank of this process in the group
	MPI_Comm_size( MPI_COMM_WORLD, &num_procs ); // Find number of processes in the group

	if( num_procs != 2 )
		MPI_Abort( MPI_COMM_WORLD, 1 );

	buffer = ( long * )malloc( buffer_count* sizeof( long ) );

	for( int i = 0; i < buffer_count; i++ ) // fill buffer with some values
		buffer[i] = ( long ) i;

	if( ID == 0 )
	{
		MPI_Send( buffer, buffer_count, MPI_LONG, 1, Tag1, MPI_COMM_WORLD );
		MPI_Recv( buffer, buffer_count, MPI_LONG, 1, Tag2, MPI_COMM_WORLD, &stat );
	}
	else
	{
		MPI_Recv( buffer, buffer_count, MPI_LONG, 0, Tag1, MPI_COMM_WORLD, &stat );
		MPI_Send( buffer, buffer_count, MPI_LONG, 0, Tag2, MPI_COMM_WORLD );
	}

	MPI_Finalize();
}
