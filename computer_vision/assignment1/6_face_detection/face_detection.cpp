#include <string>
#include <image_raii.h>
#include <cv.h>
#include <highgui.h>

/**
 * Mouse event.  Handles picking the ROI.
 */
void on_mouse( int event, int x, int y, int flags, void *param )
{
	switch( event )
	{
		case CV_EVENT_LBUTTONDOWN:
		{
			CvPoint point = cvPoint( x, y );
			( *(std::pair<CvPoint, CvPoint> *)param ).first = point;
			break;
		}
		case CV_EVENT_LBUTTONUP:
		{
			CvPoint point = cvPoint( x, y );
			( *(std::pair<CvPoint, CvPoint> *)param ).second = point;
			break;
		}
	}
}

/**
 * User defines a Region of Interest.
 * @param image image of interest.
 */
CvRect specifyROI( ImageRAII &image )
{
	const char * WINDOW_NAME = "Pick Region of Interest";
	const int WAIT_TIME = 100; // in milliseconds

	// first point corresponds to top left, second point corresponds to bottom right
	std::pair<CvPoint, CvPoint> rectangle_corners;
	// so we can tell if the rectangle_corners have been set
	rectangle_corners.first.x = -1;
	rectangle_corners.first.y = -1;
	rectangle_corners.second.x = -1;
	rectangle_corners.second.y = -1;

	cvNamedWindow( WINDOW_NAME, CV_WINDOW_AUTOSIZE );
	cvShowImage( WINDOW_NAME, image.image );

	cvSetMouseCallback( WINDOW_NAME, on_mouse, &rectangle_corners );

	// wait until the rectangular bounding box is set
	while( rectangle_corners.first.x < 0 || rectangle_corners.first.y < 0 || rectangle_corners.second.x < 0 || rectangle_corners.second.y < 0 )
	{
		cvWaitKey( WAIT_TIME );
	}

	cvDestroyWindow( WINDOW_NAME );
	CvPoint top_left_corner = rectangle_corners.first;
	CvPoint bottom_right_corner = rectangle_corners.second;
	int x_distance = bottom_right_corner.x - top_left_corner.x;
	int y_distance = bottom_right_corner.y - top_left_corner.y;
	CvRect image_roi = cvRect( top_left_corner.x, top_left_corner.y, x_distance, y_distance );

	return image_roi;
}

int main( int argc, char * agv[] )
{
	ImageRAII friends_image( "Friends.jpg" );
	CvRect image_roi = specifyROI( friends_image );

	cvSetImageROI( friends_image.image, image_roi );

	return 0;
}
