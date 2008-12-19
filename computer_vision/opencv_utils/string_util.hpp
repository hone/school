#ifndef _STRING_UTIL_H
#define _STRING_UTIL_H

#include <string>
#include <vector>
#include <sstream>

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

#endif
