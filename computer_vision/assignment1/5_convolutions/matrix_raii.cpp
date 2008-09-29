#include "matrix_raii.h"
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

MatrixRAII::~MatrixRAII()
{
    cvReleaseMat( &this->matrix );
}
