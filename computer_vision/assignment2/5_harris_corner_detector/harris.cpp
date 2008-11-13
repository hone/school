#include <iostream>
#include <cv.h>
#include <harris.hpp>

const int GAUSSIAN_X = 5;
const int GAUSSIAN_Y = 5;
const int K = 0.04;
const int BOX_SIZE = 2;
const float SIGMA = 1.0;
const float LARGE_VALUE = 100;

ImageRAII detectCorner( IplImage * image, int win, float sigma )
{
	CvSize image_size = cvGetSize( image );
	ImageRAII grayscale_image( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII corner_image( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_x( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_y( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_xy( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_x2( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_y2( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_xy_gaussian( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_x2_gaussian( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_y2_gaussian( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_x2y2( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_xy2( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_x2_y2( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_x2_y2_2( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII image_x2y2_xy2_diff( cvCreateImage( image_size, image->depth, 1 ) );
	ImageRAII final_image( cvCreateImage( image_size, image->depth, image->nChannels ) );

	// convert image to grayscale
	cvCvtColor( image, grayscale_image.image, CV_BGR2GRAY );

	// generate image derivatives
	cvSobel( grayscale_image.image, image_x.image, 1, 0 );
	cvSobel( grayscale_image.image, image_y.image, 0, 1 );

	// smoothed squared image derivatives
	cvMul( image_x.image, image_x.image, image_x2.image );
	cvMul( image_y.image, image_y.image, image_y2.image );
	cvMul( image_x.image, image_y.image, image_xy.image );
	cvSmooth( image_x2.image, image_x2_gaussian.image, CV_GAUSSIAN, GAUSSIAN_X, GAUSSIAN_Y, sigma );
	cvSmooth( image_y2.image, image_y2_gaussian.image, CV_GAUSSIAN, GAUSSIAN_X, GAUSSIAN_Y, sigma );
	cvSmooth( image_xy.image, image_xy_gaussian.image, CV_GAUSSIAN, GAUSSIAN_X, GAUSSIAN_Y, sigma );

	// compute the Harris corner, A
	cvMul( image_x2_gaussian.image, image_y2_gaussian.image, image_x2y2.image );
	cvMul( image_xy_gaussian.image, image_xy_gaussian.image, image_xy2.image );
	cvAdd( image_x2_gaussian.image, image_y2_gaussian.image, image_x2_y2.image );
	cvMul( image_x2_y2.image, image_x2_y2.image, image_x2_y2_2.image, K );
	cvSub( image_x2y2.image, image_xy2.image, image_x2y2_xy2_diff.image );
	cvSub( image_x2y2_xy2_diff.image, image_x2_y2_2.image, corner_image.image );

	// go through corner image, looking for high values in final_image
	CvScalar red;
	red.val[2] = 255;
	for( int i = 0; i < image_size.width; i++ )
	{
		for( int j = 0; j < image_size.height; j++ )
		{
			if( cvGet2D( corner_image.image, j, i ).val[0] > LARGE_VALUE )
				cvSet2D( final_image.image, j, i, red );
			else
				cvSet2D( final_image.image, j, i, cvGet2D( image, j, i ) );
		}
	}

	return final_image;
}

ImageRAII makePyramid( IplImage * image, float sigma )
{
	CvSize image_size = cvGetSize( image );
	CvSize scaled_down_size;
	scaled_down_size.width = image_size.width / 2;
	scaled_down_size.height = image_size.height / 2;
	ImageRAII smoothed( cvCreateImage( image_size, image->depth, image->nChannels ) );
	ImageRAII scaled_down( cvCreateImage( scaled_down_size, image->depth, image->nChannels ) );

	cvSmooth( image, smoothed.image, CV_GAUSSIAN, GAUSSIAN_X, GAUSSIAN_Y, sigma );
	cvResize( smoothed.image, scaled_down.image );

	return scaled_down;
}

ImageRAII localMaxSup( std::vector< IplImage * >pyrCor, int scale )
{
	return NULL;
}

int main( int argc, char * argv[] )
{
	const char * WINDOW_NAME = "Harris Corner Detection";

	if( argc < 2 )
	{
		std::cerr << "Need at least 1 argument. harris <image_filename>\n";
		exit( -1 );
	}

	ImageRAII image( argv[1] );
	ImageRAII rgb_image( cvCreateImage( cvGetSize( image.image ), image.image->depth, 3 ) );
	cvCvtColor( image.image, rgb_image.image, CV_HLS2RGB );
	ImageRAII corners = detectCorner( rgb_image.image, BOX_SIZE, SIGMA );

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, corners.image );

	cvWaitKey( 0 );
	
	return 0;
}
