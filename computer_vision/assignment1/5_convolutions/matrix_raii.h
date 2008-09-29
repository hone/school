#include <cv.h>

#ifndef MATRIX_RAII_H
#define MATRIX_RAII_H

class MatrixRAII
{
	public:
		CvMat * matrix;

		/**
		 * Default constructor. Sets matrix to null.
		 */
		MatrixRAII();
		/**
		 * Constructor.  Takes in a CvMat.
		 * @param mat CvMatrix for the object to use
		 */
		MatrixRAII( CvMat * mat );
		/**
		 * Cleans up the CvMat.
		 */
		~MatrixRAII();

};

#endif
