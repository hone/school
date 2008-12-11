#include <iostream>
#include <highgui.h>
#include <image_raii.hpp>
#include <window_raii.hpp>
#include <face_detect.hpp>

using namespace std;

FaceDetect::FaceDetect( string cascade_filename )
{
	this->cascade_name = cascade_filename;
	this->cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name.c_str(), 0, 0, 0 );
	this->storage = cvCreateMemStorage(0);
}

FaceDetect::~FaceDetect()
{
	cvReleaseHaarClassifierCascade( &(this->cascade) );
	cvReleaseMemStorage( &(this->storage) );
}

std::vector< IplImage * > FaceDetect::detect_and_save( IplImage * img, string &filename, int scale_size, int min_neighbor, int flag, double scale )
{
	vector<IplImage *> faces;

	// Image Preparation
	ImageRAII gray( cvCreateImage( cvSize( img->width, img->height ), 8, 1 ) );
	ImageRAII small_img( cvCreateImage( cvSize( cvRound( img->width / scale ), cvRound( img->height / scale ) ), 8, 1 ) );
	cvCvtColor( img, gray.image, CV_BGR2GRAY );
	cvResize( gray.image, small_img.image, CV_INTER_LINEAR );
	cvEqualizeHist( small_img.image, small_img.image );

	// Detect Objects if any
	cvClearMemStorage( storage );
	CvSeq * objects = cvHaarDetectObjects( small_img.image, cascade, storage, 1.1, min_neighbor, flag, cvSize( 10, 10 ) );

	// Loop through found objects and draw boxes around them
	for( int i = 0; i < (objects ? objects->total : 0); i++ )
	{
		CvRect * r = (CvRect*)cvGetSeqElem( objects, i );

		cvRectangle(
				img,
				cvPoint( r->x, r->y ),
				cvPoint( r->x + r->width, r->y + r->height ),
				colors[i % MAX_COLORS]
				);

		ImageRAII face( cvCreateImage( cvSize( r->width, r->height ), 8, 1 ) );
		IplImage * face_resize = cvCreateImage( cvSize( scale_size, scale_size ), 8, 1 );

		// extract face from picture
		cvSetImageROI( gray.image, *r );
		cvCopy( gray.image, face.image );
		cvResize( face.image, face_resize );
		faces.push_back( face_resize );
	}
	cvSaveImage( filename.c_str(), img );

	return faces;
}

vector<IplImage *> FaceDetect::extract_faces( IplImage * img, int scale_size, int min_neighbor, int flag, double scale )
{
	vector<IplImage *> faces;

	// Image Preparation
	ImageRAII gray( cvCreateImage( cvSize( img->width, img->height ), 8, 1 ) );
	ImageRAII small_img( cvCreateImage( cvSize( cvRound( img->width / scale ), cvRound( img->height / scale ) ), 8, 1 ) );
	cvCvtColor( img, gray.image, CV_BGR2GRAY );
	cvResize( gray.image, small_img.image, CV_INTER_LINEAR );
	cvEqualizeHist( small_img.image, small_img.image );

	// Detect Objects if any
	cvClearMemStorage( storage );
	CvSeq * objects = cvHaarDetectObjects( small_img.image, cascade, storage, 1.1, min_neighbor, flag, cvSize( 10, 10 ) );

	// Loop through found objects and draw boxes around them
	for( int i = 0; i < (objects ? objects->total : 0); i++ )
	{
		CvRect * r = (CvRect*)cvGetSeqElem( objects, i );
		ImageRAII face( cvCreateImage( cvSize( r->width, r->height ), 8, 1 ) );
		IplImage * face_resize = cvCreateImage( cvSize( scale_size, scale_size ), 8, 1 );

		// extract face from picture
		cvSetImageROI( gray.image, *r );
		cvCopy( gray.image, face.image );
		cvResize( face.image, face_resize );
		faces.push_back( face_resize );
	}

	return faces;
}
