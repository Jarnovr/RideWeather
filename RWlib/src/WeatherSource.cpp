#include "WeatherSource.h"
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "cpr/cpr.h"

namespace RideWeather
{
	WeatherSource_t::WeatherSource_t(const Resolution resolution_) : resolution(resolution_)
	{
	}

	WeatherSource_t::~WeatherSource_t()
	{
	}

	boost::posix_time::ptime KnmiTenMin::RoundTime(const boost::posix_time::ptime input)
	{
		auto timeOfDay = input.time_of_day().total_seconds();
		timeOfDay = (int32_t)std::round((double)timeOfDay / 600.) * 600;

		return boost::posix_time::ptime(input.date(), boost::posix_time::seconds(timeOfDay));
	}

	KnmiTenMin::KnmiTenMin(const boost::filesystem::path& dir) :WeatherSource_t(Resolution::TenMinutes)
	{
		//find files in directory and parse them into memory
		boost::filesystem::recursive_directory_iterator end;
		for (boost::filesystem::recursive_directory_iterator i(dir); i != end; i++)
		{
			auto path = *i;
			if (!boost::filesystem::is_regular_file(path))
				continue;
			//open file
			
			std::ifstream file(path.path());
			//Read file
			char buffer[128];
			while (!file.eof())
			{
				file.getline(buffer, 128);
				
				std::string line(buffer);
				std::vector<std::string> values;
				boost::split(values, line, [](char c) {return c == ','; });
				if (values.size() != 8)
				{
					std::cerr << "KnmiTenMin: Invalid number of values in string." << std::endl;
					continue;
				}
				auto record = new KnmiTenMinRecord();

				record->time = boost::date_time::parse_delimited_time<boost::posix_time::ptime>(values[0], ' ');
				record->temp = std::stof(values[1]);
				record->pressure = std::stof(values[2]);
				record->wind_dir = std::stof(values[3]);
				record->wind_speed = std::stof(values[4]);
				record->wind_scale = std::stof(values[5]);
				record->visibility = (uint16_t)std::stoi(values[6]);
				record->humidity = (uint8_t)std::stoi(values[7]);
				records.emplace(record->time, *record);
			}
			file.close();
		}
	}

	void KnmiTenMin::GetDataForPeriod(const boost::posix_time::ptime begin, const boost::posix_time::ptime end)
	{
		//Not relevant. Do nothing
	}

	auto KnmiTenMin::GetRecordForTime(const boost::posix_time::ptime time)
	{
		auto actualTime = RoundTime(time);
		auto record = records.find(actualTime);
		return record;
	}

	float KnmiTenMin::GetTemp(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return record->second.temp;
	}

	float KnmiTenMin::GetPressure(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return record->second.pressure;
	}

	float KnmiTenMin::GetWindDir(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return record->second.wind_dir;
	}

	float KnmiTenMin::GetWindSpeed(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return record->second.wind_speed;
	}

	float KnmiTenMin::GetHumidity(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return record->second.humidity;
	}

	KnmiHourly::KnmiHourly(const boost::filesystem::path& cacheFolder) :WeatherSource_t(Resolution::Hour), _cacheFolder(cacheFolder)
	{
	}

	void KnmiHourly::GetDataForPeriod(const boost::posix_time::ptime begin, const boost::posix_time::ptime end)
	{
		//round input date to begin of month
		auto currentMonth = boost::gregorian::date(begin.date().year(), begin.date().month(), 1);

		while (currentMonth < end.date())
		{
			GetDataForMonth(currentMonth);
			currentMonth += boost::gregorian::months(1);
		}
	}

	inline void KnmiHourly::GetDataForMonth(const boost::gregorian::date startOfMonth)
	{
		std::string stream;
		if (!GetCached(startOfMonth, stream))
		{
			stream = GetOnline(startOfMonth);
		}
		ParseEntries(stream);
	}

	inline std::string GetKeyForDate(const boost::gregorian::date startOfMonth)
	{
		return boost::gregorian::to_iso_string(startOfMonth);
	}

	bool KnmiHourly::GetCached(boost::gregorian::date startOfMonth, std::string& stream)
	{
		boost::filesystem::path fn = _cacheFolder;
		fn.append(GetKeyForDate(startOfMonth));
		if (!(boost::filesystem::exists(fn) && boost::filesystem::is_regular_file(fn)))
			return false;

		//open file
		std::ifstream file(fn, std::ios::binary | std::ios::ate);
		// get filesize
		size_t fs = file.tellg();
		file.seekg(0, std::ios::beg);
		//reserver memory
		stream.reserve(fs);
		//read using iteratior
		stream.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		file.close();
		return true;
	}

	void KnmiHourly::WriteCache(boost::gregorian::date startOfMonth, const std::string & stream)
	{
		boost::filesystem::path fn = _cacheFolder;
		fn.append(GetKeyForDate(startOfMonth));
		//create folder if needed
		if (!boost::filesystem::exists(fn.parent_path()))
		{
			boost::filesystem::create_directories(fn.parent_path());//create folder
		}
		//open file
		std::ofstream file(fn, std::ios::binary);
		file << stream;
		file.close();
	}

	std::string KnmiHourly::GetOnline(boost::gregorian::date startOfMonth)
	{
		const static cpr::Timeout timeout{ 10000 };

		cpr::Url url("http://projects.knmi.nl/klimatologie/uurgegevens/getdata_uur.cgi");
		char startParameter[12];
		sprintf(startParameter, "%04u%02u%02u%02u", (int)startOfMonth.year(), startOfMonth.month().as_number(), startOfMonth.day().as_number(), 0);
		char endParameter[12];
		sprintf(startParameter, "%04u%02u%02u%02u", (int)startOfMonth.year(), startOfMonth.month().as_number(), startOfMonth.end_of_month().day().as_number(), 24);
		cpr::Payload payload({ { "start", std::string(startParameter) } });
		payload.Add({ "end", std::string(endParameter) });
		payload.Add({ "vars", "all" });
		payload.Add({ "stns", "344" });

		std::cout << "Accessing: " << url << std::endl;
		
		cpr::Response r = cpr::Post(url, payload, timeout);
		if (r.error.code != cpr::ErrorCode::OK)
		{
			std::cerr << "HTTP Error." << std::endl;
			std::cerr << "Error_code: " << int(r.error.code) << std::endl;
			std::cerr << "Error_message: " << r.error.message << std::endl;
			throw std::runtime_error("KnmiHourly::GetOnline: encountered HTTP error.");
		}

		WriteCache(startOfMonth, r.text);
		return r.text;
	}

	void KnmiHourly::ParseEntries(const std::string & text)
	{
		std::vector<std::string> lines;
		boost::split(lines, text, [](char c) {return c == '\n'; });
		for (auto line : lines)
		{
			if (line.at(0) == '#')
				continue;

			std::vector<std::string> values;
			boost::split(values, line, [](char c) {return c == ','; });

			if (values.size() != 25)
			{
				std::cerr << "KnmiTenMin: Invalid number of values in string." << std::endl;
				continue;
			}
			KnmiHourlyRecord record;

			//first value is station, ignore for now;
			record.date = boost::gregorian::date_from_iso_string(values[1]);
			record.hour = (uint8_t) std::stoi(values[2]);
			record.wind_dir = (unsigned short)std::stoi(values[3]);
			record.wind_speed = (unsigned short)std::stoi(values[4]);
			record.wind_speed10 = (unsigned short)std::stoi(values[5]);
			record.wind_gust = (unsigned short)std::stoi(values[6]);
			record.temp = (short)std::stoi(values[7]);
			//values[8] is minium at ground level last 6 hours
			record.wet_temp = (short)std::stoi(values[9]);
			record.sun = (int8_t)std::stoi(values[10]);
			record.insolation = (unsigned short)std::stoi(values[11]);
			record.rain_dur = (int8_t)std::stoi(values[12]);
			record.rain_sum = (unsigned short)std::stoi(values[13]);
			record.pressure = (unsigned short)std::stoi(values[14]);
			record.visibility = (uint16_t)std::stoi(values[15]);
			record.cloud_cover = (uint8_t)std::stoi(values[16]);
			record.humidity = (uint8_t)std::stoi(values[17]);
			//values[18] is weather code
			//values[19] is indicator of recording type (manual, auto)
			record.fog = (bool)std::stoi(values[20]);
			record.rain = (bool)std::stoi(values[21]);
			record.snow = (bool)std::stoi(values[22]);
			record.thunderstorm = (bool)std::stoi(values[23]);
			record.icing = (bool)std::stoi(values[24]);
			records.emplace(boost::posix_time::ptime(record.date, boost::posix_time::time_duration(record.hour, 0, 0)) ,record);
		}
	}

	boost::posix_time::ptime KnmiHourly::RoundTime(const boost::posix_time::ptime input)
	{
		auto timeOfDay = input.time_of_day().total_seconds();
		timeOfDay = (int32_t)std::round((double)timeOfDay / 3600.) * 3600;

		return boost::posix_time::ptime(input.date(), boost::posix_time::seconds(timeOfDay));
	}


	auto KnmiHourly::GetRecordForTime(const boost::posix_time::ptime time)
	{
		auto actualTime = RoundTime(time);
		auto record = records.find(actualTime);
		return record;
	}

	float KnmiHourly::GetTemp(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return float(record->second.temp) * 10.f;
	}

	float KnmiHourly::GetPressure(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return float(record->second.pressure) * 10.f;
	}

	float KnmiHourly::GetWindDir(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return float(record->second.wind_dir);
	}

	float KnmiHourly::GetWindSpeed(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return float(record->second.wind_speed) *10.f;
	}

	float KnmiHourly::GetHumidity(const boost::posix_time::ptime time)
	{
		auto record = GetRecordForTime(time);
		if (record == records.end())
		{
			return NAN;
		}
		return float(record->second.humidity);
	}
}