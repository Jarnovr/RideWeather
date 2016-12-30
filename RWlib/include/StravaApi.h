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



	string StravaGetAthlete(const AccessToken_t& at, const ptrdiff_t id=0);



}