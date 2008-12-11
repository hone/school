#include <vector>
#include <string>
#include <iostream>
#include <cv.h>
#include <image_raii.hpp>
#include <window_raii.hpp>
#include <face_detect.hpp>
#include <string_util.hpp>

using namespace std;

int main( int argc, char ** argv )
{
	FaceDetect fd;
	ImageRAII image( argv[1] );
	string filename( argv[2] );
	vector< IplImage * > faces = fd.detect_and_save( image.image, filename, 100, 2, 0, 1.0 );

	vector< string > filename_parts =  tokenize_str( filename, "." );
	for( int i = 0; i < faces.size(); i++ )
	{
		string filename_thumbnail = filename_parts[0] + lexical_cast<int, string>(i) + "." + filename_parts[1];
		cout << filename_thumbnail << endl;
		cvSaveImage( filename_thumbnail.c_str(), faces[i] );
	}

	/*
	WindowRAII window( "Face" );

	for( int i = 0; i < faces.size(); i ++ )
	{
		cvShowImage( window.name, faces[i] );
		cvWaitKey(0);
	}
	*/
}
