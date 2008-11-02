#include <window_raii.hpp>

WindowRAII::WindowRAII( const char * window_name, int flags )
{
	this->name = window_name;
	cvNamedWindow( this->name, flags );
}

WindowRAII::~WindowRAII()
{
	cvDestroyWindow( this->name );
}
