#ifndef _HARRIS_H
#define _HARRIS_H
#include <vector>
#include <image_raii.hpp>

/**
 * Performs Harris Corner Detection.  Marks corners with red pixels.
 */
ImageRAII detectCorner( IplImage * image, int win, float sigma );
/**
 * Scales pyramid.
 */
ImageRAII makePyramid( IplImage * image, float sigma );
/**
 * Takes a pyramid of images with detected corners and suppresses results at similar locations at different levels in the pyramid.
 */
ImageRAII localMaxSup( std::vector< IplImage * > pyrCor, int scale );

#endif
