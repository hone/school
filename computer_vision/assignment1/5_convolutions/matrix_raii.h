#include <cv.h>

#ifndef MATRIX_RAII_H
#define MATRIX_RAII_H

class MatrixRAII
{
	public:
		CvMat * matrix;

		/**
		 * Constructor.  Takes in a CvMatrix.
		 * @param mat CvMatrix for the object to use
		 */
		MatrixRAII( CvMat * mat );
		/**
		 * Cleans up the CvMatrix.
		 */
		~MatrixRAII();

};

#endif
