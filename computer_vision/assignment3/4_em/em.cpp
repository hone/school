#include <iostream>
#include <em.hpp>
#include <image_raii.hpp>
#include <window_raii.hpp>
#include <iostream>

const int GRAPH_WIDTH = 100;
const int GRAPH_HEIGHT = 100;
const int RED = 0;
const int GREEN = 1;

void em( CvMat * data, int nClusters, int nTries, CvSize image_size )
{
	std::vector<CvMat *> starting_points;
	std::vector<CvMat *> means;
	// 2 x 2 matrix
	std::vector<CvMat *> variances;
	std::vector<double> weights;

	display_data( data );

	MatrixRAII initial_variance( cvCreateMat( 2, 2, CV_32FC1 ) );
	cvmSet( initial_variance.matrix, 0, 0, 1.0 );
	cvmSet( initial_variance.matrix, 0, 1, 0.0 );
	cvmSet( initial_variance.matrix, 1, 0, 0.0 );
	cvmSet( initial_variance.matrix, 1, 1, 1.0 );

	double initial_weight = 1.0 / nClusters;

	// initialize starting values
	for( int i = 0; i < nClusters; i++ )
	{
		CvMat * point = random_point( image_size );
		starting_points.push_back( point );
		means.push_back( point );
		
		// set variance
		variances.push_back( cvCloneMat( initial_variance.matrix ) );

		// weight
		weights.push_back( initial_weight );
	}

	MatrixRAII r = expectation( data, nClusters, means, variances, weights );
}

void expectation( CvMat * data, int nClusters, std::vector<CvMat *> means, std::vector<CvMat *> variances, std::vector<double> weights )
{
	MatrixRAII r( cvCreateMat( nClusters, data->width, CV_32FC1 ) );

	for( int c = 0; c < nClusters; c++ )
	{
		double z = 2 * M_PI * pow( cvDet( variances[c] ), 0.5 );
		// iterate over all pixels
		for( int p = 0; p < data->width; p++ )
		{
			double red = cvmGet( data, RED, p );
			double green = cvmGet( data, GREEN, p );
			MatrixRAII x( cvCreateMat( 2, 1, CV_32FC1 ) );

			cvmSet( r.matrix, c, p, normal_distribution( z, x.matrix, means[c], variances[c] ) * weights[c] );
		}
	}

	for( int p = 0; p < data->width; p++ )
	{
		double sum = 0;

		// sum of the clusters for each pixel
		for( int c = 0; c < nClusters; c++ )
		{
			sum += cvmGet( r.matrix, c, p );
		}

		for( int c = 0; c < nClusters; c++ )
		{
			double r_pk = cvmGet( r.matrix, c, p );
			cvmSet( r.matrix, c, p, r_pk / sum );
		}
	}

	return r;
}

double normal_distribution( double z, CvMat * x, CvMat * mean, CvMat * variance )
{
	MatrixRAII x_minus_mu( cvCreateMat( 2, 1, CV_32FC1 ) );
	MatrixRAII tranpose_x_minus_mu( cvCreateMat( 1, 2, CV_32FC1 ) );
	MatrixRAII x_minus_mu_times_variance( cvCreateMat( 1, 2, CV_32FC1 ) );
	MatrixRAII exponent( cvCreateMat( 1, 1, CV_32FC1 ) );

	cvSub( x,  mean, x_minus_mu.matrix );
	cvTranspose( x_minus_mu.matrix, tranpose_x_minus_mu.matrix );
	cvGEMM( tranpose_x_minus_mu.matrix, variance, -0.5, NULL, 0, x_minus_mu_times_variance.matrix );
	cvMatMul( x_minus_mu_times_variance.matrix, x_minus_mu.matrix, exponent.matrix );

	return ( 1 / z ) * pow( M_E, cvmGet( exponent.matrix, 0, 0 ) );
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

CvMat * random_point( CvSize image_size )
{
	CvMat * point( cvCreateMat( 2, 1, CV_32FC1 ) );
	time_t t;
	t = time( NULL );
	srand( t );

	int x = (int)( ( rand() * 1.0 / RAND_MAX ) * image_size.width );
	int y = (int)( ( rand() * 1.0 / RAND_MAX ) * image_size.height );
	cvmSet( point, 0, 0, x * 1.0 );
	cvmSet( point, 1, 0, y * 1.0 );

	return point;
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
