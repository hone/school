#ifndef _MOSAIC_H
#define _MOSAIC_H
#include <cv.h>
#include <string>
#include <vector>
#include <sstream>
#include <utility>

/**
 * Given the SIFT descriptors, computes the best match for every point in v1 by finding the piont in v2 that minimizes the Euclidean distance between their corresponding feature vectors.
 * @param v1 n1 by 2 matrix that contains the 2-d image locations of keypoints in image 1
 * @param v2 n2 by 2 matrix that contains the 2-d image locations of keypoints in image 2
 * @param f1 n1 x F matrix containing the feature vectors of the points in v1 in the same order .  For SIFT F = 128
 * @param f2 n2 x F matrix containing the feature vectors of the points in v1 in the same order .  For SIFT F = 128
 */
std::pair<CvMat *, CvMat *> find_correspondences( CvMat * v1, CvMat * v2, CvMat * f1, CvMat * f2 );
// computes the homography of the two points
CvMat * ransac_homography( CvMat * p1, CvMat * p2 );
// stitches two images together
IplImage * stitch( IplImage * i1, IplImage * i2, CvMat * h );
// calculates the random_number
int random_number( int max );
MatrixRAII read_file( const char * filename );
std::vector< std::string > tokenize_str( const std::string & str, const std::string & delims );
template< typename source, typename dest>
dest lexical_cast(source src)
{
  std::stringstream ss;
  dest output;

  ss << src;
  ss >> output; 

  return output;
}

#endif
