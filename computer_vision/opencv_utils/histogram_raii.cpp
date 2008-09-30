#include <histogram_raii.hpp>

HistogramRAII::HistogramRAII()
{
	this->histogram = NULL;
}

HistogramRAII::HistogramRAII( CvHistogram * histogram )
{
	this->histogram = histogram;
}

HistogramRAII::~HistogramRAII()
{
	cvReleaseHist( &this->histogram );
}
