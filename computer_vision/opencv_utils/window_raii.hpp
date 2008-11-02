#ifndef _WINDOW_RAII_HPP
#define _WINDOW_RAII_HPP

#include <highgui.h>

class WindowRAII
{
	public:
		const char * name;

		/**
		 * Constructor. Takes in the window name and the window flags.  Creates a NamedWindow.
		 */
		WindowRAII( const char * window_name, int flags = CV_WINDOW_AUTOSIZE );
		/**
		 * Destructor.  Cleans up the NamedWindow.
		 */
		~WindowRAII();
};

#endif
