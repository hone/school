#include <iostream>
#include <map>
#include <vector>
#include <canny.hpp>
#include <matrix_raii.hpp>
#include <highgui.h>

const int GRAY = 100;
const int THRESH_MIN = 1;
const int THRESH_MAX = 30;
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

ImageRAII canny( IplImage * image, std::pair< int, int > thresh, double sigma )
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
	ImageRAII hysteresis_image = hysteresis( suppressed_image.image, orientation.image, thresh );

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, destination.image );
	cvMoveWindow( WINDOW_NAME, image_size.width, 0 );

	return hysteresis_image;
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

			std::pair< CvPoint, CvPoint > positions = get_normal_positions( orientation, i, j );
			CvScalar e = suppress( s, positions, strength );
			cvSet2D( suppressed_image.image, i, j, e );
		}
	}

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, suppressed_image.image );
	cvMoveWindow( WINDOW_NAME, image_size.width * 2, 0 );

	return suppressed_image;
}

std::pair< CvPoint, CvPoint > get_normal_positions( IplImage * orientation, int x, int y )
{
	double o = cvGet2D( orientation, x, y ).val[0];
	std::pair< CvPoint, CvPoint > positions;

	if( o == YELLOW_VALUE )
	{
		positions.first = cvPoint( x, y + 1 );
		positions.second = cvPoint( x, y - 1 );
	}
	else if( o == GREEN_VALUE )
	{
		positions.first = cvPoint( x + 1, y - 1 );
		positions.second = cvPoint( x - 1, y + 1 );
	}
	else if( o == BLUE_VALUE )
	{
		positions.first = cvPoint( x + 1, y );
		positions.second = cvPoint( x - 1, y );
	}
	else if( o == RED_VALUE )
	{
		positions.first = cvPoint( x + 1, y + 1 );
		positions.second = cvPoint( x - 1, y - 1 );
	}
	// should not get here
	else
	{
		std::cerr << "Should not have an orientation of this value. Hsould be tween 0 to 180.: " << o;
		exit( -1 );
	}

	return positions;
}

CvScalar suppress( double s, std::pair< CvPoint, CvPoint > positions, IplImage * strength )
{
	std::pair< double, double > values;
	CvScalar e;

	if( check_boundaries( strength, positions.first ) )
		values.first = cvGet2D( strength, positions.first.x, positions.first.y ).val[0];
	if( check_boundaries( strength, positions.second ) )
		values.second = cvGet2D( strength, positions.second.x, positions.second.y ).val[0];

	if( s > values.first && s > values.second )
	{
		e.val[0] = s;
	}
	else
	{
		e.val[0] = 0;
	}

	return e;
}

struct classcomp
{
	bool operator()( const CvPoint& lhs, const CvPoint& rhs ) const
	{
		if( lhs.x < rhs.x )
			return true;
		else if( lhs.x == rhs.x && lhs.y < rhs.y )
			return true;
		else
			return false;
	}
};

ImageRAII hysteresis( IplImage * image, IplImage * orientation, std::pair< int, int > thresh )
{
	const char * WINDOW_NAME = "Hysteresis Threshold";

	CvSize image_size = cvGetSize( image );
	ImageRAII hysteresis_image( cvCreateImage( image_size, image->depth, image->nChannels ) );
	// key: pixel position
	// value: visited = true, unvisited = false
	std::map< CvPoint, bool, classcomp > pixels;
	std::map< CvPoint, bool, classcomp >::iterator it;
	std::vector< std::vector< CvPoint > > edges;

	// initialize map
	for( int i = 0; i < image_size.width; i++ )
	{
		for( int j = 0; j < image_size.height; j++ )
		{
			pixels[cvPoint( i, j )] = false;
		}
	}

	// visit all pixels
	for( it = pixels.begin(); it != pixels.end(); it++ )
	{
		std::vector< CvPoint > edge;
		// find next unvisited edge pixel
		bool run = true;
		while( run  )
		{
			if( it->second == false && check_boundaries( image, it->first ) && cvGet2D( image, it->first.x, it->first.y ).val[0] > thresh.second  )
				run = false;
			if( it == pixels.end() )
				run = false;
			it++;
		}

		// mark pixel as visited
		CvPoint current_pixel = it->first;
		it->second = true;
		edge.push_back( current_pixel );

		// follow links forward
		std::pair< CvPoint, CvPoint > positions = get_edge_positions( orientation, current_pixel.x, current_pixel.y );

		// go forward
		CvPoint forward = positions.first;
		while( check_boundaries( image, forward ) && cvGet2D( image, forward.x, forward.y ).val[0] > thresh.first )
		{
			// mark pixel as visited
			edge.push_back( forward );
			pixels.find( forward )->second = true;
			
			std::pair< CvPoint, CvPoint > forward_positions = get_edge_positions( orientation, forward.x, forward.y );
			forward = forward_positions.first;
		}

		// go backward
		CvPoint backward = positions.second;
		while( check_boundaries( image, backward ) && cvGet2D( image, backward.x, backward.y ).val[0] > thresh.first )
		{
			// mark pixel as visited
			edge.push_back( backward );
			pixels.find( backward )->second = true;

			std::pair< CvPoint, CvPoint > backward_positions = get_edge_positions( orientation, backward.x, backward.y );
			backward = backward_positions.second;
		}

		// store this edge
		edges.push_back( edge );
	}

	int size = 0;
	// set the edges in the image
	std::vector< std::vector< CvPoint > >::iterator edges_iterator;
	for( edges_iterator = edges.begin(); edges_iterator < edges.end(); edges_iterator++ )
	{
		std::vector< CvPoint >::iterator edge_iterator;
		std::vector< CvPoint > edge = *edges_iterator;
		for( edge_iterator = edge.begin(); edge_iterator < edge.end(); edge_iterator++ )
		{
			size++;
			CvPoint pixel = *edge_iterator;
			CvScalar e;
			e.val[0] = GRAY;
			cvSet2D( hysteresis_image.image, pixel.x, pixel.y, e );
		}
	}

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, hysteresis_image.image );
	cvMoveWindow( WINDOW_NAME, image_size.width * 3, 0 );

	return hysteresis_image;
}

bool check_boundaries( IplImage * image, CvPoint position )
{
	CvSize image_size = cvGetSize( image );
	return(  position.x >= 0 && position.x < image_size.width && position.y >= 0 && position.y < image_size.height );
}

std::pair< CvPoint, CvPoint > get_edge_positions( IplImage * orientation, int x, int y )
{
	double o = cvGet2D( orientation, x, y ).val[0];
	std::pair< CvPoint, CvPoint > positions;

	if( o == YELLOW_VALUE )
	{
		positions.first = cvPoint( x + 1, y );
		positions.second = cvPoint( x - 1, y );
	}
	else if( o == GREEN_VALUE )
	{
		positions.first = cvPoint( x + 1, y + 1 );
		positions.second = cvPoint( x - 1, y - 1 );
	}
	else if( o == BLUE_VALUE )
	{
		positions.first = cvPoint( x, y + 1 );
		positions.second = cvPoint( x, y - 1 );
	}
	else if( o == RED_VALUE )
	{
		positions.first = cvPoint( x + 1, y - 1 );
		positions.second = cvPoint( x - 1, y + 1 );
	}
	// should not get here
	else
	{
		std::cerr << "Should not have an orientation of this value. Hsould be tween 0 to 180.: " << o << std::endl;
		exit( -1 );
	}

	return positions;
}

int main( int argc, char * argv[] )
{
	const char * WINDOW_NAME = "Original Image";

	// x is low
	// y is high
	std::pair< int, int > thresh = std::make_pair( THRESH_MIN, THRESH_MAX);

	if( argc < 2 )
	{
		std::cerr << "Not enough parameters.\n";
		exit( -1 );
	}
	ImageRAII image( argv[1] );

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, image.image );
	cvMoveWindow( WINDOW_NAME, 0, 0 );

	ImageRAII edge_detection_image = canny( image.image, thresh, SIGMA );

	cvWaitKey( 0 );
	return 0;
}
