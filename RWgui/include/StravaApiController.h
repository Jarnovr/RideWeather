#pragma once
#include <string>
#include <QObject>
#include <QThread>
#include "StravaApi.h"


namespace RideWeather
{

	class StravaApiController_t :
		public QObject
	{
		Q_OBJECT
	    QThread workerThread;

	public:
		StravaApiController_t(StravaApi_t *stravaApi);
		~StravaApiController_t();
		void GetAthlete(const ptrdiff_t id) { emit GetAthleteSignal(id); };
	public slots:
		void AthleteReady(const std::shared_ptr<RideWeather::Athlete_t> & athl_str) { emit AthleteReadySignal(athl_str); };

	signals:
		void GetAthleteSignal(const ptrdiff_t);
		void AthleteReadySignal(const std::shared_ptr<RideWeather::Athlete_t> &);

	protected:
		StravaApi_t *_stravaApi;

	};

	class GetAthleteWorker : public QObject
	{
		Q_OBJECT
	public:
		GetAthleteWorker(StravaApi_t *stravaApi) : _stravaApi(stravaApi) {};
	public slots:
		void GetAthlete(const ptrdiff_t id);
	signals:
		void AthleteReady(const std::shared_ptr<RideWeather::Athlete_t>& athlete);
	protected:
		StravaApi_t *_stravaApi;
	};

}