#pragma once
#include <string>
#include <QObject>
#include <QThread>
#include "StravaApi.h"


namespace RideWeather
{


	class StravaApiWorker : public QObject
	{
		Q_OBJECT
	private:
		StravaApiWorker() {};
		StravaApiWorker(const StravaApiWorker&) {};
		StravaApiWorker(StravaApi_t *stravaApi) : _stravaApi(stravaApi) { };
		static StravaApiWorker* _instance;
	public:
		static StravaApiWorker* GetWorker(StravaApi_t *stravaApi)
		{
			if (_instance == nullptr)
			{
				_instance = new StravaApiWorker(stravaApi);
			}
			return _instance;
		}
		virtual ~StravaApiWorker() { _instance = nullptr; }
	public slots:
		void GetAthlete(const ptrdiff_t id) const;
		void GetList(const std::shared_ptr<RideWeather::Athlete_t>& athlete) const;
		void DownloadDetail(const std::shared_ptr<RideWeather::Athlete_t>& athlete) const;
	signals:
		void AthleteReady(const std::shared_ptr<RideWeather::Athlete_t>& athlete) const;
		void ListReady() const;
		void DownloadDetailReady() const;
		void SignalProgress(int value) ;
	protected:
		StravaApi_t *_stravaApi;
		
		void SetProgressHelper(int val) { emit SignalProgress(val); };
		static void SetProgress(int val) { _instance->SetProgressHelper(val); };
	};


	class StravaApiController_t :
		public QObject
	{
		Q_OBJECT
	    QThread workerThread;

	public:
		StravaApiController_t(StravaApi_t *stravaApi);
		~StravaApiController_t();
		const StravaApiWorker *const Worker() const { return stravaApiWorker; } ;
	protected:
		StravaApi_t *_stravaApi;
		StravaApiWorker *stravaApiWorker;
	};

}