// Standard libs
#include <string>

// OpenCV libs
#include <highgui.h>

#ifndef _IMAGE_RAII_H
#define _IMAGE_RAII_H

class ImageRAII
{
	public:
		IplImage * image;

        /**
         * Default constructor.  sets the image pointer to null.
         */
        ImageRAII();

		/**
		 * Construct ImageRAII object using the given filename.
		 * @param filename name of the file.
		 */
		ImageRAII( std::string filename );
		/**
		 * Construct ImageRAII object using the given image.
		 * @param image image to use
		 */
		ImageRAII( IplImage * image );
		/**
		 * Releases image.
		 */
		~ImageRAII();

	private:
		/**
		 * Check if image loaded
		 */
		void checkImageLoad( std::string message );
};

#endif
