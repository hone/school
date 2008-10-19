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
void nonMaxSup( IplImage * image );
void hysteresis( IplImage * image );

#endif
