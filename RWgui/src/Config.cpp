#include "Config.h"

#include <QStandardPaths>
#include <string>
#include <boost/filesystem/fstream.hpp>
#include <iostream>


namespace RideWeather
{
	Configuration::Configuration()
	{
		//load configuration from disk if exists, otherwise use sensible defaults.
		QString tmpString = QStandardPaths::locate(QStandardPaths::AppConfigLocation, "RideWeather.cnf");
		std::cout << tmpString.toStdString() << std::endl;
		
		if (tmpString.isEmpty())
		{
			//Config file does not exist yet. Create it.
			configFileName=QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toUtf8().toStdString();
			configFileName.append("/RideWeather.cnf");
			if (!boost::filesystem::exists(configFileName.parent_path()))
			{
				boost::filesystem::create_directories(configFileName.parent_path());//create folder
			}
			//Create empty file
			boost::filesystem::ofstream configFile(configFileName);
			configFile << "" << std::endl;
			configFile.close();
		}
		else
		{
			configFileName = tmpString.toUtf8().toStdString();
		}

		dataFolder= QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toUtf8().toStdString();
		std::cout << dataFolder << std::endl;
		if (!boost::filesystem::exists(dataFolder))
		{
			boost::filesystem::create_directories(dataFolder);//create folder
		}

		cacheFolder = QStandardPaths::writableLocation(QStandardPaths::CacheLocation).toUtf8().toStdString();
		std::cout << cacheFolder << std::endl;
		if (!boost::filesystem::exists(cacheFolder))
		{
			boost::filesystem::create_directories(cacheFolder);//create folder
		}


	}

}