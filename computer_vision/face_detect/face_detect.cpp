#include <iostream>
#include <highgui.h>
#include <image_raii.hpp>
#include <face_detect.hpp>

using namespace std;

const char* cascade_name = "haarcascade_frontalface_alt2.xml";
CvHaarClassifierCascade * cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
CvMemStorage* storage = cvCreateMemStorage(0);

void detect_and_draw( IplImage * img, const char * filename, int min_neighbor, int flag, double scale )
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
	CvSeq * objects = cvHaarDetectObjects( small_img, cascade, storage, 1.1, min_neighbor, flag, cvSize( 10, 10 ) );

	// Loop through found objects and draw boxes around them
	for( int i = 0; i < (objects ? objects->total : 0); i++ )
	{
		CvRect * r = (CvRect*)cvGetSeqElem( objects, i );
		cvRectangle(
				img,
				cvPoint( r->x, r->y ),
				cvPoint( r->x + r->width, r->y + r->height ),
				colors[i % 8]
				);
	}
	cvSaveImage( filename, img );

	cvReleaseImage( &gray );
	cvReleaseImage( &small_img );
}

vector< string > tokenize_str( const string & str, const string & delims )
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
	ImageRAII image( argv[1] );
	int neighbor = lexical_cast< string, int >( argv[3] );
	int flag = lexical_cast< string, int >( argv[4] );
	detect_and_draw( image.image, argv[2], neighbor, flag, 1.0 );

	return 0;
}
