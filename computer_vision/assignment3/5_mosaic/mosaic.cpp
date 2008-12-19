#include <image_raii.hpp>
#include <matrix_raii.hpp>
#include <window_raii.hpp>
#include <mosaic.hpp>
#include <iostream>
#include <fstream>
#include <map>

const int FEATURE_LENGTH = 128;
const float THRESHOLD = 0.15;

std::pair<CvMat *, CvMat * > find_correspondences( CvMat * v1, CvMat * v2, CvMat * f1, CvMat * f2 )
{
	std::vector< std::pair< int, int > > points;
	std::cout << v1->cols << std::endl;

	// check for matches with the vectors in image1 and image2
	for( int i = 0; i < v1->height; i++ )
	{
		double magnitude1 = 0;
		MatrixRAII current_vector( cvCreateMat( 1, v1->cols, CV_32FC1 ) );
		// keeps track of minimum row found b/t image1 and image2 vectors
		MatrixRAII min( cvCreateMat( 1, v2->cols, CV_32FC1 ) );
		cvGetRow( v1, current_vector.matrix, i );
		CvPoint point1 = cvPoint( ( int )cvmGet( current_vector.matrix, 0, 1 ), ( int )cvmGet( current_vector.matrix, 0, 0 ) );
		std::map< float, int > angles;

		for( int k = 0; k < f1->width; k++ )
			magnitude1 += pow( cvmGet( f1, i, k ), 2 );
		magnitude1 = cvSqrt( magnitude1 );

		// compare a vector in image1 to every vector in image2 by calculating the cosine simularity
		for( int j = 0; j < v2->height; j++ )
		{
			MatrixRAII descriptor1( cvCreateMat( 1, f1->cols, CV_32FC1 ) );
			MatrixRAII descriptor2( cvCreateMat( 1, f2->cols, CV_32FC1 ) );

			cvGetRow( f1, descriptor1.matrix, i );
			cvGetRow( f2, descriptor2.matrix, j );

			double dot_product = cvDotProduct( descriptor1.matrix, descriptor2.matrix );
			double magnitude2 = 0;
			for( int k = 0; k < f2->width; k++ )
				magnitude2 += pow( cvmGet( descriptor1.matrix, 0, k ), 2 );
			magnitude2 = cvSqrt( magnitude2 );

			angles.insert( std::pair< float, int >( acos( dot_product / ( magnitude1 * magnitude2 ) ), j ) );
		}

		std::map< float, int >::iterator it =  angles.begin();
		int index = it->second;
		float angle = it->first;
		it++;
		if( angle < THRESHOLD * it->first )
		{
			points.push_back( std::make_pair( i, index ) );
		}
	}

	std::pair< CvMat *, CvMat *> matched_points;
	CvMat * p1 = cvCreateMat( points.size(), 2, CV_32FC1 );
	CvMat * p2 = cvCreateMat( points.size(), 2, CV_32FC1 );
	for( int i = 0; i < points.size(); i++ )
	{
		cvmSet( p1, i, 0, cvmGet( v1, points[i].first, 0 ) );
		cvmSet( p1, i, 1, cvmGet( v1, points[i].first, 1 ) );
		cvmSet( p2, i, 0, cvmGet( v2, points[i].second, 0 ) );
		cvmSet( p2, i, 1, cvmGet( v2, points[i].second, 1 ) );
	}
	matched_points.first = p1;
	matched_points.second = p2;

	return matched_points;
}

CvMat * ransac_homography( CvMat * p1, CvMat * p2 )
{
	CvMat * h = cvCreateMat( 3, 3, CV_32FC1 );
	cvFindFundamentalMat( p1, p2, h );
	/*
	for( int j = 0; j < ITERATIONS; j++ )
	{
		MatrixRAII p1_tmp( cvCreateMat( 4, 2, CV_32FC1 ) );
		MatrixRAII p2_tmp( cvCreateMat( 4, 2, CV_32FC1 ) );
		MatrixRAII h( cvCreateMat( 3, 3, CV_32FC1 ) );
		int rows = p1->rows;

		for( int i = 0; i < 4; i++ )
		{
			int index = random_number( rows );

			cvmSet( p1_tmp.matrix, index, 0, cvmGet( p1, index, 0 ) );
			cvmSet( p1_tmp.matrix, index, 1, cvmGet( p1, index, 1 ) );
			cvmSet( p1_tmp.matrix, index, 0, cvmGet( p1, index, 0 ) );
			cvmSet( p1_tmp.matrix, index, 1, cvmGet( p1, index, 1 ) );
		}
		cvFindFundamentalMatrix( p1_tmp.matrix, p2_tmp.matrix, h );
	}
	*/

	return h;
}

IplImage * stitch( IplImage * i1, IplImage * i2, CvMat * h )
{
	CvSize image1_size = cvGetSize( i1 );
	CvSize image2_size = cvGetSize( i2 );
	IplImage * final_image = cvCreateImage( cvSize( image1_size.width + image2_size.width, image1_size.height + image2_size.height ), i1->depth, i1->nChannels );

	cvWarpAffine( i1, final_image, h, CV_WARP_INVERSE_MAP );
	cvWarpAffine( i2, final_image, h, CV_WARP_INVERSE_MAP );

	return final_image;
}

int random_number( int max )
{   
	return (int)( ( rand() * 1.0 / RAND_MAX ) * max );
}

MatrixRAII read_file( const char * filename )
{
	std::string line;
	std::ifstream in_file( filename );

	int width, height;
	std::vector< std::vector< std::string > > data;

	// parse rest of file
	height = 0;
	while( getline( in_file, line ) )
	{
		std::vector< std::string > tokens = tokenize_str( line, " " );
		data.push_back( tokens );

		if( height == 0 )
			width = tokens.size();
		height++;
	}

	std::cout << "Width: " << width << std::endl;
	std::cout << "Height: " << height << std::endl;

	MatrixRAII mat( cvCreateMat( height, width, CV_32FC1 ) );

	for( int y = 0; y < data.size(); y++ )
	{
		std::vector< std::string > y_value = data[y];
		for( int x = 0; x < y_value.size(); x++ )
		{
			cvmSet( mat.matrix, y, x, lexical_cast< std::string, float >( y_value[x] ) );
		}
		
	}

	return mat;
}

std::vector< std::string > tokenize_str( const std::string & str, const std::string & delims )
{
    // Skip delims at beginning, find start of first token
    std::string::size_type lastPos = str.find_first_not_of(delims, 0);
	// Find next delimiter @ end of token
	std::string::size_type pos     = str.find_first_of(delims, lastPos);

    // output vector
    std::vector< std::string > tokens;

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delims.  Note the "not_of". this is beginning of token
        lastPos = str.find_first_not_of(delims, pos);
        // Find next delimiter at end of token.
        pos     = str.find_first_of(delims, lastPos);
    }

    return tokens;
}

int main( int argc, char ** argv )
{
	if( argc < 6 )
	{
		std::cerr << "Error: Need to supply at least 2 images.\n";
		exit( 0 );
	}

	// seed random number generator
	time_t t;
	t = time( NULL );
	srand( t );

	ImageRAII image1( argv[1] );
	ImageRAII image2( argv[2] );
	MatrixRAII points1 = read_file( argv[3] );
	MatrixRAII points2 = read_file( argv[4] );
	MatrixRAII features1 = read_file( argv[5] );
	MatrixRAII features2 = read_file( argv[6] );

	std::pair<CvMat *, CvMat *> p1p2 = find_correspondences( points1.matrix, points2.matrix, features1.matrix, features2.matrix );
	MatrixRAII h( ransac_homography( p1p2.first, p1p2.second ) );
	ImageRAII final( stitch( image1.image, image2.image, h.matrix ) );

	WindowRAII window( "Stitched!" );
	cvShowImage( window.name, final.image );
}
