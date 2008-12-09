#include <face_detect.hpp>

CvHaarClassifierCascade * cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
CvMemStorage* storage = cvCreateMemStorage(0);

void detect_and_draw( IplImage * img, double scale )
{
	static CvScalar colors[] = {
		{{0,0,255}}, {{0,128,255}}, {{0,255,255}}, {{0,255,0}},
		{{255,128,0}},{{255,255,0}}, {{255,0,0}}, {{255,0,255}}
	}; // just some pretty colors to draw with

	// Image Preparation
	IplImage * gray = cvCreateImage( cvSize( img->width, img->height ), 8, 1 );
	IplImage * small_img = cvCreateImage( cvSize( cvRound( img->width / scale ), cvRound( img->height / scale ) ), 8, 1 );
	cvCvtColor( img, gray, CV_BGR2GRAY );
	cvResize( gray, small_img, CV_INTER_LINEAR );
	cvEqualizeHist( small_img, small_img );

	// Detect Objects if any
	cvClearMemStorage( storage );
	CvSeq * objects = cvHaarDetectObjects( small_img, cascade, storage, 1.1, 2, 0 /* CV_HAAR_DO_CANNY_PRUNING */, cvSize( 30, 30 ) );

	// Loop through found objects and draw boxes around them
	for( int i = 0; i < (objects ? objects->total : 0); i++ )
	{
		CvRect * r = (CvRect*)cvGetSeqElem( objects, i );
		cvRectangle(
				img,
				cvPoint( r.x, r.y ),
				cvPoint( r.x + r.width, r.y + r.height ),
				colors[i % 8]
				)
	}
	cvReleaseImage( &gray );
	cvReleaseImage( &small_img );
}
