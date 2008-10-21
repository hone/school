#include <iostream>
#include <sift.hpp>
#include <highgui.h>

int const NUM_OF_ARGS = 2;

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

int main( int argc, char * argv[] )
{
	const char * WINDOW_NAME = "Appended Images";

	if( argc <= NUM_OF_ARGS )
	{
		std::cerr << "Need " << NUM_OF_ARGS << " args.  sift <image1> <image2>";
		exit( -1 );
	}

	ImageRAII image1( argv[1] );
	ImageRAII image2( argv[2] );

	ImageRAII appended_images = appendimages( image1.image, image2.image );

	cvNamedWindow( WINDOW_NAME );
	cvShowImage( WINDOW_NAME, appended_images.image );

	cvWaitKey( 0 );

	return 0;
}
