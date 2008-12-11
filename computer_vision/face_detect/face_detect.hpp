#ifndef _FACE_DETECT_H
#define _FACE_DETECT_H

#include <cv.h>
#include <vector>
#include <string>
#include <sstream>
#include <string>

const int MAX_COLORS = 8;
const CvScalar colors[] = {
	{{0,0,255}}, {{0,128,255}}, {{0,255,255}}, {{0,255,0}},
	{{255,128,0}},{{255,255,0}}, {{255,0,0}}, {{255,0,255}}

};

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
		std::vector< IplImage * > extract_faces( IplImage * img, int scale_size, int min_neighbor, int flag, double scale = 1.0 );
		std::vector< IplImage * > detect_and_save( IplImage * img, std::string &filename, int scale_size, int min_neighbor, int flag, double scale = 1.0 );
};

#endif
