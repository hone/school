#include <sstream>
#include <fstream>
#include <iostream>
#include <sift.hpp>
#include <highgui.h>

const int NUM_OF_ARGS = 4;
const int KEY_VECTORS_WIDTH = 4;
const int KEY_DESCRIPTORS_WIDTH = 128;
const int DESCRIPTORS_PER_LINE = 20;

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
	std::pair< CvMat *, CvMat * > tmp;
	std::pair< MatrixRAII, MatrixRAII > image1_keys;
	std::pair< MatrixRAII, MatrixRAII > image2_keys;

	tmp = readkeys( argv[2] );
	image1_keys.first.matrix = tmp.first;
	image1_keys.second.matrix = tmp.second;
	tmp = readkeys( argv[4] );
	image2_keys.first.matrix = tmp.first;
	image2_keys.second.matrix = tmp.second;

	ImageRAII appended_images = appendimages( image1.image, image2.image );

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, appended_images.image );

	cvWaitKey( 0 );

	return 0;
}
