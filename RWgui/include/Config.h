#pragma once

#include <boost/filesystem.hpp>
#include <QSettings>

namespace RideWeather
{
	class Configuration
	{
	public:
		Configuration();
		virtual ~Configuration();
		boost::filesystem::path configFileName;
		boost::filesystem::path dataFolder;
		boost::filesystem::path cacheFolder;


	private:
		QSettings *settings;
	};


}