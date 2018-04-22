#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <map>

namespace RideWeather
{
	class WeatherSource_t
	{
	public:
		enum class Resolution { Hour, TenMinutes, Undefined };
		Resolution resolution;

		virtual void GetDataForPeriod(const boost::posix_time::ptime begin, const boost::posix_time::ptime end) = 0;
		virtual float GetTemp(const boost::posix_time::ptime time) = 0;
		virtual float GetPressure(const boost::posix_time::ptime time) = 0;
		virtual float GetWindDir(const boost::posix_time::ptime time) = 0;
		virtual float GetWindSpeed(const boost::posix_time::ptime time) = 0;
		virtual float GetHumidity(const boost::posix_time::ptime time) = 0;

		WeatherSource_t(const Resolution resolution_);
		virtual ~WeatherSource_t();
	};


	class KnmiTenMin : WeatherSource_t
	{
	private:
		struct KnmiTenMinRecord
		{
			boost::posix_time::ptime time;
			float temp;
			float pressure;
			float wind_dir;
			float wind_speed;
			float wind_scale;
			uint16_t visibility;
			uint8_t humidity;
		};
		std::map<boost::posix_time::ptime, KnmiTenMinRecord> records;
		boost::posix_time::ptime RoundTime(const boost::posix_time::ptime input);
		auto GetRecordForTime(const boost::posix_time::ptime time);
	public:
		KnmiTenMin(const boost::filesystem::path& dir);
		virtual ~KnmiTenMin() {};

		void GetDataForPeriod(const boost::posix_time::ptime begin, const boost::posix_time::ptime end);
		float GetTemp(const boost::posix_time::ptime time);
		float GetPressure(const boost::posix_time::ptime time);
		float GetWindDir(const boost::posix_time::ptime time);
		float GetWindSpeed(const boost::posix_time::ptime time);
		float GetHumidity(const boost::posix_time::ptime time);
	};

	class KnmiHourly : WeatherSource_t
	{
	public:
		KnmiHourly(const boost::filesystem::path& cacheFolder);
		virtual ~KnmiHourly() {};

		void GetDataForPeriod(const boost::posix_time::ptime begin, const boost::posix_time::ptime end);
		float GetTemp(const boost::posix_time::ptime time);
		float GetPressure(const boost::posix_time::ptime time);
		float GetWindDir(const boost::posix_time::ptime time);
		float GetWindSpeed(const boost::posix_time::ptime time);
		float GetHumidity(const boost::posix_time::ptime time);
	private:
		struct KnmiHourlyRecord
		{
			boost::gregorian::date date;
			unsigned short wind_dir;
			unsigned short wind_speed; // in 0.1 m/s
			unsigned short wind_speed10; //in 0.1 m/s over last 10 min
			unsigned short wind_gust; //in 0.1 m/s
			unsigned short insolation; // In J/cm^2/h = 100/36 W/m^2
			unsigned short rain_sum; //in 0.1 mm / hour
			unsigned short pressure; //in 0.1 hPa sealevel
			short temp; //in 0.1 deg Celsius
			short wet_temp; //in 0.1 deg Celsius
			uint16_t visibility;
			uint8_t hour;
			uint8_t cloud_cover; // in eights
			uint8_t humidity; // in %
			int8_t sun; // in 0.1 hours or -1
			int8_t rain_dur; // in 0.1 hours or -1
			bool fog;
			bool rain;
			bool snow;
			bool thunderstorm;
			bool icing;
		};

		boost::filesystem::path _cacheFolder;
		std::map<boost::posix_time::ptime, KnmiHourlyRecord> records;
		void GetDataForMonth(const boost::gregorian::date startOfMonth);
		bool GetCached(boost::gregorian::date startOfMonth, std::string& stream);
		void WriteCache(boost::gregorian::date startOfMonth, const std::string & stream);
		std::string GetOnline(boost::gregorian::date startOfMonth);
		void ParseEntries(const std::string& text);
		boost::posix_time::ptime RoundTime(const boost::posix_time::ptime input);
		auto GetRecordForTime(const boost::posix_time::ptime time);
	};
}