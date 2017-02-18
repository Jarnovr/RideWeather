#pragma once

#include <boost/filesystem.hpp>


namespace RideWeather
{
	class Configuration
	{
	public:
		Configuration();
		boost::filesystem::path configFileName;
		boost::filesystem::path dataFolder;
		boost::filesystem::path cacheFolder;


	
	};


}