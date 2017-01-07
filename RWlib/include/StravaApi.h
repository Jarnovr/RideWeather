#pragma once

#include "StravaType.h"

#include <string>

#include "cpr/cpr.h"

namespace RideWeather
{
	class RateLimiter_t 
	{
	public:
		void ProcessResponse(cpr::Header& hdr);
		bool ShouldWait();
		void Wait();
		RateLimiter_t() : short_limit(600), long_limit(30000), short_usage(0),
			long_usage(0), last_usage(0) {};
	protected:
		ptrdiff_t short_limit;
		ptrdiff_t long_limit;
		ptrdiff_t short_usage;
		ptrdiff_t long_usage;
		ptrdiff_t last_usage;
	};


	///Athelete page
	string StravaGetAthlete(const AccessToken_t& at, const ptrdiff_t id=0);
	string StravaPutAthlete(const AccessToken_t& at, const string& city = "",
		const string& state = "", const string& country = "", const char sex = '\0', const double& weight = 0.0);
	string StravaGetZones(const AccessToken_t& at);
	string StravaGetStats(const AccessToken_t& at);

	//Activity page
	string StravaCreateManualActivity(const AccessToken_t& at, const string& name, const ActivityType_t type,
		const TimeS_t& start_date_local, ptrdiff_t elapsed_time, const string& description = "",
		const double distance = -1.0, const bool private_act = false,
		const bool trainer = false, const bool commute = false);
	string StravaGetActivity(const AccessToken_t & at, const ptrdiff_t id);
	string StravaGetActivities(const AccessToken_t& at, ptrdiff_t before=-1, ptrdiff_t after=-1, ptrdiff_t page=-1, ptrdiff_t per_page=-1);

	//Streams
	string StravaGetActivityStream(const AccessToken_t& at, ptrdiff_t id, string types, string resolution="");
}