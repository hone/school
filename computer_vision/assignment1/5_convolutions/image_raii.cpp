#include "image_loader.h"

ImageLoader::ImageLoader( std::string filename )
{
	this->image = cvLoadImage( filename.c_str() );
	checkImageLoad( filename );
}

ImageLoader::ImageLoader( IplImage * image )
{
	this->image = image;
	checkImageLoad( "" );
}

ImageLoader::~ImageLoader()
{
	cvReleaseImage( &this->image );
}

void ImageLoader::checkImageLoad( std::string message )
{
	if( !this->image )
	{
		printf( "Could not load image: %s", message.c_str() );
		exit( 0 );
	}
}
