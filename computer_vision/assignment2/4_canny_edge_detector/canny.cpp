#include <image_raii.hpp>
#include <matrix_raii.hpp>
#include <iostream>
#include <cv.h>
#include <highgui.h>

const int GAUSSIAN_X = 5;
const int GAUSSIAN_Y = 5;
const float SIGMA = 1.0;
const int QUIT_KEY_CODE = 113;

/**
 * Apply canny detection algorithm. This function handles the enhancement and calls the nonmaxsuppresion and hysteresis.
 */
ImageRAII canny( IplImage * image, CvMat * thresh, double sigma )
{
	const char * WINDOW_NAME = "Basic Canny Edge Detector";

	ImageRAII grayscale( cvCreateImage( cvGetSize( image ), image->depth, 1 ) );
	ImageRAII destination( cvCreateImage( cvGetSize( image ), image->depth, grayscale.image->nChannels ) );
	ImageRAII gaussian( cvCreateImage( cvGetSize( image ), image->depth, grayscale.image->nChannels ) );
	ImageRAII gradient_x( cvCreateImage( cvGetSize( image ), image->depth, grayscale.image->nChannels ) );
	ImageRAII gradient_y( cvCreateImage( cvGetSize( image ), image->depth, grayscale.image->nChannels ) );
	ImageRAII gradient( cvCreateImage( cvGetSize( image ), image->depth, grayscale.image->nChannels ) );
	ImageRAII orientation( cvCreateImage( cvGetSize( image ), image->depth, grayscale.image->nChannels ) );
	MatrixRAII sobel_x( cvCreateMat( 3, 3, CV_64FC1 ) );
	MatrixRAII sobel_y( cvCreateMat( 3, 3, CV_64FC1 ) );
	double sobel_x_data[3][3] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
	double sobel_y_data[3][3] = { 1, 2, 1, 0, 0, 0, -1, -2, 1 };

	// setup sobel operator
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			cvmSet( sobel_x.matrix, i, j, sobel_x_data[i][j] );
			cvmSet( sobel_y.matrix, i, j, sobel_y_data[i][j] );
		}
	}

	// convert image to grayscale
	cvCvtColor( image, grayscale.image, CV_BGR2GRAY );

	// gaussian smoothing
	cvSmooth( grayscale.image, gaussian.image, CV_GAUSSIAN, GAUSSIAN_X, GAUSSIAN_Y, sigma );
	// find edge strength
	cvFilter2D( gaussian.image, gradient_x.image, sobel_x.matrix );
	cvFilter2D( gaussian.image, gradient_y.image, sobel_y.matrix );
	// find edge orientation
	char * source_x_buffer = gradient_x.image->imageData;
	char * source_y_buffer = gradient_y.image->imageData;
	char * destination_buffer = destination.image->imageData;
	char * orientation_buffer = orientation.image->imageData;
	CvSize image_size = cvGetSize( gaussian.image );

	for( int i = 0; i < image_size.width; i++ )
	{
		for( int j = 0; j < image_size.height; j++ )
		{
			char x = *( source_x_buffer + i * gradient_x.image->widthStep + gradient_x.image->nChannels * j );
			char y = *( source_y_buffer + i * gradient_y.image->widthStep + gradient_y.image->nChannels * j );
			*( destination_buffer + i * destination.image->widthStep + destination.image->nChannels * j ) = sqrt( pow( x, 2 ) + ( y, 2 ) );
			*( orientation_buffer + i * orientation.image->widthStep + orientation.image->nChannels * j ) = cvFastArctan( y, x );
		}
	}

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, destination.image );

	int key_code = 0;
	while( key_code != QUIT_KEY_CODE )
	{
		key_code = cvWaitKey( 0 );
	}

	return destination;
}

void nonMaxSup( IplImage * image )
{
}

void hysteresis( IplImage * image )
{
}

int main( int argc, char * argv[] )
{
	MatrixRAII thresh( cvCreateMat( 2, 1, CV_32FC1 ) );
	cvmSet( thresh.matrix, 0, 0, 1 );
	cvmSet( thresh.matrix, 1, 0, 255 );

	if( argc < 2 )
	{
		std::cerr << "Not enough parameters.\n";
		exit( -1 );
	}
	ImageRAII image( argv[1] );

	ImageRAII edge_detection_image = canny( image.image, thresh.matrix, SIGMA );

	return 0;
}
