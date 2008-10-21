#ifndef _SIFT_H
#define _SIFT_H
#include <vector>
#include <utility>
#include <sstream>
#include <string>
#include <cv.h>
#include <image_raii.hpp>
#include <matrix_raii.hpp>

/**
 * Append two images together.
 */
ImageRAII appendimages( IplImage * image1, IplImage * image2 );
/**
 * Read the .key file which contains the keypoints for the images.
 */
std::pair< CvMat *, CvMat * > readkeys( const char * filename );
/**
 * String tokenizer taken from http://www.rosettacode.org/wiki/Tokenizing_A_String#C.2B.2B
 */
std::vector<std::string> tokenize_str( const std::string & str, const std::string & delims );

/**
 * Typecasts from source to destination.
 */
template< typename source, typename dest>
dest lexical_cast(source src)
	{
		std::stringstream ss;
		dest output;

		ss << src;
		ss >> output;
		   
		return output; 
	}

#endif
