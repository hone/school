#include <iostream>
#include <canny.hpp>
#include <matrix_raii.hpp>
#include <highgui.h>

const int GAUSSIAN_X = 5;
const int GAUSSIAN_Y = 5;
const float SIGMA = 1.0;

const float YELLOW_VALUE = 0;
const float GREEN_VALUE = 45;
const float BLUE_VALUE = 90;
const float RED_VALUE = 135;

const float YELLOW1_MIN = 0;
const float YELLOW1_MAX = 22.5;
const float GREEN_MIN = 22.5;
const float GREEN_MAX = 67.5;
const float BLUE_MIN = 67.5;
const float BLUE_MAX = 112.5;
const float RED_MIN = 112.5;
const float RED_MAX = 157.5;
const float YELLOW2_MIN = 157.5;
const float YELLOW2_MAX = 180;

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
	cvSobel( gaussian.image, gradient_x.image, 1, 0, 3 );
	cvSobel( gaussian.image, gradient_y.image, 0, 1, 3 );
	// find edge orientation
	CvSize image_size = cvGetSize( gaussian.image );

	for( int i = 0; i < image_size.width; i++ )
	{
		for( int j = 0; j < image_size.height; j++ )
		{
			double x = cvGet2D( gradient_x.image, i, j ).val[0];
			double y = cvGet2D( gradient_y.image, i, j ).val[0];
			float angle;

			if( x == 0 )
			{
				if( y == 0 )
					angle = 0;
				else
					angle = 90;
			}
			else
				angle = cvFastArctan( y, x );

			CvScalar g;
			CvScalar a;
		   	g.val[0] = cvSqrt( pow( x, 2 ) + pow( y, 2 ) );
			a.val[0] = find_angle( angle );

			cvSet2D( destination.image, i, j, g );
			cvSet2D( orientation.image, i, j, a );
		}
	}

	ImageRAII suppressed_image = nonMaxSup( destination.image, orientation.image );

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, destination.image );
	cvMoveWindow( WINDOW_NAME, image_size.width, 0 );

	return destination;
}

float find_angle( float angle )
{
	float adjusted_angle = 0;

	if( ( angle >= YELLOW1_MIN && angle < YELLOW1_MAX ) || ( angle >= YELLOW2_MIN && angle <= YELLOW2_MAX ) )
	{
		adjusted_angle = YELLOW_VALUE;
	}
	else if( angle >= GREEN_MIN && angle < GREEN_MAX )
	{
		adjusted_angle = GREEN_VALUE;
	}
	else if( angle >= BLUE_MIN && angle < BLUE_MAX )
	{
		adjusted_angle = BLUE_VALUE;
	}
	else if( angle >= RED_MIN && angle < RED_MAX )
	{
		adjusted_angle = RED_VALUE;
	}
	// shouldn't happen
	else
	{
		std::cerr << "Angle must be in the range of 0 to 180 degrees.: " << angle << std::endl;
		exit( -1 );
	}

	return adjusted_angle;
}

ImageRAII nonMaxSup( IplImage * strength, IplImage * orientation )
{
	const char * WINDOW_NAME = "Non-maximum Suppression";

	CvSize image_size = cvGetSize( strength );
	ImageRAII suppressed_image( cvCreateImage( image_size, strength->depth, strength->nChannels ) );

	for( int i = 0; i < image_size.width; i++ )
	{
		for( int j = 0; j < image_size.height; j++ )
		{
			double s = cvGet2D( strength, i, j ).val[0];
			double o = cvGet2D( orientation, i, j ).val[0];

			CvScalar e;
			CvPoint position1, position2;
			if( o == YELLOW_VALUE )
			{
				position1 = cvPoint( i, j + 1 );
				position2 = cvPoint( i, j - 1 );
			}
			else if( o == GREEN_VALUE )
			{
				position1 = cvPoint( i + 1, j - 1 );
				position2 = cvPoint( i - 1, j + 1 );
			}
			else if( o == BLUE_VALUE )
			{
				position1 = cvPoint( i + 1, j );
				position2 = cvPoint( i - 1, j );
			}
			else if( o == RED_VALUE )
			{
				position1 = cvPoint( i + 1, j + 1 );
				position2 = cvPoint( i - 1, j - 1 );
			}
			// should not get here
			else
			{
				std::cerr << "Should not have an orientation of this value. Hsould be tween 0 to 180.: " << o;
				exit( -1 );
			}

			e = suppress( s, position1, position2, strength, image_size );
			cvSet2D( suppressed_image.image, i, j, e );
		}
	}

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, suppressed_image.image );
	cvMoveWindow( WINDOW_NAME, image_size.width * 2, 0 );

	return suppressed_image;
}

CvScalar suppress( double s, CvPoint position1, CvPoint position2, IplImage * strength, CvSize image_size )
{
	double value1, value2;
	CvScalar e;

	if( position1.x >= 0 && position1.x < image_size.width && position1.y >= 0 && position1.y < image_size.height )
		value1 = cvGet2D( strength, position1.x, position1.y ).val[0];
	if( position2.y >= 0 && position2.x >= image_size.width && position2.y >= 0 && position2.y < image_size.height )
		value2 = cvGet2D( strength, position2.x, position2.y ).val[0];

	if( s > value1 && s > value2 )
	{
		e.val[0] = s;
	}
	else
	{
		e.val[0] = 0;
	}

	return e;
}

void hysteresis( IplImage * image )
{
}

int main( int argc, char * argv[] )
{
	const char * WINDOW_NAME = "Original Image";

	MatrixRAII thresh( cvCreateMat( 2, 1, CV_32FC1 ) );
	cvmSet( thresh.matrix, 0, 0, 1 );
	cvmSet( thresh.matrix, 1, 0, 255 );

	if( argc < 2 )
	{
		std::cerr << "Not enough parameters.\n";
		exit( -1 );
	}
	ImageRAII image( argv[1] );

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, image.image );
	cvMoveWindow( WINDOW_NAME, 0, 0 );

	ImageRAII edge_detection_image = canny( image.image, thresh.matrix, SIGMA );

	cvWaitKey( 0 );
	return 0;
}
