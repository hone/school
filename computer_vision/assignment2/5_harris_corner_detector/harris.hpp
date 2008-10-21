#ifndef _HARRIS_H
#define _HARRIS_H
#include <image_raii.hpp>

/**
 * Performs Harris Corner Detection.  Marks corners with red pixels.
 */
ImageRAII detectCorner( IplImage * image, int win, float sigma );
ImageRAII makePyramid( IplImage * image, float sigma );

#endif
