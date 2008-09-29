#include <image_raii.h>

ImageRAII::ImageRAII()
{
    this->image = NULL;
}

ImageRAII::ImageRAII( std::string filename )
{
	this->image = cvLoadImage( filename.c_str() );
	checkImageLoad( filename );
}

ImageRAII::ImageRAII( IplImage * image )
{
	this->image = image;
	checkImageLoad( "" );
}

ImageRAII::~ImageRAII()
{
	cvReleaseImage( &this->image );
}

void ImageRAII::checkImageLoad( std::string message )
{
	if( !this->image )
	{
		printf( "Could not load image: %s", message.c_str() );
		exit( 0 );
	}
}
