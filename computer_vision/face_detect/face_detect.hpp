#include <cv.h>
// Page 511 to 512 of the O'Reilly Learning OpenCV book
// Detect and draw detected object boxes on image
// Presumes 2 Globals:
//  Cascade is loaded by:
//		cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
//	AND that storage is allocated:
//	CvMemStorage* storage = cvCreateMemStorage(0);
void detect_and_draw( IplImage * img, double scale = 1.3 );
