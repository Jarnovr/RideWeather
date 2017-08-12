#include "StravaApiController.h"

namespace RideWeather
{

	StravaApiController_t::StravaApiController_t(StravaApi_t *stravaApi)
	{
		if (stravaApi == nullptr)
			throw StravaException_t("Null pointer argument: stravaApi.");
		_stravaApi = stravaApi;

		qRegisterMetaType<ptrdiff_t>("ptrdiff_t");
		qRegisterMetaType<std::shared_ptr<RideWeather::Athlete_t>>("std::shared_ptr<RideWeather::Athlete_t>");

		GetAthleteWorker *getAthleteWorker = new GetAthleteWorker(_stravaApi);
		getAthleteWorker->moveToThread(&workerThread);
		connect(&workerThread, &QThread::finished, getAthleteWorker, &QObject::deleteLater);
		connect(this, &StravaApiController_t::GetAthleteSignal, getAthleteWorker, &GetAthleteWorker::GetAthlete);
		connect(getAthleteWorker, &GetAthleteWorker::AthleteReady, this, &StravaApiController_t::AthleteReady);


		workerThread.start();
	}


	StravaApiController_t::~StravaApiController_t()
	{
		workerThread.quit();
		workerThread.wait();
	}


	void GetAthleteWorker::GetAthlete(const ptrdiff_t id)
	{
		auto athlete = std::make_shared<RideWeather::Athlete_t>(_stravaApi->GetAthlete(id));
		emit AthleteReady(athlete);
	}
}