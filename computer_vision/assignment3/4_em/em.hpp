#ifndef _EM_HPP
#define _EM_HPP

#include <cv.h>
#include <matrix_raii.hpp>

void em( CvMat * data, int nClusters, int nTries, CvSize image_size );
MatrixRAII convert_data( IplImage * image );
CvPoint random_point( CvSize image_size );

#endif
