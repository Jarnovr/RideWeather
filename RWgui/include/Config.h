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
		const boost::filesystem::path getToken()& { return token; };
		void setToken(const boost::filesystem::path token_);

	private:
		QSettings *settings;
		boost::filesystem::path token;
	};


}