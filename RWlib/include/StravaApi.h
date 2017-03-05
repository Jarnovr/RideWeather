#pragma once

#include "StravaType.h"

#include <string>

#include <boost/filesystem.hpp>
#include "boost/date_time.hpp"

#include "cpr/cpr.h"

namespace RideWeather
{
	class StravaApi_t {
	public:
		bool ExceptInsteadWait;
		StravaApi_t() : ExceptInsteadWait(false), short_limit(600), long_limit(30000), short_usage(0),
			long_usage(0), last_usage(boost::posix_time::microsec_clock::universal_time()) {};
		StravaApi_t(const AccessToken_t& at_, boost::filesystem::path cacheFolder_) : ExceptInsteadWait(false),
			short_limit(600), long_limit(30000), short_usage(0), long_usage(0),
			last_usage(boost::posix_time::microsec_clock::universal_time()), at(at_), cacheFolder(cacheFolder_) {};

		///Athelete page
		std::string GetAthlete(const ptrdiff_t id = 0);
		std::string PutAthlete(const string& city = "",
			const string& state = "", const string& country = "", const char sex = '\0', const double& weight = 0.0);
		std::string GetZones();
		std::string GetStats();

		//Activity page
		std::string CreateManualActivity(const string& name, const ActivityType_t type,
			const TimeS_t& start_date_local, ptrdiff_t elapsed_time, const string& description = "",
			const double distance = -1.0, const bool private_act = false,
			const bool trainer = false, const bool commute = false);
		std::string GetActivity(const ptrdiff_t id);
		std::string ListActivities(ptrdiff_t before = -1, ptrdiff_t after = -1, ptrdiff_t page = -1, ptrdiff_t per_page = 50);

		//Streams
		std::string GetActivityStream(ptrdiff_t id, string types, string resolution = "");
	protected:
		AccessToken_t at;
		boost::filesystem::path cacheFolder;

		ptrdiff_t short_limit;
		ptrdiff_t long_limit;
		ptrdiff_t short_usage;
		ptrdiff_t long_usage;
		boost::posix_time::ptime last_usage;
		void ProcessResponse(cpr::Header& hdr);
		void WaitIfNeeded();
		bool GetCached(const std::string& key, std::string& out);
		void WriteCache(const std::string& key, const std::string& json);
	};
}