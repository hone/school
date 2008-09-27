#include <string>
#include "image_loader.h"
#include <cv.h>
#include <highgui.h>

/**
 * constructs a box filter. a matrix with all 1s and is normalized.
 * @param size of box filter as an int.
 */
CvMat * makeBoxFilter( int w )
{
	int array_size = w * w;
	double one_over_w_squared = 1.0 / array_size;
	CvMat * mat = cvCreateMat( w, w, CV_32FC1 );
	
	// set matrix values
	for( int i = 0; i < w; i++ )
	{
		for( int j = 0; j < w; j++ )
		{
			cvmSet( mat, i, j, one_over_w_squared );
		}
	}

	return mat;
}

int main( int argc, char * argv[] )
{
	const char * WINDOW_NAME = "mainWin";

	// load image
	std::string filename = "cameraman.tif";
	ImageLoader original_image( filename );
	CvSize image_dimensions = { original_image.image->width, original_image.image->height };
	IplImage * grayscale_image = cvCreateImage( image_dimensions, IPL_DEPTH_8U, 1 );
	IplImage * box_filtered_image = cvCreateImage( image_dimensions, IPL_DEPTH_8U, 1 );
	CvMat * box_filter = makeBoxFilter( 3 );

	// Convert image to grayscale
	cvCvtColor( original_image.image, grayscale_image, CV_BGR2GRAY );
	cvFilter2D( grayscale_image, box_filtered_image, box_filter );

	// create window
	cvNamedWindow( WINDOW_NAME, CV_WINDOW_AUTOSIZE );
	cvShowImage( WINDOW_NAME, box_filtered_image );

	// wait for keyboard input
	cvWaitKey( 0 );

	cvReleaseImage( &grayscale_image );
	cvReleaseMat( &box_filter );
	return 0;
}
