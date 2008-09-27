// Standard libs
#include <string>

// OpenCV libs
#include <highgui.h>

#ifndef _IMAGE_H
#define _IMAGE_H

class ImageLoader
{
	public:
		IplImage * image;

		/**
		 * Construct ImageLoader object using the given filename.
		 * @param filename name of the file.
		 */
		ImageLoader( std::string filename );
		/**
		 * Construct ImageLoader object using the given image.
		 * @param image image to use
		 */
		ImageLoader( IplImage * image );
		/**
		 * Releases image.
		 */
		~ImageLoader();

	private:
		/**
		 * Check if image loaded
		 */
		void checkImageLoad( std::string message );
};

#endif
