#ifndef _HARRIS_H
#define _HARRIS_H
#include <image_raii.hpp>

ImageRAII detectCorner( IplImage * image, int win, float sigma );

#endif
