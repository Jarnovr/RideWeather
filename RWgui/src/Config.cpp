#include "Config.h"

#include <string>
#include <iostream>
#include <QStandardPaths>


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

		//read other settings from file
		settings = new QSettings(QString::fromStdString(configFileName.generic_string()),QSettings::IniFormat);
		token = settings->value("Athlete/token","").toString().toUtf8().toStdString();

	}

	Configuration::~Configuration()
	{
		delete settings;
	}

	void Configuration::setToken(const boost::filesystem::path token_)
	{
		token = token_;
		settings->setValue("Athlete/token",QString::fromStdString(token_.generic_string()));
	}



}