#include <string_util.hpp>

using namespace std;

vector< string > tokenize_str( const string & str, const string & delims )
{
	// Skip delims at beginning, find start of first token
	std::string::size_type lastPos = str.find_first_not_of(delims, 0); 
	// Find next delimiter @ end of token
	std::string::size_type pos     = str.find_first_of(delims, lastPos);

	// output vector
	std::vector< std::string > tokens;

	while (std::string::npos != pos || std::string::npos != lastPos)
	{   
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delims.  Note the "not_of". this is beginning of token
		lastPos = str.find_first_not_of(delims, pos);
		// Find next delimiter at end of token.
		pos     = str.find_first_of(delims, lastPos);
	}

	return tokens;
}

