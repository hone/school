#include <image_raii.hpp>
#include <matrix_raii.hpp>
#include <cv.h>
#include <highgui.h>

int main( int argc, char * argv[] )
{
	MatrixRAII x( cvCreateMat( 3, 1, CV_8UC1 ) );
	int *data = x.matrix->data.i;
}
