#ifndef _FACE_DETECT_H
#define _FACE_DETECT_H

#include <cv.h>
#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <string>
#include <string_util.hpp>

const int MAX_COLORS = 8;
const CvScalar COLORS[] = {
	{{0,0,255}}, {{0,128,255}}, {{0,255,255}}, {{0,255,0}},
	{{255,128,0}},{{255,255,0}}, {{255,0,0}}, {{255,0,255}}

};
const int WIDTH = 640;
const int HEIGHT = 480;

class FaceDetect
{

	private:
		std::string cascade_name;
		CvHaarClassifierCascade * cascade;
		CvMemStorage * storage;

	public:
		FaceDetect( std::string cascade_filename = "haarcascade_frontalface_alt2.xml" );
		~FaceDetect();
		// Page 511 to 512 of the O'Reilly Learning OpenCV book
		// Detect and draw detected object boxes on image
		// Presumes 2 Globals:
		//  Cascade is loaded by:
		//		cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
		//	AND that storage is allocated:
		//	CvMemStorage* storage = cvCreateMemStorage(0);
		std::vector< std::pair< IplImage *, CvRect > > extract_faces( IplImage * img, int scale_size, int min_neighbor, int flag, double scale = 1.0 );
		std::vector< IplImage * > detect_and_save( IplImage * img, std::string &filename, int scale_size, int min_neighbor, int flag, double scale = 1.0 );
		CvSeq * detect_rectangles( IplImage * img, int min_neighbor, int flag, int scale  = 1.0 );
		// Function prototype for drawing rectangle and labeling face in image
		void detect_and_label( IplImage* image, std::vector<std::pair<int, CvRect> > labels, std::string filename );
};

#endif
