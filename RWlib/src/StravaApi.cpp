#include "StravaApi.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "cpr/cpr.h"


namespace RideWeather
{
	const static cpr::Timeout timeout{ 10000 };

	void StravaApi_t::ProcessResponse(cpr::Header & hdr)
	{
		auto hdrlimit = hdr.find("X-Ratelimit-Limit");
		auto hdrusage = hdr.find("X-Ratelimit-Usage");
		last_usage = boost::posix_time::microsec_clock::universal_time();
		string lim = hdrlimit->second;
		size_t idx;
		short_limit = stoi(lim, &idx);
		long_limit = stoi(lim.substr(idx + 1), &idx);
		string us = hdrusage->second;
		short_usage = stoi(us, &idx);
		long_usage = stoi(us.substr(idx + 1), &idx);
	}

	void StravaApi_t::WaitIfNeeded()
	{
		if (ExceptInsteadWait && ((long_usage >= long_limit) || (short_usage >= short_limit)))
			throw StravaException_t("StravaApi_t: Rate Limit Exceeded.");

		//check daily limit
		if (long_usage >= long_limit)
		{
			std::cerr << "Daily limit reached, waiting till midnight UTC." << std::endl;
			//make endtime
			boost::posix_time::ptime endWait(last_usage.date() + boost::gregorian::date_duration(1), boost::posix_time::time_duration(0, 0, 1));
			//calc wait time;
			boost::posix_time::time_duration wait = endWait - boost::posix_time::microsec_clock::universal_time();
			//sleep
			std::this_thread::sleep_for(std::chrono::seconds(wait.total_seconds()));
			long_usage = 0; short_usage = 0;
		}
		else if (short_usage >= short_limit)
		{
			std::cerr << "Short limit reached, waiting till next quarter of hour." << std::endl;
			//make endtime
			boost::posix_time::ptime endWait = last_usage;
			int wait_time = 901 - (endWait.time_of_day().total_seconds() % 900); //get seconds in day, modulo quarter, caclulate how long till next.
			//sleep
			std::this_thread::sleep_for(std::chrono::seconds(wait_time));
			short_usage = 0;
		}

	}

	bool StravaApi_t::GetCached(const std::string & key, std::string & out)
	{
		boost::filesystem::path fn = cacheFolder;
		fn.append(key);
		if (!(boost::filesystem::exists(fn) && boost::filesystem::is_regular_file(fn)))
			return false;

		//open file
		boost::filesystem::ifstream file(fn, std::ios::binary | std::ios::ate);
		// get filesize
		size_t fs = file.tellg();
		file.seekg(0, std::ios::beg);
		//reserver memory
		out.reserve(fs);
		//read using iteratior
		out.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()) );
		file.close();
		return true;

	}

	void StravaApi_t::WriteCache(const std::string & key, const std::string & json)
	{
		boost::filesystem::path fn = cacheFolder;
		fn.append(key);
		//create folder if needed
		if (!boost::filesystem::exists(fn.parent_path()))
		{
			boost::filesystem::create_directories(fn.parent_path());//create folder
		}
		//open file
		boost::filesystem::ofstream file(fn, std::ios::binary);
		file << json;
		file.close();

	}

	string StravaApi_t::GetAthlete(const ptrdiff_t id)
	{
		string url_base;
		//Rate Limiting
		WaitIfNeeded();

		if (id == 0)
			url_base.assign("https://www.strava.com/api/v3/athlete");
		else
		{
			url_base.assign("https://www.strava.com/api/v3/athlete/");
			url_base.append(std::to_string(id));
		}
		string auth("Bearer ");
		auth.append(at.access_token, 40);

		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(cpr::Url{ url_base }, header, timeout);
		
		if(r.error.code!=cpr::ErrorCode::OK)
		{
			std::cerr << "HTTP Error." << std::endl;
			std::cerr << "Error_code: " << int(r.error.code) << std::endl;
			std::cerr << "Error_message: " << r.error.message << std::endl;
			//try cached value
			string json;
			if (GetCached(string("/athletes/").append(std::to_string(id)).append("/athlete"), json))
				return json;
			else
				throw StravaException_t("StravaApi::GetAthlete: encountered HTTP error.");
		}
		ProcessResponse(r.header);//Process rate limiting part of response

		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			//try cached value
			string json;
			if (GetCached(string("/athletes/").append(std::to_string(id)).append("/athlete"), json))
				return json;
			else
				throw StravaException_t("StravaApi::GetAthlete: encountered HTTP error.");
		}
		//put in cache
		WriteCache(string("/athletes/").append(std::to_string(id)).append("/athlete"),r.text);

		return r.text;
	}

	string StravaApi_t::PutAthlete(const string & city, const string & state, const string & country, const char sex, const double & weight)
	{
		//Rate Limiting
		WaitIfNeeded();

		const static cpr::Url url{ "https://www.strava.com/api/v3/athlete" };
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Payload payload({});
		if (city.compare(""))
			payload.AddPair({ "city",city });
		if (state.compare(""))
			payload.AddPair({ "state",state });
		if (country.compare(""))
			payload.AddPair({ "country",country });
		if (sex == 'M' || sex == 'F')
			payload.AddPair({ "sex",sex });
		if (weight > 0)
			payload.AddPair({ "weight",std::to_string(weight) });

		cpr::Response r = cpr::Put(url, header, payload, timeout);
		if (r.error.code != cpr::ErrorCode::OK)
		{
			std::cerr << "HTTP Error." << std::endl;
			std::cerr << "Error_code: " << int(r.error.code) << std::endl;
			std::cerr << "Error_message: " << r.error.message << std::endl;
			throw StravaException_t("StravaPutAthlete: encountered HTTP error.");
		}
		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaPutAthlete: encountered HTTP error.");
		}
		ProcessResponse(r.header);//Process rate limiting part of response

		return r.text;
	}

	string StravaApi_t::GetZones()
	{
		//Rate Limiting
		WaitIfNeeded();


		const static cpr::Url url{ "https://www.strava.com/api/v3/athlete/zones" };
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(url, header, timeout);
		if (r.error.code != cpr::ErrorCode::OK)
		{
			std::cerr << "HTTP Error." << std::endl;
			std::cerr << "Error_code: " << int(r.error.code) << std::endl;
			std::cerr << "Error_message: " << r.error.message << std::endl;
			string json;
			if (GetCached(string("/athletes/").append("/zones"), json))
				return json;
			else
				throw StravaException_t("StravaApi::GetZones: encountered HTTP error.");
		}
		ProcessResponse(r.header);//Process rate limiting part of response

		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			string json;
			if (GetCached(string("/athletes/").append("/zones"), json))
				return json;
			else
				throw StravaException_t("StravaApi::GetZones: encountered HTTP error.");
		}
		//put in cache
		WriteCache(string("/athletes/").append("/zones"), r.text);

		return r.text;
	}

	string StravaApi_t::GetStats()
	{
		//Rate Limiting
		WaitIfNeeded();

		cpr::Url url{ "https://www.strava.com/api/v3/athletes/" };
		url.append(std::to_string(at.athlete->id));
		url.append("/stats");
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(url, header, timeout);
		if (r.error.code != cpr::ErrorCode::OK)
		{
			std::cerr << "HTTP Error." << std::endl;
			std::cerr << "Error_code: " << int(r.error.code) << std::endl;
			std::cerr << "Error_message: " << r.error.message << std::endl;
			string json;
			if (GetCached(string("/athletes/").append("/stats"), json))
				return json;
			else
				throw StravaException_t("StravaApi::GetStats: encountered HTTP error.");
		}
		ProcessResponse(r.header);//Process rate limiting part of response
		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			string json;
			if (GetCached(string("/athletes/").append("/stats"), json))
				return json;
			else
				throw StravaException_t("StravaApi::GetStats: encountered HTTP error.");
		}
		//put in cache
		WriteCache(string("/athletes/").append("/stats"), r.text);
		return r.text;
	}

	string StravaApi_t::CreateManualActivity(const string& name, const ActivityType_t type,
		const TimeS_t& start_date_local, ptrdiff_t elapsed_time, const string& description,
		const double distance, const bool private_act, const bool trainer, const bool commute)
	{
		//Rate Limiting
		WaitIfNeeded(); 
		const static cpr::Url url{ "https://www.strava.com/api/v3/activities" };
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Payload payload({ {"name",name},{"start_date_local",boost::posix_time::to_iso_extended_string(start_date_local)},
			{"elapsed_time",std::to_string(elapsed_time)}, {"type", ActivityType(type)} });

		if (description.compare(""))
			payload.AddPair({ "description",description });
		if (distance > 0)
			payload.AddPair({ "distance",std::to_string(distance) });
		if (private_act)
			payload.AddPair({ "private","1" });
		if (trainer)
			payload.AddPair({ "trainer","1" });
		if (commute)
			payload.AddPair({ "commmute","1" });

		cpr::Response r = cpr::Post(url, header, payload, timeout);
		if (r.error.code != cpr::ErrorCode::OK)
		{
			std::cerr << "HTTP Error." << std::endl;
			std::cerr << "Error_code: " << int(r.error.code) << std::endl;
			std::cerr << "Error_message: " << r.error.message << std::endl;
			throw StravaException_t("StravaApi::CreateManualActivity: encountered HTTP error.");
		}
		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaApi::CreateManualActivity: encountered HTTP error.");
		}
		ProcessResponse(r.header);//Process rate limiting part of response

		return r.text;
	}

	string StravaApi_t::GetActivity(const ptrdiff_t id)
	{
		string json;
		//Return cached activity if available
		if (GetCached(string("/activity/").append(std::to_string(id)).append("/activity"), json))
			return json;

		//Rate Limiting
		WaitIfNeeded();

		cpr::Url url{ "https://www.strava.com/api/v3/activities/" };
		url.append(std::to_string(id));
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(url, header, timeout);
		if (r.error.code != cpr::ErrorCode::OK)
		{
			std::cerr << "HTTP Error." << std::endl;
			std::cerr << "Error_code: " << int(r.error.code) << std::endl;
			std::cerr << "Error_message: " << r.error.message << std::endl;
			throw StravaException_t("StravaApi::GetActivity: encountered HTTP error.");
		}
		ProcessResponse(r.header);//Process rate limiting part of response
		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaApi::GetActivity: encountered HTTP error.");
		}
		WriteCache(string("/activity/").append(std::to_string(id)).append("/activity"), r.text);
		return r.text;
	}

	string StravaApi_t::GetActivityStream(ptrdiff_t id, string types, string resolution)
	{
		string json;
		//Return cached activity if available
		if (GetCached(string("/activity/").append(std::to_string(id)).append("/streams/").append(types), json))
			return json;
		//Rate Limiting
		WaitIfNeeded();

		cpr::Url url{ "https://www.strava.com/api/v3/activities/" };
		url.append(std::to_string(id));
		url.append("/streams/");
		url.append(types);
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(url, header, timeout);
		if (r.error.code != cpr::ErrorCode::OK)
		{
			std::cerr << "HTTP Error." << std::endl;
			std::cerr << "Error_code: " << int(r.error.code) << std::endl;
			std::cerr << "Error_message: " << r.error.message << std::endl;
			throw StravaException_t("StravaApi::GetStreams: encountered HTTP error.");
		}
		ProcessResponse(r.header);//Process rate limiting part of response

		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaApi::GetStreams: encountered HTTP error.");
		}
		WriteCache(string("/activity/").append(std::to_string(id)).append("/streams/").append(types), r.text);

		return r.text;
	}

} //name space