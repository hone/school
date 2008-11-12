#include <iostream>
#include <em.hpp>
#include <image_raii.hpp>
#include <window_raii.hpp>
#include <vector>
#include <iostream>

const int GRAPH_WIDTH = 100;
const int GRAPH_HEIGHT = 100;
const int RED = 0;
const int GREEN = 1;

void em( CvMat * data, int nClusters, int nTries, CvSize image_size )
{
	std::vector<CvPoint> starting_points;
	std::vector<CvPoint> means;

	display_data( data );
	
	for( int i = 0; i < nClusters; i++ )
	{
		CvPoint point = random_point( image_size );
		starting_points.push_back( point );
		means.push_back( point );
	}
}

MatrixRAII convert_data( IplImage * image )
{
	CvSize image_size = cvGetSize( image );
	int max_cols = image_size.width * image_size.height;
	MatrixRAII data( cvCreateMat( 2, max_cols, CV_32FC1 ) );

	for( int x = 0; x < image->width; x++ )
	{
		for( int y = 0; y < image->height; y++ )
		{
			// red
			cvmSet( data.matrix, RED, ( y * image->width ) + x, cvGet2D( image, y, x ).val[2] / 255.0 );
			// green
			cvmSet( data.matrix, GREEN, ( y * image->width ) + x, cvGet2D( image, y, x ).val[1] / 255.0 );
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

void display_data( CvMat * data )
{
	ImageRAII graph( cvCreateImage( cvSize( 101, 101 ), IPL_DEPTH_8U, 1 ) );

	for( int i = 0; i < graph.image->width; i++ )
	{
		for( int j = 0; j < graph.image->height; j++ )
		{
			CvScalar white;
			white.val[0] = 255;
			cvSet2D( graph.image, j, i, white );
		}
	}

	for( int i = 0; i < data->width; i++ )
	{
		CvScalar black;
		black.val[0] = 0;
		int green = cvmGet( data, GREEN, i ) * 100;
		int red = cvmGet( data, RED, i ) * 100;
		std::cout << "Green: " << green << std::endl;
		std::cout << "Red: " << red << std::endl;
		cvSet2D( graph.image, 100 - red, green, black );
	}

	WindowRAII window( "Graph" );
	cvShowImage( window.name, graph.image );
	cvWaitKey( 0 );
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
