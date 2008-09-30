#include <cv.h>

#ifndef _HISTOGRAM_RAII
#define _HISTOGRAM_RAII

class HistogramRAII
{
	public:
		CvHistogram * histogram;

		/**
		 * Default constructor.  Sets the histogram to null.
		 */
		HistogramRAII();
		/**
		 * Constructor. Takes in a CvHistogram pointer and sets the member method to it.
		 */
		HistogramRAII( CvHistogram * histogram );
		/**
		 * Destructor.  Cleans up the CvHistogram.
		 */
		~HistogramRAII();
};

#endif
