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
	
	// only resize if image is too big
	if( img->width > WIDTH || img->height > HEIGHT )
	{
		if( img->width > img->height )
			scale = ( 1.0 * WIDTH ) / img->width;
		else
			scale = ( 1.0 * HEIGHT ) / img->height;
	}

	// Image Preparation
	ImageRAII img_resize( cvCreateImage( cvSize( cvRound( img->width * scale ), cvRound( img->height * scale ) ), img->depth, img->nChannels ) );
	cvResize( img, img_resize.image );
	ImageRAII gray( cvCreateImage( cvSize( img->width, img->height ), 8, 1 ) );
	ImageRAII small_img( cvCreateImage( cvSize( cvRound( img->width ), cvRound( img->height ) ), 8, 1 ) );
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
				img_resize.image,
				cvPoint( r->x * scale, r->y * scale ),
				cvPoint( (r->x + r->width) * scale, (r->y + r->height) * scale ),
				COLORS[i % MAX_COLORS]
				);

		ImageRAII face( cvCreateImage( cvSize( r->width, r->height ), 8, 1 ) );
		IplImage * face_resize = cvCreateImage( cvSize( scale_size, scale_size ), 8, 1 );

		// extract face from picture
		cvSetImageROI( gray.image, *r );
		cvCopy( gray.image, face.image );
		cvResize( face.image, face_resize );
		faces.push_back( face_resize );
	}

	cvSaveImage( filename.c_str(), img_resize.image );

	return faces;
}

vector< pair< IplImage *, CvRect > > FaceDetect::extract_faces( IplImage * img, int scale_size, int min_neighbor, int flag, double scale )
{
	vector< pair< IplImage *, CvRect > > faces;

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
		faces.push_back( make_pair( face_resize, *r ) );
	}

	return faces;
}

CvSeq * FaceDetect::detect_rectangles( IplImage * img, int min_neighbor, int flag, int scale )
{
	// Image Preparation
	ImageRAII gray( cvCreateImage( cvSize( img->width, img->height ), 8, 1 ) );
	ImageRAII small_img( cvCreateImage( cvSize( cvRound( img->width / scale ), cvRound( img->height / scale ) ), 8, 1 ) );
	cvCvtColor( img, gray.image, CV_BGR2GRAY );
	cvResize( gray.image, small_img.image, CV_INTER_LINEAR );
	cvEqualizeHist( small_img.image, small_img.image );

	// Detect Objects if any
	cvClearMemStorage( storage );
	CvSeq * objects = cvHaarDetectObjects( small_img.image, cascade, storage, 1.1, min_neighbor, flag, cvSize( 10, 10 ) );

	return objects;
}

void FaceDetect::detect_and_label( IplImage* img, std::vector<std::pair<int, CvRect> > labels, string filename )
{
	double scale = 1.0;

	// only resize if image is too big
	if( img->width > WIDTH || img->height > HEIGHT )
	{
		if( img->width > img->height )
			scale = ( 1.0 * WIDTH ) / img->width;
		else
			scale = ( 1.0 * HEIGHT ) / img->height;
	}

	CvFont font;
	double hScale = 1.0;
	double vScale = 1.0;
	int lineWidth = 1;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale, vScale, 0, lineWidth);

	// Create a new image based on the input image
	ImageRAII temp( cvCreateImage( cvSize(img->width * scale, img->height * scale), img->depth, img->nChannels ) );
	cvResize( img, temp.image );

	// Create two points to represent the face locations
	CvPoint pt1, pt2;

	// Loop through the entered set face rectangles and labels
	for(int i = 0; i < labels.size(); i++)
	{
		CvRect r = labels[i].second;

		// Find the dimensions of the rectangle
		pt1.x = r.x * scale;
		pt2.x = ( r.x + r.width ) * scale;
		pt1.y = r.y * scale;
		pt2.y = ( r.y + r.height ) * scale;

		// Draw the rectangle in the input image
		cvRectangle(temp.image, pt1, pt2, COLORS[i % MAX_COLORS], 3, 8, 0);

		// Draw the label for this face rectangle
		cvPutText(temp.image, lexical_cast<int, string>(labels[i].first).c_str(), pt1, &font, cvScalar(255, 255, 0));
	}

	cvSaveImage( filename.c_str(), temp.image );
}
