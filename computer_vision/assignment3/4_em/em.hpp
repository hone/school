#ifndef _EM_HPP
#define _EM_HPP

#include <cv.h>
#include <matrix_raii.hpp>
#include <vector>

// performs em algorithm
void em( CvMat * data, int nClusters, int nTries, IplImage * image );
// performs expectation step of the em algorithm
MatrixRAII expectation( CvMat * data, int nClusters, std::vector<CvMat *> &means, std::vector<CvMat *> &variances, std::vector<double> &weights );
// creates a point given the red / green values
MatrixRAII create_point( double red, double green );
// calculates the normal distribution
double normal_distribution( double z, CvMat * x, CvMat * mean, CvMat * variance );
// performs maximization step of the em algorithm
void maximization( CvMat * r, CvMat * data, std::vector<CvMat *> &means, std::vector<CvMat *> &variances, std::vector<double> &weights );
// converts an image to a matrix of size [2 x # of pixels]
MatrixRAII convert_data( IplImage * image );
// generates a random point within the bounds of the image_size
CvPoint random_point( CvSize image_size );
// given the [2 x # of pixels] matrix, generate a graph
void display_data( CvMat * data, std::vector<CvMat *> means, std::vector<CvMat *> variances, std::vector<double> weights, CvSize image_size );

#endif
