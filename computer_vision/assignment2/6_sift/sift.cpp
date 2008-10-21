#include <limits.h>
#include <fstream>
#include <iostream>
#include <sift.hpp>
#include <highgui.h>

const int NUM_OF_ARGS = 4;
const int KEY_VECTORS_WIDTH = 4;
const int KEY_DESCRIPTORS_WIDTH = 128;
const int DESCRIPTORS_PER_LINE = 20;
const float THRESHOLD = 0.1;

ImageRAII appendimages( IplImage * image1, IplImage * image2 )
{
	CvSize image1_size = cvGetSize( image1 );
	CvSize image2_size = cvGetSize( image2 );
	CvSize final_size = cvSize( image1_size.width + image2_size.width, std::max( image1_size.height, image2_size.height ) );
	ImageRAII grayscale_image1( cvCreateImage( image1_size, image1->depth, 1 ) );
	ImageRAII grayscale_image2( cvCreateImage( image2_size, image2->depth, 1 ) );
	ImageRAII final_image( cvCreateImage( final_size, image1->depth, 1 ) );

	// convert to grayscale
	cvCvtColor( image1, grayscale_image1.image, CV_BGR2GRAY );
	cvCvtColor( image2, grayscale_image2.image, CV_BGR2GRAY );

	int width_adjust = 0;
	// combine images side by side
	for( int i = 0; i < image1_size.width; i++ )
	{
		for( int j = 0; j < image1_size.height; j++ )
		{
			*( final_image.image->imageData + j * final_image.image->widthStep + final_image.image->nChannels * ( i + width_adjust ) ) = *( grayscale_image1.image->imageData + j * grayscale_image1.image->widthStep + grayscale_image1.image->nChannels * i );
		}
	}
	width_adjust = image1_size.width;
	for( int i = 0; i < image2_size.width; i++ )
	{
		for( int j = 0; j < image2_size.height; j++ )
		{
			*( final_image.image->imageData + j * final_image.image->widthStep + final_image.image->nChannels * ( i + width_adjust ) ) = *( grayscale_image2.image->imageData + j * grayscale_image2.image->widthStep + grayscale_image2.image->nChannels * i );
		}
	}
return final_image;
}

std::pair< CvMat *, CvMat * > readkeys( const char * filename )
{
	int buffer_length = 2048;
	char buffer[buffer_length];

	std::ifstream in_file( filename );

	in_file.getline( buffer, buffer_length );
	std::vector< std::string > tokens = tokenize_str( buffer, " " );

	// parse header information
	int number_of_keypoints = lexical_cast< std::string, int >( tokens[0] );
	int length = lexical_cast< std::string, int >( tokens[1] );

	CvMat * key_vectors = cvCreateMat( number_of_keypoints, KEY_VECTORS_WIDTH, CV_32FC1 );
	CvMat * key_descriptors = cvCreateMat( number_of_keypoints, KEY_DESCRIPTORS_WIDTH, CV_32FC1 );

	// parse rest of file
	for( int j = 0; j < number_of_keypoints; j++ )
	{
		// parse vectors
		in_file.getline( buffer, buffer_length );
		tokens = tokenize_str( buffer, " " );
		for( int i = 0; i < tokens.size(); i++ )
		{
			cvmSet( key_vectors, j, i, lexical_cast< std::string, float >( tokens[i] ) );
		}

		// parse descriptors
		float sum = 0;
		for( int k = 0; k < ceil( (float)length / DESCRIPTORS_PER_LINE ); k ++ )
		{
			in_file.getline( buffer, buffer_length );
			tokens = tokenize_str( buffer, " " );
			
			for( int i = 0; i < tokens.size(); i++ )
				sum += lexical_cast< std::string, float >( tokens[i] ) ;

		}
		float normalize = cvSqrt( pow( sum, 2 ) );
		for( int i = 0; i < tokens.size(); i++ )
			cvmSet( key_descriptors, j, i,  lexical_cast< std::string, float >( tokens[i] ) / normalize );
	}

	return std::make_pair( key_vectors, key_descriptors );
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

ImageRAII match( IplImage * image1, IplImage * image2, std::pair< CvMat *, CvMat * > image1_keys, std::pair< CvMat *, CvMat * > image2_keys )
{
	ImageRAII appended_images = appendimages( image1, image2 );
	ImageRAII rgb_appended_images( cvCreateImage( cvGetSize( appended_images.image ), appended_images.image->depth, 3 ) );
	cvCvtColor( appended_images.image, rgb_appended_images.image, CV_GRAY2RGB );
	CvScalar red;
	red.val[2] = 255;

	// check for matches with the vectors in image1 and image2
	for( int i = 0; i < image1_keys.first->height; i++ )
	{
		MatrixRAII current_vector( cvCreateMat( 1, image1_keys.first->cols, CV_32FC1 ) );
		// keeps track of minimum row found b/t image1 and image2 vectors
		MatrixRAII min( cvCreateMat( 1, image2_keys.first->cols, CV_32FC1 ) );
		bool draw = false;
		cvGetRow( image1_keys.first, current_vector.matrix, i );
		CvPoint point1 = cvPoint( ( int )cvmGet( current_vector.matrix, 0, 1 ), ( int )cvmGet( current_vector.matrix, 0, 0 ) );

		double difference, distance_squared = 0;
		double distance_squared1 = INT_MAX;
		double distance_squared2 = INT_MAX;
		// compare a vector in image1 to every vector in image2
		for( int j = 0; j < image2_keys.first->cols; j++ )
		{

			// calculate the squared distance between the keypoint discriptors
			for( int k = 0; k < image1_keys.second->width; k++ )
			{
				double descriptor1 = cvmGet( image1_keys.second, i, k );
				double descriptor2 = cvmGet( image2_keys.second, j, k );
				difference = descriptor1 - descriptor2;
				distance_squared += pow( difference, 2 );
			}

			// find the closest euclidean squared distance (takes care of negatives) of the two smallest distances
			if( distance_squared < distance_squared1 )
			{
				distance_squared2 = distance_squared1;
				distance_squared1 = distance_squared;
			}
			else if( distance_squared < distance_squared2 )
			{
				distance_squared2 = distance_squared;
			}

			// only keep the distance if it's smaller than a threshold of the 2nd smallest distance
			if( distance_squared1 < THRESHOLD *  distance_squared2 )
			{
				cvGetRow( image2_keys.first, min.matrix, i );
				draw = true;
			}
			else
			{
				draw = false;
			}
		}

		// found a minimum within the desired distance, draw the line
		if( draw )
		{
			CvPoint point2 = cvPoint( ( int )cvmGet( min.matrix, 0, 1 ) + image1->width, ( int )cvmGet( min.matrix, 0, 0 ) );
			cvLine( rgb_appended_images.image, point1, point2, red );
		}
	}

	return rgb_appended_images;
}

int main( int argc, char * argv[] )
{
	const char * WINDOW_NAME = "Appended Images";

	if( argc <= NUM_OF_ARGS )
	{
		std::cerr << "Need " << NUM_OF_ARGS << " args.  sift <image1> <image1_key> <image2> <image2_key>";
		exit( -1 );
	}

	ImageRAII image1( argv[1] );
	ImageRAII image2( argv[3] );
	std::pair< CvMat *, CvMat * > tmp1;
	std::pair< CvMat *, CvMat * > tmp2;
	std::pair< MatrixRAII, MatrixRAII > image1_keys;
	std::pair< MatrixRAII, MatrixRAII > image2_keys;

	tmp1 = readkeys( argv[2] );
	image1_keys.first.matrix = tmp1.first;
	image1_keys.second.matrix = tmp1.second;
	tmp2 = readkeys( argv[4] );
	image2_keys.first.matrix = tmp2.first;
	image2_keys.second.matrix = tmp2.second;

	ImageRAII appended_images = match( image1.image, image2.image, tmp1, tmp2 );

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, appended_images.image );

	cvWaitKey( 0 );

	return 0;
}
