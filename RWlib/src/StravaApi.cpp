#include "StravaApi.h"

#include <iostream>

#include "cpr/cpr.h"


namespace RideWeather
{
	RateLimiter_t rate_limiter;


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

		cpr::Header header{ {"Authorization", auth} };
		cpr::Response r = cpr::Get(cpr::Url{ url_base }, header, cpr::Timeout{10000});
		
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

}