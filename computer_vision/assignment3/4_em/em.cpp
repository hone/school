#include <iostream>
#include <em.hpp>
#include <image_raii.hpp>
#include <vector>

void em( CvMat * data, int nClusters, int nTries, CvSize image_size )
{
	random_point( image_size );
}

MatrixRAII convert_data( IplImage * image )
{
	CvSize image_size = cvGetSize( image );
	int max_cols = image_size.width * image_size.height;
	MatrixRAII data( cvCreateMat( 2, max_cols * 2, CV_32FC1 ) );

	for( int x = 0; x < image->width; x++ )
	{
		for( int y = 0; y < image->height; y++ )
		{
			// green
			cvmSet( data.matrix, 0, y + x, cvGet2D( image, y, x ).val[1] / 255.0 );
			// red
			cvmSet( data.matrix, 1, y + x + max_cols, cvGet2D( image, y, x ).val[2] / 255.0 );
		}
	}

	return data;
}

CvPoint random_point( CvSize image_size )
{
	time_t t;
	t = time( NULL );
	srand( t );

	int x = (int)( ( rand() * 1.0 / RAND_MAX ) * image_size.width );
	int y = (int)( ( rand() * 1.0 / RAND_MAX ) * image_size.height );

	return cvPoint( x, y );
}

int main( int argc, char * argv[] )
{
	if( argc < 4 )
	{
		std::cerr << "ERROR: Need to pass in the image, number of clusters, and number of restarts\n";
		std::cerr << "Usage: em <image> <# of clusters> <# of restarts>\n";
	}
	else
	{
		ImageRAII image( argv[1] );
		CvSize image_size = cvGetSize( image.image );
		MatrixRAII data = convert_data( image.image );
		em( data.matrix, 2, 1, image_size );
	}

	return 0;
}
