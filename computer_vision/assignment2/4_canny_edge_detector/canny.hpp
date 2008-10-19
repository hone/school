#ifndef _CANNY_H
#define _CANNY_H
#include <image_raii.hpp>
#include <cv.h>

/**
 * Apply canny detection algorithm. This function handles the enhancement and calls the nonmaxsuppresion and hysteresis.
 */
ImageRAII canny( IplImage * image, CvMat * thresh, double sigma );
/**
 * Based on the angle given, return the correct edge direction.
 */
float find_angle( float angle );
/**
 * Applies the Non-maximum suppression algorithm to the Canny Enhancer.
 */
ImageRAII nonMaxSup( IplImage * strength, IplImage * orientation );
/**
 * Decides where to suppress the point based on the neighboring points.
 */
CvScalar suppress( double s, CvPoint position1, CvPoint position2, IplImage * strength, CvSize image_size );
void hysteresis( IplImage * image );


#endif
