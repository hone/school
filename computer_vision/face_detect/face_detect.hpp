#include <cv.h>
#include <vector>
#include <string>
#include <sstream>

// Page 511 to 512 of the O'Reilly Learning OpenCV book
// Detect and draw detected object boxes on image
// Presumes 2 Globals:
//  Cascade is loaded by:
//		cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
//	AND that storage is allocated:
//	CvMemStorage* storage = cvCreateMemStorage(0);
void detect_and_draw( IplImage * img, const char * filename, double scale = 1.3 );

/**
 * String tokenizer taken from http://www.rosettacode.org/wiki/Tokenizing_A_String#C.2B.2B
 */
std::vector<std::string> tokenize_str( const std::string & str, const std::string & delims );

/**
 * Typecasts from source to destination.
 */
template< typename source, typename dest>
dest lexical_cast(source src)
    {
        std::stringstream ss;
        dest output;

        ss << src;
        ss >> output;
    
        return output;
    }
