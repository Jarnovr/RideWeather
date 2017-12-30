#include "WeatherSource.h"

namespace RideWeather
{
	WeatherSource_t::WeatherSource_t(Resolution resolution_): resolution(resolution_)
	{
	}

	WeatherSource_t::~WeatherSource_t()
	{
	}

	KnmiTenMin::KnmiTenMin(boost::filesystem::path dir) :WeatherSource_t(Resolution::TenMinutes)
	{
	     
	}

	KnmiHourly::KnmiHourly(boost::filesystem::path cacheFolder) :WeatherSource_t(Resolution::Hour), _cacheFolder(cacheFolder)
	{

	}
}