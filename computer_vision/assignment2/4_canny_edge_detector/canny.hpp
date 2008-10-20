#ifndef _CANNY_H
#define _CANNY_H
#include <utility>
#include <image_raii.hpp>
#include <cv.h>

/**
 * Apply canny detection algorithm. This function handles the enhancement and calls the nonmaxsuppresion and hysteresis.
 */
ImageRAII canny( IplImage * image, std::pair< int, int > thresh, double sigma );
/**
 * Based on the angle given, return the correct edge direction.
 */
float find_angle( float angle );
/**
 * Applies the Non-maximum suppression algorithm to the Canny Enhancer.
 */
ImageRAII nonMaxSup( IplImage * strength, IplImage * orientation );
/**
 * Return the two positions along the normal to the edge.
 */
std::pair< CvPoint, CvPoint > get_normal_positions( IplImage * orientation, int x, int y );
/**
 * Decides where to suppress the point based on the neighboring points.
 */
CvScalar suppress( double s, std::pair< CvPoint, CvPoint > positions, IplImage * strength );
/**
 * Apply threshold hysteresis.
 */
ImageRAII hysteresis( IplImage * image, IplImage * orientation, std::pair< int, int > thresh );
/**
 * Return the two positions along the edge.
 */
std::pair< CvPoint, CvPoint > get_edge_positions( IplImage * orientation, int x, int y );
/**
 * Given a position and image, checks whether the position lies within proper image boundaries
 */
bool check_boundaries( IplImage * image, CvPoint position );

#endif
