#include <iostream>
#include <em.hpp>
#include <image_raii.hpp>
#include <window_raii.hpp>
#include <iostream>

const int GRAPH_WIDTH = 100;
const int GRAPH_HEIGHT = 100;
const int RED = 0;
const int GREEN = 1;

void em( CvMat * data, int nClusters, int nTries, IplImage * image )
{
	CvSize image_size = cvGetSize( image );
	std::vector<CvPoint> starting_points;
	std::vector<CvMat *> means;
	// 2 x 2 matrix
	std::vector<CvMat *> variances;
	std::vector<double> weights;

	MatrixRAII initial_variance( cvCreateMat( 2, 2, CV_32FC1 ) );
	cvmSet( initial_variance.matrix, 0, 0, 1.0 );
	cvmSet( initial_variance.matrix, 0, 1, 0.0 );
	cvmSet( initial_variance.matrix, 1, 0, 0.0 );
	cvmSet( initial_variance.matrix, 1, 1, 1.0 );

	double initial_weight = 1.0 / nClusters;

	// initialize starting values
	for( int i = 0; i < nClusters; i++ )
	{
		CvPoint point = random_point( image_size );
		starting_points.push_back( point );
		double red = cvGet2D( image, point.y, point.x ).val[2] / 255.0;
		double green = cvGet2D( image, point.y, point.x ).val[1] / 255.0;
		MatrixRAII x = create_point( red, green );
		means.push_back( cvCloneMat( x.matrix ) );
		
		// set variance
		variances.push_back( cvCloneMat( initial_variance.matrix ) );

		// weight
		weights.push_back( initial_weight );
	}

	display_data( data, means, variances, weights, image_size );

	MatrixRAII r = expectation( data, nClusters, means, variances, weights );
	maximization( r.matrix, data, means, variances, weights );

	display_data( data, means, variances, weights, image_size );
}

MatrixRAII expectation( CvMat * data, int nClusters, std::vector<CvMat *> &means, std::vector<CvMat *> &variances, std::vector<double> &weights )
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
			MatrixRAII x = create_point( red, green );

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

MatrixRAII create_point( double red, double green )
{
	MatrixRAII x( cvCreateMat( 2, 1, CV_32FC1 ) );
	cvmSet( x.matrix, 0, 0, red );
	cvmSet( x.matrix, 1, 0, green );

	return x;
}

double normal_distribution( double z, CvMat * x, CvMat * mean, CvMat * variance )
{
	MatrixRAII x_minus_mu( cvCreateMat( 2, 1, CV_32FC1 ) );
	MatrixRAII tranpose_x_minus_mu( cvCreateMat( 1, 2, CV_32FC1 ) );
	MatrixRAII x_minus_mu_times_variance( cvCreateMat( 1, 2, CV_32FC1 ) );
	MatrixRAII exponent( cvCreateMat( 1, 1, CV_32FC1 ) );

	cvAbsDiff( x,  mean, x_minus_mu.matrix );
	cvTranspose( x_minus_mu.matrix, tranpose_x_minus_mu.matrix );
	cvGEMM( tranpose_x_minus_mu.matrix, variance, -0.5, NULL, 0, x_minus_mu_times_variance.matrix );
	cvMatMul( x_minus_mu_times_variance.matrix, x_minus_mu.matrix, exponent.matrix );

	return ( 1 / z ) * pow( M_E, cvmGet( exponent.matrix, 0, 0 ) );
}

void maximization( CvMat * r, CvMat * data, std::vector<CvMat *> &means, std::vector<CvMat *> &variances, std::vector<double> &weights )
{
	int n = data->width;
	std::vector<CvMat *> old_means;
	for( std::vector<CvMat *>::iterator it = means.begin(); it != means.end(); it++ )
	{
		old_means.push_back( cvCloneMat( *it ) );
	}

	for( int c = 0; c < r->height; c++ )
	{
		double sum = 0;
		MatrixRAII mean_numerator_sum( cvCreateMat( 2, 1, CV_32FC1 ) );

		for( int p = 0; p < n; p++ )
		{
			double r_pk = cvmGet( r, c, p );
			double red = cvmGet( data, RED, p );
			double green = cvmGet( data, GREEN, p );
			MatrixRAII x = create_point( red, green );

			MatrixRAII mean_numerator( cvCreateMat( 2, 1, CV_32FC1 ) );
			MatrixRAII mean_numerator_tmp( cvCreateMat( 2, 1, CV_32FC1 ) );
			cvScale( x.matrix, mean_numerator.matrix, r_pk );
			mean_numerator_tmp.matrix = cvCloneMat( mean_numerator_sum.matrix );
			cvAdd( mean_numerator.matrix, mean_numerator_tmp.matrix, mean_numerator_sum.matrix );

			sum += r_pk;
		}

		weights[c] = ( 1 / n ) * sum;
		MatrixRAII mean( cvCreateMat( 2, 1, CV_32FC1 ) );
		cvScale( mean_numerator_sum.matrix, mean.matrix, sum );
		means[c] = mean.matrix;

		MatrixRAII variance_numerator_sum( cvCreateMat( 2, 2, CV_32FC1 ) );

		for( int p = 0; p < n; p++ )
		{
			double r_pk = cvmGet( r, c, p );
			double red = cvmGet( data, RED, p );
			double green = cvmGet( data, GREEN, p );
			MatrixRAII x = create_point( red, green );
			MatrixRAII x_new_mean( cvCreateMat( 2, 1, CV_32FC1 ) );
			MatrixRAII x_old_mean( cvCreateMat( 2, 1, CV_32FC1 ) );
			MatrixRAII x_old_mean_transpose( cvCreateMat( 1, 2, CV_32FC1 ) );
			MatrixRAII variance_numerator( cvCreateMat( 2, 2, CV_32FC1 ) );
			MatrixRAII variance_numerator_tmp( cvCreateMat( 2, 2, CV_32FC1 ) );
			
			cvAbsDiff( x.matrix, means[c], x_new_mean.matrix );
			cvAbsDiff( x.matrix, old_means[c], x_old_mean.matrix );
			cvTranspose( x_old_mean.matrix, x_old_mean_transpose.matrix );
			cvGEMM( x_new_mean.matrix, x_old_mean_transpose.matrix, r_pk, NULL, 0, variance_numerator.matrix  );
			variance_numerator_tmp.matrix = cvCloneMat( variance_numerator_sum.matrix );
			cvAdd( variance_numerator.matrix, variance_numerator_tmp.matrix, variance_numerator_sum.matrix );
		}

		MatrixRAII variance( cvCreateMat( 2, 2, CV_32FC1 ) );
		cvScale( variance_numerator_sum.matrix, variance.matrix, sum );
		variances[c] = variance.matrix;
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

	int x = (int)( ( rand() * 1.0 / RAND_MAX ) * image_size.width );
	int y = (int)( ( rand() * 1.0 / RAND_MAX ) * image_size.height );

	return cvPoint( x, y );
}

void display_data( CvMat * data, std::vector<CvMat *> means, std::vector<CvMat *> variances, std::vector<double> weights, CvSize image_size )
{
	int num_channels = 3;
	ImageRAII graph( cvCreateImage( cvSize( 101, 101 ), IPL_DEPTH_8U, num_channels ) );

	for( int i = 0; i < graph.image->width; i++ )
	{
		for( int j = 0; j < graph.image->height; j++ )
		{
			CvScalar white;
			for( int k = 0; k < num_channels; k++ )
			{
				white.val[k] = 255;
			}
			cvSet2D( graph.image, j, i, white );
		}
	}

	for( int i = 0; i < data->width; i++ )
	{
		CvScalar black;
		for( int k = 0; k < num_channels;k ++ )
		{
			black.val[k] = 0;
		}
		int green = cvmGet( data, GREEN, i ) * 100;
		int red = cvmGet( data, RED, i ) * 100;
		cvSet2D( graph.image, 100 - red, green, black );
	}

	int color = 0;
	for( std::vector<CvMat *>::iterator it = means.begin(); it != means.end(); it++ )
	{
		CvScalar red_color;
		red_color.val[2] = 255;
		CvScalar blue_color;
		blue_color.val[0] = 255;
		int green = cvmGet( *it, GREEN, 0 ) * 100;
		int red = cvmGet( *it, RED, 0 ) * 100;
		std::cout << red << ", " << green << std::endl;

		for( int i = -3; i <= 3; i++ )
		{
			for( int j = -3; j <= 3; j++ )
			{
				int new_red = red + i;
				int new_green = green + i;

				if( new_red < 0 )
					new_red = 0;
				if( new_green < 0 )
					new_green = 0;
				if( new_red >= image_size.width )
					new_red = image_size.width - 1;
				if( new_green >= image_size.height )
					new_green = image_size.height - 1;
				if( color == 0 )
					cvSet2D( graph.image, new_red, new_green, red_color );
				else
					cvSet2D( graph.image, new_red, new_green, blue_color );
			}
		}

		color++;
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
		time_t t;
		t = time( NULL );
		srand( t );

		ImageRAII image( argv[1] );
		MatrixRAII data = convert_data( image.image );
		em( data.matrix, 2, 1, image.image );
	}

	return 0;
}
