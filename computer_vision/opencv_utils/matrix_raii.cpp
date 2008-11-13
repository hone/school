#include <matrix_raii.hpp>
#include <stdio.h>

MatrixRAII::MatrixRAII()
{
    this->matrix = NULL;
}

MatrixRAII::MatrixRAII( CvMat * mat )
{
    this->matrix = mat;

    if( !matrix )
    {
        printf( "Could not create matrix." );
        exit( 0 );
    }
}

MatrixRAII::MatrixRAII( const MatrixRAII &mat )
{
	this->matrix = cvCloneMat( mat.matrix );
	
	if( !matrix )
	{
        printf( "Could not create matrix." );
        exit( 0 );
	}
}

MatrixRAII::~MatrixRAII()
{
    cvReleaseMat( &this->matrix );
}
