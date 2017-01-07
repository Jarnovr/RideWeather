#include "StravaApi.h"

#include <iostream>

#include "cpr/cpr.h"


namespace RideWeather
{
	RateLimiter_t rate_limiter;
	const static cpr::Timeout timeout{ 10000 };

	void RateLimiter_t::ProcessResponse(cpr::Header & hdr)
	{
		auto hdrlimit = hdr.find("X-Ratelimit-Limit");
		auto hdrusage = hdr.find("X-Ratelimit-Usage");
		string lim=hdrlimit->second;
		size_t idx;
		short_limit = stoi(lim, &idx);
		long_limit = stoi(lim.substr(idx + 1), &idx);
		string us = hdrusage->second;
		short_usage = stoi(us, &idx);
		long_usage= stoi(us.substr(idx + 1), &idx);
	}

	bool RateLimiter_t::ShouldWait()
	{
		if ((short_usage >= short_limit) || (long_usage >= long_limit))
			return true;
		return false;
	}

	void RateLimiter_t::Wait()
	{
		throw StravaException_t("RateLimiter_t::Wait() To BE implemented");
	}

	string StravaGetAthlete(const AccessToken_t & at, const ptrdiff_t id)
	{
		string url_base;
		if (id == 0)
			url_base.assign("https://www.strava.com/api/v3/athlete");
		else
		{
			url_base.assign("https://www.strava.com/api/v3/athlete/");
			url_base.append(std::to_string(id));
		}
		string auth("Bearer ");
		auth.append(at.access_token,40);

		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(cpr::Url{ url_base }, header, timeout);
		
		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaGetAthlete: encountered HTTP error.");
		}
		rate_limiter.ProcessResponse(r.header);//Process rate limiting part of response

		
		return r.text;
	}

	string StravaPutAthlete(const AccessToken_t & at, const string & city, const string & state, const string & country, const char sex, const double & weight)
	{
		const static cpr::Url url{"https://www.strava.com/api/v3/athlete"};
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Payload payload({});
		if (city.compare(""))
			payload.AddPair({"city",city});
		if (state.compare(""))
			payload.AddPair({ "state",state });
		if (country.compare(""))
			payload.AddPair({ "country",country });
		if (sex == 'M' || sex == 'F')
			payload.AddPair({"sex",sex});
		if (weight > 0)
			payload.AddPair({"weight",std::to_string(weight)});

		cpr::Response r = cpr::Put(url,header,payload, timeout);
		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaPutAthlete: encountered HTTP error.");
		}
		rate_limiter.ProcessResponse(r.header);//Process rate limiting part of response
		
		return r.text;
	}

	string StravaGetZones(const AccessToken_t & at)
	{
		const static cpr::Url url{ "https://www.strava.com/api/v3/athlete/zones" };
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(url, header, timeout);
		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaGetZones: encountered HTTP error.");
		}
		rate_limiter.ProcessResponse(r.header);//Process rate limiting part of response
		return r.text;
	}

	string StravaGetStats(const AccessToken_t& at)
	{
		cpr::Url url{ "https://www.strava.com/api/v3/athletes/" };
		url.append(std::to_string(at.athlete->id));
		url.append("/stats");
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(url, header, timeout);
		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaGetStats: encountered HTTP error.");
		}
		rate_limiter.ProcessResponse(r.header);//Process rate limiting part of response
		return r.text;
	}

	string StravaCreateManualActivity(const AccessToken_t& at, const string& name, const ActivityType_t type,
		const TimeS_t& start_date_local, ptrdiff_t elapsed_time, const string& description,
		const double distance, const bool private_act, const bool trainer, const bool commute)
	{
		const static cpr::Url url{ "https://www.strava.com/api/v3/activities" };
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Payload payload({ {"name",name},{"start_date_local",start_date_local},
			{"elapsed_time",std::to_string(elapsed_time)}, {"type", ActivityType(type)} });
		
		if (description.compare(""))
			payload.AddPair({ "description",description });
		if (distance>0)
			payload.AddPair({ "distance",std::to_string(distance) });
		if (private_act)
			payload.AddPair({ "private","1" });
		if (trainer)
			payload.AddPair({ "trainer","1" });
		if (commute)
			payload.AddPair({ "commmute","1" });

		cpr::Response r = cpr::Post(url, header, payload, timeout);
		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaCreateManualActivity: encountered HTTP error.");
		}
		rate_limiter.ProcessResponse(r.header);//Process rate limiting part of response

		return r.text;
	}

	string StravaGetActivity(const AccessToken_t & at, const ptrdiff_t id)
	{
		cpr::Url url{ "https://www.strava.com/api/v3/activities/" };
		url.append(std::to_string(id));
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(url, header, timeout);

		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaGetActivity: encountered HTTP error.");
		}
		rate_limiter.ProcessResponse(r.header);//Process rate limiting part of response
		return r.text;
	}

	string StravaGetActivityStream(const AccessToken_t& at, ptrdiff_t id, string types, string resolution)
	{

		cpr::Url url{ "https://www.strava.com/api/v3/activities/" };
		url.append(std::to_string(id));
		url.append("/streams/");
		url.append(types);
		string auth("Bearer ");
		auth.append(at.access_token, 40);
		cpr::Header header{ { "Authorization", auth } };
		cpr::Response r = cpr::Get(url, header, timeout);

		if (r.status_code >= 400)
		{
			std::cerr << "Received HTTP Error." << std::endl;
			std::cerr << "Response_code: " << r.status_code << std::endl;
			std::cerr << "Response: " << r.text << std::endl;
			throw StravaException_t("StravaGetStreams: encountered HTTP error.");
		}
		rate_limiter.ProcessResponse(r.header);//Process rate limiting part of response
		return r.text;
	}

} //name space