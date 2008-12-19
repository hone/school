#include <string>
#include <iostream>
#include <image_raii.hpp>
#include <histogram_raii.hpp>
#include <window_raii.hpp>
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
CvRect specifyROI( IplImage * image )
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

	WindowRAII window( WINDOW_NAME );
	cvShowImage( window.name, image);

	cvSetMouseCallback( window.name, on_mouse, &rectangle_corners );

	// wait until the rectangular bounding box is set
	while( rectangle_corners.first.x < 0 || rectangle_corners.first.y < 0 || rectangle_corners.second.x < 0 || rectangle_corners.second.y < 0 )
	{
		cvWaitKey( WAIT_TIME );
	}

	CvPoint top_left_corner = rectangle_corners.first;
	CvPoint bottom_right_corner = rectangle_corners.second;
	int x_distance = bottom_right_corner.x - top_left_corner.x;
	int y_distance = bottom_right_corner.y - top_left_corner.y;
	CvRect image_roi = cvRect( top_left_corner.x, top_left_corner.y, x_distance, y_distance );

	// display rectangle
	CvScalar red;
	red.val[2] = 255;
	// copy image so not to ruin original image
	ImageRAII image_copy( cvCreateImage( cvGetSize( image ), image->depth, image->nChannels ) );
	cvRectangle( image_copy.image, top_left_corner, bottom_right_corner, red, 1 );
	cvShowImage( window.name, image_copy.image );
	cvWaitKey( WAIT_TIME );

	return image_roi;
}

/**
 * Makes a hue histogram given the image for a specified number of bins.
 * @param image image to generate hue histogram off of.
 * @param bins number of bins to use in the histogram
 */
HistogramRAII makeHueHistogram( IplImage * image, int bins )
{
	const int HIST_DIMENSIONS = 1;
	const float NORMALIZATION_FACTOR = 1.0;
	ImageRAII image_hsv( cvCreateImage( cvGetSize( image ), image->depth, 3 ) );
	ImageRAII hue_plane( cvCreateImage( cvGetSize( image ), image->depth, 1 ) );
	ImageRAII saturation_plane( cvCreateImage( cvGetSize( image ), image->depth, 1 ) );
	IplImage * planes[] = { hue_plane.image };
	ImageRAII value_plane( cvCreateImage( cvGetSize( image ), image->depth, 1 ) );
	int hist_size[] = { bins };
	float hist_ranges[] = { 0, 360 };
	float *ranges[] = { hist_ranges };

	// Convert RGB to HSV
	cvCvtColor( image, image_hsv.image, CV_BGR2HSV );
	// Split HSV Channels
	cvCvtPixToPlane( image_hsv.image, hue_plane.image, saturation_plane.image, value_plane.image, 0 );
	WindowRAII window( "ROI" );
	cvShowImage( window.name, hue_plane.image );
	cvWaitKey( 0 );
	HistogramRAII hue_histogram( cvCreateHist( HIST_DIMENSIONS, hist_size, CV_HIST_ARRAY, ranges, 1 ) );
	cvCalcHist( planes, hue_histogram.histogram, 0, 0 );
	// normalize histogram b/t 0 and 1
	cvNormalizeHist( hue_histogram.histogram, NORMALIZATION_FACTOR );

	return hue_histogram;
}

/**
 * makes a probability map given the hue histogram and image.
 * @param image image to compare against hue histogram
 * @param histogram hue histogram
 * @param bins number of bins
 */
float * makeProbabilityMap( IplImage * image, CvHistogram * histogram, int bins )
{
	CvSize image_size = cvGetSize( image );
	float sum = 0;
	float bin_values[bins];
	ImageRAII image_hsv( cvCreateImage( cvGetSize( image ), image->depth, 3 ) );
	ImageRAII map( cvCreateImage( image_size, IPL_DEPTH_8U, 1 ) );
	float * probability_map = new float[image_size.width * image_size.height];

	// convert to HSV
	cvCvtColor( image, image_hsv.image, CV_BGR2HSV );

	for( int i = 0; i < bins; i++ )
	{
		int bin_value = cvQueryHistValue_1D( histogram, i );
		sum += bin_value;
		bin_values[i] = bin_value;
	}

	for( int i = 0; i < image_size.width; i++ )
	{
		for( int j = 0; j < image_size.height; j++ )
		{
			float hue = cvGet2D( image_hsv.image, j, i ).val[0] / 255.0;
			for( int k = 0; k < bins; k++ )
			{
				float max_bin_value = ( 1.0 / bins ) * k + 1;
				float min_bin_value = ( 1.0 / bins ) * k;
				if( hue < max_bin_value && hue > min_bin_value )
				{
					float hist_value = cvQueryHistValue_1D( histogram, k );
					CvScalar value;
					value.val[0] = hist_value * 255;
					cvSet2D( map.image, j, i, value );
				}
			}
			/*
			float total_bin_contribution = 0;
			CvRect roi = cvRect( i, j, 1, 1 );
			CvSize one_pixel = cvSize( 1, 1 );
			ImageRAII pixel_image( cvCreateImage( one_pixel, image->depth, 3 ) );
			cvSetImageROI( image, roi );
			cvCopy( image, pixel_image.image );
			HistogramRAII pixel_histogram = makeHueHistogram( pixel_image.image, 10 );

			for( int l = 0; l < bins; l++ )
			{
				int bin_value = cvQueryHistValue_1D( pixel_histogram.histogram, l );
				if( bin_value > 0 )
				{
					total_bin_contribution = bin_value * bin_values[l];
				}
			}

			*( probability_map + i * image_size.width + j ) = total_bin_contribution / sum;
		*/
		}
	}

	WindowRAII window( "Probability Map" );
	cvShowImage( window.name, map.image );
	cvWaitKey( 0 );

	return probability_map;
}

int main( int argc, char * agv[] )
{
	const int BINS = 32;

	float * probability_map;

	ImageRAII friends_image( "Friends.jpg" );
	CvRect image_roi = specifyROI( friends_image.image );
	CvSize roi_size = cvSize( image_roi.width, image_roi.height );
	ImageRAII roi_image( cvCreateImage( roi_size, friends_image.image->depth, 3 ) );
	
	cvSetImageROI( friends_image.image, image_roi );
	cvCopy( friends_image.image, roi_image.image );
	cvResetImageROI( friends_image.image );

	HistogramRAII histogram = makeHueHistogram( roi_image.image, BINS );
	probability_map = makeProbabilityMap( friends_image.image , histogram.histogram, BINS );
	cvResetImageROI( friends_image.image );

	// display probability map
	CvSize image_size = cvGetSize( friends_image.image );
	printf( "%d %d", image_size.width, image_size.height );
	for( int i = 0; i < image_size.width; i++ )
	{
		for( int j = 0; j < image_size.height; j++ )
		{
			//printf( "%f.2 ", *( probability_map + i * image_size.width + j ) );
		}
		//printf( "\n" );
	}

	delete[] probability_map;

	return 0;
}
