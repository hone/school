#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
#include "image_raii.h"
#include "matrix_raii.h"
#include <cv.h>
#include <highgui.h>

/**
 * constructs a box filter. a matrix with all 1s and is normalized.
 * @param size of box filter as an int.
 */
MatrixRAII makeBoxFilter( int w )
{
	int array_size = w * w;
	double one_over_w_squared = 1.0 / array_size;
	MatrixRAII mat( cvCreateMat( w, w, CV_64FC1 ) );
	
	// set matrix values
	for( int i = 0; i < w; i++ )
	{
		for( int j = 0; j < w; j++ )
		{
			cvmSet( mat.matrix, i, j, one_over_w_squared );
		}
	}

	return mat;
}

/**
 * constructs a 1D Gaussian filter.
 * @param sigma standard deviation parameter.  width of filter = 6 * sigma
 */
MatrixRAII make1DGaussianFilter( double sigma )
{
	int columns =  5 * sigma;
	int max_column = columns / 2;
	MatrixRAII mat( cvCreateMat( 1, columns, CV_64FC1 ) );
	double initial_gaussian[columns];

	// generate the float gaussian
	for( int i = 0; i < columns; i++ )
	{
		int corrected_position = i - max_column;
		double base_fraction = ( 1 / ( sqrt( 2 * M_PI ) * sigma ) );
		double exponent_fraction = ( -1 * pow( corrected_position, 2 ) ) / ( 2 * pow( sigma, 2 ) );
		double double_gaussian = base_fraction * pow( M_E, exponent_fraction );
		initial_gaussian[i] = double_gaussian;
	}

	// get the minimum/set value
	double minimum = *std::min_element( initial_gaussian, initial_gaussian + columns );
	for( int i = 0; i < columns; i++ )
	{
		initial_gaussian[i] /= minimum;
	}
	
	// calculate sum ( for weighting later )
	int sum = 0;
	for( int i = 0; i < columns; i++ )
	{
		sum += (int)initial_gaussian[i];
	}

	for( int i = 0; i < columns; i++ )
	{
		cvmSet( mat.matrix, 0, i, initial_gaussian[i] / sum );
	}

	return mat;
}

int main( int argc, char * argv[] )
{
	const char * ORIGINAL_IMAGE_WINDOW_NAME = "Original Image";
	const char * BOX_FILTER_WINDOW_NAME = "Box Filter";
	const char * GAUSSIAN_FILTER_WINDOW_NAME = "Gaussian Filter";
	const int QUIT_KEY_CODE = 113;

	int box_filter_width = 3;
	float sigma = 1.0;
	std::string filename = "cameraman.tif";
	ImageRAII original_image( filename );
	CvSize image_dimensions = { original_image.image->width, original_image.image->height };
	ImageRAII box_filter_image( cvCreateImage( image_dimensions, original_image.image->depth, 3 ) );
	ImageRAII gaussian_image( cvCreateImage( image_dimensions, original_image.image->depth, 3 ) );
	MatrixRAII box_filter = makeBoxFilter( box_filter_width );
	MatrixRAII gaussian_filter_x = make1DGaussianFilter( sigma );
	MatrixRAII gaussian_filter_y = cvCreateMat( sigma * 5, 1, CV_64FC1 );
	cvTranspose( gaussian_filter_x.matrix, gaussian_filter_y.matrix );
	std::vector<ImageRAII> original_image_channels( 3 );
	std::vector<ImageRAII> box_filter_channels( 3 );
	std::vector<ImageRAII> gaussian_filter_channels( 3 );
	std::vector<ImageRAII> gaussian_filter_2_channels( 3 );

	// initialize image channel vectors
	for( int i = 0; i < original_image.image->nChannels; i++ )
	{
		original_image_channels[i].image = cvCreateImage( image_dimensions, original_image.image->depth, 1 );
		box_filter_channels[i].image = cvCreateImage( image_dimensions, original_image.image->depth, 1 );
		gaussian_filter_channels[i].image = cvCreateImage( image_dimensions, original_image.image->depth, 1 );
		gaussian_filter_2_channels[i].image = cvCreateImage( image_dimensions, original_image.image->depth, 1 );
	}

	// split image channels
	cvSplit( original_image.image, original_image_channels[0].image, original_image_channels[1].image, original_image_channels[2].image, NULL );

	// apply filters
	for( int i = 0; i < original_image.image->nChannels; i++ )
	{
		cvFilter2D( original_image_channels[i].image, box_filter_channels[i].image, box_filter.matrix );
		cvFilter2D( original_image_channels[i].image, gaussian_filter_channels[i].image, gaussian_filter_x.matrix );
		cvFilter2D( gaussian_filter_channels[i].image, gaussian_filter_2_channels[i].image, gaussian_filter_y.matrix );
	}

	// Merge channels back
	cvMerge( box_filter_channels[0].image, box_filter_channels[1].image, box_filter_channels[2].image, NULL, box_filter_image.image );
	cvMerge( gaussian_filter_2_channels[0].image, gaussian_filter_2_channels[1].image, gaussian_filter_2_channels[2].image, NULL, gaussian_image.image );

	// create windows
	cvNamedWindow( ORIGINAL_IMAGE_WINDOW_NAME, CV_WINDOW_AUTOSIZE );
	cvShowImage( ORIGINAL_IMAGE_WINDOW_NAME, original_image.image );
	cvMoveWindow( ORIGINAL_IMAGE_WINDOW_NAME, 0, 0 );
	cvNamedWindow( BOX_FILTER_WINDOW_NAME, CV_WINDOW_AUTOSIZE );
	cvShowImage( BOX_FILTER_WINDOW_NAME, box_filter_image.image );
	cvMoveWindow( BOX_FILTER_WINDOW_NAME, original_image.image->width, 0 );
	cvNamedWindow( GAUSSIAN_FILTER_WINDOW_NAME, CV_WINDOW_AUTOSIZE );
	cvShowImage( GAUSSIAN_FILTER_WINDOW_NAME, gaussian_image.image );
	cvMoveWindow( GAUSSIAN_FILTER_WINDOW_NAME, original_image.image->width + box_filter_image.image->width, 0 );

	// wait for keyboard input
	int key_code = 0;
	while( key_code != QUIT_KEY_CODE )
	{
		key_code = cvWaitKey( 0 );
	}

	return 0;
}
