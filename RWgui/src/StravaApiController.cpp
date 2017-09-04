#include "StravaApiController.h"

#include <exception>
#include <iostream>

namespace RideWeather
{

	StravaApiController_t::StravaApiController_t(StravaApi_t *stravaApi)
	{
		if (stravaApi == nullptr)
			throw StravaException_t("Null pointer argument: stravaApi.");
		_stravaApi = stravaApi;

		qRegisterMetaType<ptrdiff_t>("ptrdiff_t");
		qRegisterMetaType<std::shared_ptr<RideWeather::Athlete_t>>("std::shared_ptr<RideWeather::Athlete_t>");

		stravaApiWorker = StravaApiWorker::GetWorker(stravaApi);
		stravaApiWorker->moveToThread(&workerThread);
		connect(&workerThread, &QThread::finished, stravaApiWorker, &QObject::deleteLater);
				
		workerThread.start();
	}


	StravaApiController_t::~StravaApiController_t()
	{
		delete stravaApiWorker;
		workerThread.quit();
		workerThread.wait();
	}
	StravaApiWorker* StravaApiWorker::_instance = nullptr;

	void StravaApiWorker::GetAthlete(const ptrdiff_t id) const
	{
		try
		{
			auto athlete = std::make_shared<RideWeather::Athlete_t>(_stravaApi->GetAthlete(id));
			_stravaApi->LoadAthleteActivitiesList(*athlete, &StravaApiWorker::SetProgress);
			emit AthleteReady(athlete);
		}
		catch (std::exception e)
		{
			std::cerr << "An exception occured during StravaApiWOrker::GetAthlete(): " << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}

	void StravaApiWorker::GetList(const std::shared_ptr<RideWeather::Athlete_t>& athlete) const
	{
		try
		{
			_stravaApi->RefreshAthleteActivities(*athlete, &StravaApiWorker::SetProgress);
			emit ListReady();
		}
		catch (std::exception e)
		{
			std::cerr << "An exception occured during StravaApiWOrker::GetList(): " << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}

	void StravaApiWorker::DownloadDetail(const std::shared_ptr<RideWeather::Athlete_t>& athlete) const
	{
		try
		{
			_stravaApi->GetAthleteActivityStreams(*athlete, &StravaApiWorker::SetProgress);
			emit DownloadDetailReady();
		}
		catch (std::exception e)
		{
			std::cerr << "An exception occured during StravaApiWOrker::DownloadDetail(): " << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}
}