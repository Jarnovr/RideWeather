#define RAPIDJSON_SSE42
#define _CRT_SECURE_NO_WARNINGS
#include "StravaType.h"

#include <iostream>
#include <string>

#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
using namespace rapidjson;
using namespace std;

#include "StravaType.h"


namespace RideWeather
{

	void Strava_t::ParseJson(const string & json)
	{
		//First create rapidjson object
		document->Parse<ParseFlag::kParseFullPrecisionFlag | ParseFlag::kParseCommentsFlag | ParseFlag::kParseNanAndInfFlag>(json);
		//generic error checking
		if (document->HasParseError())
		{
			cerr << "JSON parse error: " << GetParseError_En(document->GetParseError());
			cerr << "(" << document->GetErrorOffset() << ")" << endl;

			throw StravaException_t(type_str.append("::").append(type_str).append("JSON parser error.\n"));
		}

		//go over returned dom and do error/sanity check and fill the data structure
		if (!document->IsObject())
			throw StravaException_t(type_str.append("::").append(type_str).append(", returned DOM no object\n"));
		//dom->Swap(document);
		dom->CopyFrom(*document, document->GetAllocator());
	}

	void Athlete_t::MakeActivityIds()
	{
		activity_ids.clear();
		activity_ids.reserve(activities.size());
		for (auto& v : activities)
		{
			activity_ids.push_back(v.first);
		}
	}

	bool Athlete_t::isActIdListOutDated() const
	{
		return activities.size() != activity_ids.size();
	}

	void Athlete_t::ParseDom()
	{
		string tmp;
		//DebugPrintDom();

		id = ParseInt64("id");
		resource_state = ParseInt64("resource_state");

		if (resource_state <= 0 || resource_state > 3)
			throw StravaException_t(string("Invalid resource_state.").append(std::to_string(resource_state).append(".\n")));

		switch (resource_state)
		{
		case 3:
			follower_count = ParseInt64("follower_count");
			friend_count = ParseInt64("friend_count");
			mutual_friend_count = ParseInt64("mutual_friend_count");
			athlete_type = AthleteType_t(ParseInt64("athlete_type"));
			ParseString(date_preference, "date_preference");
			ParseString(tmp, "measurement_preference");
			if (!tmp.compare("feet"))
				measurement_preference = MeasurementType_t::feet;
			else if (!tmp.compare("meters"))
				measurement_preference = MeasurementType_t::meters;
			else
				throw StravaException_t("Athlete_t measurement_preference not feet or meters");
			ParseString(email, "email");
			ftp = ParseInt64("ftp");
			weight = ParseDouble("weight");

			if (!dom->HasMember("clubs"))
				throw StravaException_t("Athlete_t missing clubs list");
			if (!(*dom)["clubs"].IsArray())
				throw StravaException_t("Athlete_t clubs not array");
			for (auto& v : (*dom)["clubs"].GetArray())
			{
				clubs.push_back(Club_t(v));
			}

			if (!dom->HasMember("bikes"))
				throw StravaException_t("Athlete_t missing bikes list");
			if (!(*dom)["bikes"].IsArray())
				throw StravaException_t("Athlete_t bikes not array");
			for (auto& v : (*dom)["bikes"].GetArray())
			{
				bikes.push_back(Bike_t(v));
			}
			if (!dom->HasMember("shoes"))
				throw StravaException_t("Athlete_t missing shoes list");
			if (!(*dom)["shoes"].IsArray())
				throw StravaException_t("Athlete_t shoes not array");
			for (auto& v : (*dom)["shoes"].GetArray())
			{
				shoes.push_back(Shoe_t(v));
			}

		case 2:
			ParseString(firstname, "firstname");
			ParseString(lastname, "lastname");
			ParseString(profile_medium, "profile_medium");
			ParseString(profile, "profile");
			ParseString(city, "city");
			ParseString(state, "state");
			ParseString(country, "country");

			if (!dom->HasMember("sex"))
				throw StravaException_t("Athlete missing sex\n");
			if (!(*dom)["sex"].IsString())
				throw StravaException_t("Athlete_t sex not char\n");
			sex = *(*dom)["sex"].GetString();

			if (!dom->HasMember("friend"))
				throw StravaException_t("Athlete missing friend\n");
			if ((*dom)["friend"].IsNull())
				friend_ = Connection_t::null;
			else
			{
				ParseString(tmp, "friend");
				if (!tmp.compare("pending"))
					friend_ = Connection_t::pending;
				else if (!tmp.compare("accepted"))
					friend_ = Connection_t::accepted;
				else if (!tmp.compare("blocked"))
					friend_ = Connection_t::blocked;
				else friend_ = Connection_t::null;
			}

			if (!dom->HasMember("follower"))
				throw StravaException_t("Athlete missing follower\n");
			if ((*dom)["follower"].IsNull())
				follower = Connection_t::null;
			else
			{
				ParseString(tmp, "follower");
				if (!tmp.compare("pending"))
					follower = Connection_t::pending;
				else if (!tmp.compare("accepted"))
					follower = Connection_t::accepted;
				else if (!tmp.compare("blocked"))
					follower = Connection_t::blocked;
				else follower = Connection_t::null;
			}

			premium = ParseBool("premium");
			ParseTimeS(created_at, "created_at");
			ParseTimeS(updated_at, "updated_at");
		case 1:;
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Zones_t::ParseDom()
	{
		//DebugPrintDom();
		if (!dom->HasMember("heart_rate"))
			throw StravaException_t("Zones_t missing heart_rate");
		if (!(*dom)["heart_rate"].HasMember("custom_zones"))
			throw StravaException_t("Zones_t heart rate has no member custom_zones");
		heart_rate_custom = (*dom)["heart_rate"]["custom_zones"].GetBool();

		if (!(*dom)["heart_rate"].HasMember("zones"))
			throw StravaException_t("Heart_rate missing zones");
		for (auto& v : (*dom)["heart_rate"]["zones"].GetArray())
		{
			zone_t tmp;
			tmp.min = (short)v["min"].GetInt64();
			tmp.max = (short)v["max"].GetInt64();
			heart_zones.push_back(tmp);
		}

		if (dom->HasMember("power"))
		{
			if (!(*dom)["power"].HasMember("zones"))
				throw StravaException_t("power missing zones");
			for (auto& v : (*dom)["power"]["zones"].GetArray())
			{
				zone_t tmp;
				tmp.min = (short)v["min"].GetInt64();
				tmp.max = (short)v["max"].GetInt64();
				power_zones.push_back(tmp);
			}
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Total_t::ParseDom()
	{
		//DebugPrintDom();
		//Parse content
		count = ParseInt64("count");
		distance = ParseDouble("distance");
		moving_time = ParseInt64("moving_time");
		elapsed_time = ParseInt64("elapsed_time");
		elevation_gain = ParseDouble("elevation gain");
		achievement_count = ParseInt64("achievement_count");
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Totals_t::ParseDom()
	{
		//DebugPrintDom();
		biggest_ride_distance = ParseDouble("biggest_ride_distance");
		biggest_climb_elevation_gain = ParseDouble("biggest_climb_elevation");

		if (!dom->HasMember("recent_ride_totals"))
			throw StravaException_t("Totals_t missing recent_ride_totals");
		recent_ride_totals = Total_t((*dom)["recent_ride_totals"]);
		if (!dom->HasMember("recent_run_totals"))
			throw StravaException_t("Totals_t missing recent_run_totals");
		recent_run_totals = Total_t((*dom)["recent_run_totals"]);
		if (!dom->HasMember("recent_swim_totals"))
			throw StravaException_t("Totals_t missing recent_swim_totals");
		recent_swim_totals = Total_t((*dom)["recent_swim_totals"]);

		if (!dom->HasMember("ytd_ride_totals"))
			throw StravaException_t("Totals_t missing ytd_ride_totals");
		ytd_ride_totals = Total_t((*dom)["ytd_ride_totals"]);
		if (!dom->HasMember("ytd_run_totals"))
			throw StravaException_t("Totals_t missing ytd_run_totals");
		ytd_run_totals = Total_t((*dom)["ytd_run_totals"]);
		if (!dom->HasMember("ytd_swim_totals"))
			throw StravaException_t("Totals_t missing ytd_swim_totals");
		ytd_swim_totals = Total_t((*dom)["ytd_swim_totals"]);

		if (!dom->HasMember("all_ride_totals"))
			throw StravaException_t("Totals_t missing all_ride_totals");
		all_ride_totals = Total_t((*dom)["all_ride_totals"]);
		if (!dom->HasMember("all_run_totals"))
			throw StravaException_t("Totals_t missing all_run_totals");
		all_run_totals = Total_t((*dom)["all_run_totals"]);
		if (!dom->HasMember("all_swim_totals"))
			throw StravaException_t("Totals_t missing all_swim_totals");
		all_swim_totals = Total_t((*dom)["all_swim_totals"]);
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Activity_t::ParseDom()
	{
		string tmp;
		//DebugPrintDom();

		id = ParseInt64("id");
		resource_state = ParseInt64("resource_state");

		if (resource_state <= 0 || resource_state > 3)
			throw StravaException_t("Invalid resource_state.\n");

		switch (resource_state)
		{
		case 3:
			calories = ParseDouble("calories");
			ParseString(description, "description");
			if (!dom->HasMember("gear"))
				throw StravaException_t("Activity_t has no member gear.\n");
			gear = make_shared<Gear_t>((*dom)["gear"]);
			if (!dom->HasMember("segment_efforts"))
				throw StravaException_t("Athlete_t missing segment_efforts list");
			if (!(*dom)["segment_efforts"].IsArray())
				throw StravaException_t("Athlete_t segment_efforts not array");
			for (auto& v : (*dom)["segment_efforts"].GetArray())
			{
				segment_efforts.push_back(SegmentEffort_t(v));
			}
			if (dom->HasMember("splits_metric"))
			{

				if (!(*dom)["splits_metric"].IsArray())
					throw StravaException_t("Athlete_t splits_metric not array");
				for (auto& v : (*dom)["splits_metric"].GetArray())
				{
					splits_metric.push_back(Splits_t(v));
				}
			}
			if (dom->HasMember("splits_standard"))
			{

				if (!(*dom)["splits_standard"].IsArray())
					throw StravaException_t("Athlete_t splits_standard not array");
				for (auto& v : (*dom)["splits_metric"].GetArray())
				{
					splits_standard.push_back(Splits_t(v));
				}
			}

			if (dom->HasMember("laps"))
			{
				if (!(*dom)["laps"].IsArray())
					throw StravaException_t("Athlete_t laps not array");
				for (auto& v : (*dom)["laps"].GetArray())
				{
					laps.push_back(Lap_t(v));
				}
			}
			if (dom->HasMember("best_efforts"))
			{
				if (!(*dom)["best_efforts"].IsArray())
					throw StravaException_t("Athlete_t best_efforts not array");
				for (auto& v : (*dom)["best_efforts"].GetArray())
				{
					best_efforts.push_back(Effort_t(v));
				}
			}
			ParseStringIf(device_name, "device_name");
			ParseStringIf(embed_token, "embed_token");
			//skip photos for now

		case 2:
			ParseString(external_id, "external_id");
			upload_id = ParseInt64("upload_id");
			ParseString(name, "name");
			distance = ParseDouble("distance");
			moving_time = ParseInt64("moving_time");
			elapsed_time = ParseInt64("elapsed_time");
			total_elevation_gain = ParseDouble("total_elevation_gain");
			elev_high = ParseDoubleIf("elev_high");
			elev_low = ParseDoubleIf("elev_low");
			ParseString(typeS, "type");
			type = ActivityType(typeS);
			ParseTimeS(start_date, "start_date");
			ParseTimeS(start_date_local, "start_date_local");
			ParseString(timezone_act, "timezone");
			if (!dom->HasMember("start_latlng"))
				throw StravaException_t("Activity_t missing start_latlng");
			start_latlng = Point_t((*dom)["start_latlng"]);
			if (!dom->HasMember("end_latlng"))
				throw StravaException_t("Activity_t missing end_latlng");
			end_latlng = Point_t((*dom)["end_latlng"]);
			achievement_count = ParseInt64If("achievement_count");
			kudos_count = ParseInt64If("kudos_count");
			comment_count = ParseInt64If("comment_count");
			athlete_count = ParseInt64If("athlete_count");
			photo_count = ParseInt64If("photo_count");
			total_photo_count = ParseInt64If("total_photo_count");
			if (dom->HasMember("map"))
				map = make_shared<Map_t>((*dom)["map"]);
			trainer = ParseBoolIf("trainer");
			commute = ParseBoolIf("commute");
			manual = ParseBoolIf("manual");
			private_act = ParseBoolIf("private");
			flagged = ParseBool("flagged");
			workout_type = static_cast<Workout_t> (ParseInt64If("workout_type"));
			ParseStringIf(gear_id, "gear_id");
			average_speed = ParseDoubleIf("average_speed");
			max_speed = ParseDoubleIf("max_speed");
			average_cadence = ParseDoubleIf("average_cadence");
			average_temp = ParseDoubleIf("average_temp");
			average_watts = ParseDoubleIf("average_watts");
			max_watts = ParseIntIf("max_watts");
			weighted_avereage_watts = ParseIntIf("weighted_average_watts");
			kilojoules = ParseDoubleIf("kilojoules");
			device_watts = ParseBoolIf("device_watts");
			has_heartrate = ParseBoolIf("has_heartrate");
			if (has_heartrate)
			{
				average_heartrate = ParseDouble("average_heartrate");
				max_heartrate = ParseDoubleIf("max_heartrate");
			}
			suffer_score = ParseIntIf("suffer_score");
			has_kudoed = ParseBool("has_kudoed");

		case 1:;
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}


	ActivityType_t ActivityType(const string & str)
	{
		if (!str.compare("Ride"))
			return ActivityType_t::Ride;
		if (!str.compare("Run"))
			return ActivityType_t::Run;
		if (!str.compare("Swim"))
			return ActivityType_t::Swim;
		if (!str.compare("Hike"))
			return ActivityType_t::Hike;
		if (!str.compare("Walk"))
			return ActivityType_t::Walk;
		if (!str.compare("AlpineSki"))
			return ActivityType_t::AlpineSki;
		if (!str.compare("BackcountrySki"))
			return ActivityType_t::BackcountrySki;
		if (!str.compare("Canoeing"))
			return ActivityType_t::Canoeing;
		if (!str.compare("Crossfit"))
			return ActivityType_t::Crossfit;
		if (!str.compare("EBikeRide"))
			return ActivityType_t::EBikeRide;
		if (!str.compare("Elliptical"))
			return ActivityType_t::Elliptical;
		if (!str.compare("IceSkate"))
			return ActivityType_t::IceSkate;
		if (!str.compare("InlineSkate"))
			return ActivityType_t::InlineSkate;
		if (!str.compare("Kayaking"))
			return ActivityType_t::Kayaking;
		if (!str.compare("Kitesurf"))
			return ActivityType_t::Kitesurf;
		if (!str.compare("NordicSki"))
			return ActivityType_t::NordicSki;
		if (!str.compare("RockClimbing"))
			return ActivityType_t::RockClimbing;
		if (!str.compare("RollerSki"))
			return ActivityType_t::RollerSki;
		if (!str.compare("Rowing"))
			return ActivityType_t::Rowing;
		if (!str.compare("Snowboard"))
			return ActivityType_t::Snowboard;
		if (!str.compare("Snowshoe"))
			return ActivityType_t::Snowshoe;
		if (!str.compare("StairStepper"))
			return ActivityType_t::StairStepper;
		if (!str.compare("StandUpPaddling"))
			return ActivityType_t::StandUpPaddling;
		if (!str.compare("Surfing"))
			return ActivityType_t::Surfing;
		if (!str.compare("VirtualRide"))
			return ActivityType_t::VirtualRide;
		if (!str.compare("WeightTraining"))
			return ActivityType_t::WeightTraining;
		if (!str.compare("Windsurf"))
			return ActivityType_t::Windsurf;
		if (!str.compare("Workout"))
			return ActivityType_t::Workout;
		if (!str.compare("Yoga"))
			return ActivityType_t::Yoga;
		//else
		return ActivityType_t::other;
	}

	string ActivityType(const ActivityType_t type)
	{
		switch (type)
		{
		case ActivityType_t::Ride:
			return "Ride";
		case ActivityType_t::Run:
			return "Run";
		case ActivityType_t::Swim:
			return "Swim";
		case ActivityType_t::Hike:
			return "Hike";
		case ActivityType_t::Walk:
			return "Walt";
		case  ActivityType_t::AlpineSki:
			return "AlpineSki";
		case ActivityType_t::BackcountrySki:
			return "BackcountrySki";
		case ActivityType_t::Canoeing:
			return "Canoeing";
		case ActivityType_t::Crossfit:
			return "Crossfit";
		case ActivityType_t::EBikeRide:
			return "EBikeRide";
		case ActivityType_t::Elliptical:
			return "Elliptical";
		case ActivityType_t::IceSkate:
			return "IceSkate";
		case ActivityType_t::InlineSkate:
			return "InlineSkate";
		case ActivityType_t::Kayaking:
			return "Kayaking";
		case ActivityType_t::Kitesurf:
			return "Kitesurf";
		case ActivityType_t::NordicSki:
			return "NordicSki";
		case ActivityType_t::RockClimbing:
			return "RockClimbing";
		case ActivityType_t::RollerSki:
			return "RollerSki";
		case ActivityType_t::Rowing:
			return "Rowing";
		case ActivityType_t::Snowboard:
			return "Snowboard";
		case ActivityType_t::Snowshoe:
			return "Snowshoe";
		case ActivityType_t::StairStepper:
			return "StairStepper";
		case ActivityType_t::StandUpPaddling:
			return "StandUpPaddling";
		case ActivityType_t::Surfing:
			return "Surfing";
		case ActivityType_t::VirtualRide:
			return "VirtualRide";
		case ActivityType_t::WeightTraining:
			return "WeightTraining";
		case ActivityType_t::Windsurf:
			return "Windsurf";
		case ActivityType_t::Workout:
			return "Workout";
		case ActivityType_t::Yoga:
			return "Yoga";
		default:
			return "Other";
		}
	}

	Point_t::Point_t(const rapidjson::Value & dom) :Point_t()
	{

		if (dom.IsNull() || !dom.IsArray() || dom.GetArray().Size() != 2)
		{
			latitude = 0;
			longtitude = 0;
		}
		else
		{
			latitude = dom.GetArray()[0].GetDouble();
			longtitude = dom.GetArray()[1].GetDouble();
		}
	}

	void Achievement_t::ParseDom()
	{
		//DebugPrintDom();
		type_id = static_cast<AchievementType_t>(ParseInt64("type_id"));
		ParseString(type, "type");
		rank = ParseInt64("rank");
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Club_t::ParseDom()
	{

		string tmp;
		//DebugPrintDom();
		id = ParseInt64("id");
		resource_state = ParseInt64("resource_state");
		switch (resource_state)
		{
		case 3:
			ParseString(description, "description");
			ParseString(tmp, "club_type");
			if (!tmp.compare("casual_club"))
				club_type = ClubType_t::casual_club;
			else if (!tmp.compare("racing_team"))
				club_type = ClubType_t::racing_team;
			else if (!tmp.compare("shop"))
				club_type = ClubType_t::shop;
			else if (!tmp.compare("company"))
				club_type = ClubType_t::company;
			else club_type = ClubType_t::club_other;
			ParseString(tmp, "membership");
			if (!tmp.compare("member"))
				membership = Membership_t::member;
			else if (!tmp.compare("pending"))
				membership = Membership_t::pending;
			else membership = Membership_t::null;

			admin = ParseBool("admin");
			owner = ParseBool("owner");
			following_count = ParseInt64("following_count");
			verified = ParseBool("verified");

		case 2:
			ParseString(name, "name");
			ParseString(profile_medium, "profile_medium");
			ParseString(profile, "profile");
			ParseString(cover_photo, "cover_photo");
			ParseString(cover_photo_small, "cover_photo_small");

			ParseString(tmp, "sport_type");
			if (!tmp.compare("cycling"))
				sport_type = Sport_t::cycling;
			else if (!tmp.compare("running"))
				sport_type = Sport_t::running;
			else if (!tmp.compare("triathlon"))
				sport_type = Sport_t::triathlon;
			else sport_type = Sport_t::sport_other;

			ParseString(city, "city");
			ParseString(state, "state");
			ParseString(country, "country");
			private_club = ParseBool("private");
			member_count = ParseInt64("member_count");
			featured = ParseBool("featured");

			ParseString(url, "url");
		case 1:;
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Gear_t::ParseDom()
	{
		//DebugPrintDom();
		resource_state = ParseInt64("resource_state");
		switch (resource_state)
		{
		case 3:
			ParseStringIf(description, "description");
		case 2:
			primary = ParseBool("primary");
			ParseString(name, "name");
			distance = ParseDouble("distance");
		case 1:
			ParseString(id, "id");
		}

	}

	Bike_t::Bike_t(rapidjson::Value & DOM) : Gear_t(DOM)
	{
		//DebugPrintDom();
		//Note the Gear_t constructor swaps in the dom->
		type_str.assign("Bike_t");
		ParseStringIf(brand_name, "brand_name");
		ParseStringIf(model_name, "model_name");
		frame_type = static_cast<FrameType_t>(ParseInt64If("frame_type"));
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	Polyline_t::Polyline_t(const string & str)
	{
		PolyString.assign(str);
		//decode using code from https://github.com/paulobarcelos/ofxGooglePolyline/blob/master/src/ofxGooglePolyline.h
		ptrdiff_t len = str.length();
		ptrdiff_t index = 0;
		points.clear();
		double lat = 0.0;
		double lng = 0.0;

		while (index < len) {
			char b = 0;
			int shift = 0;
			int result = 0;
			do {
				b = str.at(index++) - 63;
				result |= (b & 0x1f) << shift;
				shift += 5;
			} while (b >= 0x20);
			double dlat = ((result & 1) ? ~(result >> 1) : (result >> 1));
			lat += dlat;

			shift = 0;
			result = 0;
			do {
				b = str.at(index++) - 63;
				result |= (b & 0x1f) << shift;
				shift += 5;
			} while (b >= 0x20);
			double dlng = ((result & 1) ? ~(result >> 1) : (result >> 1));
			lng += dlng;

			points.push_back(Point_t(lng * 1.0e-5, lat * 1.0e-5));
		}
	}

	void Map_t::ParseDom()
	{
		//DebugPrintDom();
		ParseString(id, "id");
		resource_state = ParseInt64("resource_state");
		string tmp;

		switch (resource_state)
		{
		case 3:
			ParseStringIf(tmp, "polyline");
			polyline = std::make_unique<Polyline_t>(tmp);
		case 2:
			ParseStringIf(tmp, "summary_polyline");
			summary_polyline = std::make_unique<Polyline_t>(tmp);
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	AccessToken_t::AccessToken_t(const boost::filesystem::path & filename) :AccessToken_t()
	{
		//open file
		boost::filesystem::ifstream file(filename, std::ios::binary | std::ios::ate);
		// get filesize
		size_t fs = file.tellg();
		file.seekg(0, std::ios::beg);
		//reserver memory
		string json;
		json.reserve(fs);
		//read using iteratior
		json.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		file.close();
		ParseJson(json); ParseDom();

	}

	void AccessToken_t::ParseDom()
	{
		string tmp;
		//DebugPrintDom();
		ParseString(tmp, "access_token");
		if (tmp.length() != 40)
			throw StravaException_t("AccessToken_t::AccesToken_t() access_token not 40 chars.");
		strncpy(access_token, tmp.c_str(), 40);

		ParseString(token_type, "token_type");

		if (!dom->HasMember("athlete"))
			throw StravaException_t("AccessToken_t::AccessToken_t() has no member athlete");
		athlete = make_shared<Athlete_t>((*dom)["athlete"]);
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Route_t::ParseDom()
	{
		string tmp;
		//DebugPrintDom();
		resource_state = ParseInt64("resource_state");

		switch (resource_state)
		{
		case 3:
			if (!dom->HasMember("segments"))
				throw StravaException_t("Route_t missing segments list");
			if (!(*dom)["segments"].IsArray())
				throw StravaException_t("Route_t segments not array");
			for (auto& v : (*dom)["segments"].GetArray())
			{
				segments.push_back(Segment_t(v));
			}
		case 2:
			ParseString(name, "name");
			ParseString(description, "description");
			if (!dom->HasMember("athlete"))
				throw StravaException_t("AccessToken_t::AccessToken_t() has no member athlete");
			athlete = make_shared<Athlete_t>((*dom)["athlete"]);
			distance = ParseDouble("distance");
			elevation_gain = ParseDouble("elevation_gain");
			if (!dom->HasMember("map"))
				throw StravaException_t("AccessToken_t::AccessToken_t() has no member map");
			map = make_shared<Map_t>((*dom)["map"]);

			ParseString(tmp, "type");
			if (!tmp.compare("1"))
				type = Type_t::ride;
			else if (!tmp.compare("2"))
				type = Type_t::run;
			else
				type = Type_t::other;

			ParseString(tmp, "sub_type");
			if (!tmp.compare("1"))
				sub_type = SubType_t::road;
			else if (!tmp.compare("2"))
				sub_type = SubType_t::mtb;
			else if (!tmp.compare("3"))
				sub_type = SubType_t::cx;
			else if (!tmp.compare("4"))
				sub_type = SubType_t::trail;
			else if (!tmp.compare("5"))
				sub_type = SubType_t::mixed;
			else
				sub_type = SubType_t::other;

			private_route = ParseBool("private");
			starred = ParseBool("starred");
			timestamp = ParseInt64("timestamp");


		case 1:
			id = ParseInt64("id");
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Race_t::ParseDom()
	{
		//DebugPrintDom();
		id = ParseInt64("id");
		resource_state = ParseInt64("resource_state");
		string tmp;
		switch (resource_state)
		{
		case 3:
			if (!dom->HasMember("route_ids"))
				throw StravaException_t("Route_t missing route_ids list");
			if (!(*dom)["route_ids"].IsArray())
				throw StravaException_t("Route_t route_ids not array");
			for (auto& v : (*dom)["route_ids"].GetArray())
			{
				route_ids.push_back(v.GetInt64());
			}
			ParseString(website_url, "website_url");
		case 2:
			ParseString(name, "name");
			running_race_type = static_cast<RunningRace_t>(ParseInt64("running_race_type"));
			distance = ParseDouble("distance");
			ParseTimeS(start_date_local, "start_date_local");
			ParseString(city, "city");
			ParseString(state, "state");
			ParseString(country, "country");

			ParseString(tmp, "measurement_preference");
			if (!tmp.compare("feet"))
				measurement_preference = MeasurementType_t::feet;
			else if (!tmp.compare("meters"))
				measurement_preference = MeasurementType_t::meters;
			else
				throw StravaException_t("Athlete_t measurement_preference not feet or meters");

			ParseString(url, "url");
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Segment_t::ParseDom()
	{
		//DebugPrintDom();
		resource_state = ParseInt64("resource_state");

		string tmp;
		switch (resource_state)
		{
		case 3:
			ParseTimeS(created_at, "created_at");
			ParseTimeS(updated_at, "updated_at");
			total_elevation_gain = ParseDouble("total_elevation_gain");
			if (!dom->HasMember("map"))
				throw StravaException_t("Segment_t() no member map");
			map = make_shared<Map_t>((*dom)["map"]);
			effort_count = ParseInt64("effort_count");
			athlete_count = ParseInt64("athlete_count");
			star_count = ParseInt64("start_count");
		case 2:
			ParseString(name, "name");
			ParseString(tmp, "activity_type");
			activity_type = ActivityType(tmp);
			distance = ParseDouble("distance");
			average_grade = ParseDouble("average_grade");
			maximum_grade = ParseDouble("maximum_grade");
			elevation_high = ParseDouble("elevation_high");
			elevation_low = ParseDouble("elevation_low");
			if (!dom->HasMember("start_latlng"))
				throw StravaException_t("Segment_t() no member start_latlng");
			start_latlng = Point_t((*dom)["start_latlng"]);
			if (!dom->HasMember("end_latlng"))
				throw StravaException_t("Segment_t() no member end_latlng");
			end_latlng = Point_t((*dom)["end_latlng"]);
			climb_category = ParseInt64("climb_category");
			ParseString(city, "city");
			ParseString(state, "state");
			ParseString(country, "country");
			private_segment = ParseBool("private");
			starred = ParseBoolIf("starred");
			hazardous = ParseBoolIf("hazardous");
		case 1:
			id = ParseInt64("id");
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void SegmentEffort_t::ParseDom()
	{
		//DebugPrintDom();
		resource_state = ParseInt64("resource_state");

		string tmp;
		switch (resource_state)
		{
		case 3:
		case 2:
			ParseString(name, "name");
			if (!dom->HasMember("activity"))
				throw StravaException_t("SegmentEffort_t() no member activity");
			activity = (*dom)["activity"]["id"].GetInt64();
			if (!dom->HasMember("athlete"))
				throw StravaException_t("SegmentEffort_t() no member athlete");
			athlete = (*dom)["athlete"]["id"].GetInt64();
			elapsed_time = ParseInt64("elapsed_time");
			moving_time = ParseInt64("moving_time");
			ParseTimeS(start_date, "start_date");
			ParseTimeS(start_date_local, "start_date_local");
			distance = ParseDouble("distance");
			start_index = ParseInt64("start_index");
			end_index = ParseInt64("end_index");
			average_cadence = ParseDoubleIf("average_cadence");
			device_watts = ParseBoolIf("device_watts");
			average_heartrate = ParseDoubleIf("average_heartrate");
			max_heartrate = ParseDoubleIf("max_heartrate");
			if (!dom->HasMember("segment"))
				throw StravaException_t("SegmentEffort_t() no member segment");
			segment = make_shared<Segment_t>((*dom)["segment"]);
			kom_rank = ParseInt64("kom_rank");
			pr_rank = ParseInt64("pr_rank");
			hidden = ParseBool("hidden");
		case 1:
			id = ParseInt64("id");
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Effort_t::ParseDom()
	{
		//DebugPrintDom();
		resource_state = ParseInt64("resource_state");
		id = ParseInt64("id");
		ParseString(name, "name");
		if (dom->HasMember("segment") && !(*dom)["segment"].IsNull())
			segment = (*dom)["segment"]["id"].GetInt64();
		else
			segment = 0;

		if (dom->HasMember("activity") && !(*dom)["activity"].IsNull())
			activity = (*dom)["activity"]["id"].GetInt64();
		else activity = 0;

		if (dom->HasMember("athlete") && !(*dom)["athlete"].IsNull())
			athlete = (*dom)["athlete"]["id"].GetInt64();
		else athlete = 0;

		kom_rank = ParseInt64If("kom_rank");
		pr_rank = ParseInt64If("pr_rank");
		elapsed_time = ParseInt64If("elapsed_time");
		moving_time = ParseInt64If("moving_time");

		ParseTimeS(start_date, "start_date");
		ParseTimeS(start_date_local, "start_date_local");
		distance = ParseInt64If("distance");
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Splits_t::ParseDom()
	{
		//DebugPrintDom();
		average_speed = ParseDouble("average_speed");
		distance = ParseDouble("distance");
		elapsed_time = ParseInt64("elapsed_time");
		elevation_difference = ParseDouble("elevation_difference");
		pace_zone = ParseInt64("pace_zone");
		moving_time = ParseInt64("moving_time");
		split = ParseInt64("split");
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	Data_t Stream_t::DataTypeFromStreamType(StreamType_t stream_type) {
		switch (stream_type)
		{
		case StreamType_t::time:
		case StreamType_t::heartrate:
		case StreamType_t::cadence:
		case StreamType_t::watts:
		case StreamType_t::temp:
			//Integer case
			return Data_t::kInt;
			break;
		case StreamType_t::distance:
		case StreamType_t::altitude:
		case StreamType_t::velocity_smooth:
		case StreamType_t::grade_smooth:
			//float case
			return Data_t::kDouble;
			break;
		case StreamType_t::moving:
			//bool case
			return Data_t::kBool;
			break;
		case StreamType_t::latlng:
			//coordinates
			return Data_t::kPoint;
			break;
		default:
			return Data_t::kNull;
		}
	}

	StreamType_t Stream_t::GetStreamType(rapidjson::Value & dom)
	{
		string type;
		if (!dom.HasMember("type"))
			throw StravaException_t("GetStreamType missing type.\n");
		if (!(dom)["type"].IsString())
			throw StravaException_t("GetStreamType type not a astring\n");
		type.assign((dom)["type"].GetString(), (dom)["type"].GetStringLength());
		if (!type.compare("time"))
			return StreamType_t::time;
		else if (!type.compare("latlng"))
			return StreamType_t::latlng;
		else if (!type.compare("distance"))
			return StreamType_t::distance;
		else if (!type.compare("altitude"))
			return StreamType_t::altitude;
		else if (!type.compare("velocity_smooth"))
			return StreamType_t::velocity_smooth;
		else if (!type.compare("heartrate"))
			return StreamType_t::heartrate;
		else if (!type.compare("cadence"))
			return StreamType_t::cadence;
		else if (!type.compare("watts"))
			return StreamType_t::watts;
		else if (!type.compare("temp"))
			return StreamType_t::temp;
		else if (!type.compare("moving"))
			return StreamType_t::moving;
		else if (!type.compare("grade_smooth"))
			return StreamType_t::grade_smooth;
		else
			throw StravaException_t("Stream_t StreamType not known.");
	}

	void Stream_t::ParseStreamArray(const string & json, Activity_t & activity)
	{
		//parse json array
		//First create rapidjson object
		rapidjson::Document document;
		document.Parse<rapidjson::ParseFlag::kParseFullPrecisionFlag | rapidjson::ParseFlag::kParseCommentsFlag | rapidjson::ParseFlag::kParseNanAndInfFlag>(json);
		if (document.HasParseError())
		{
			std::cerr << "JSON parse error: " << GetParseError_En(document.GetParseError());
			std::cerr << "(" << document.GetErrorOffset() << ")" << std::endl;

			throw StravaException_t(string("Stream_t::PasreStreamArray").append("JSON parser error.\n"));
		}
		//go over returned dom and do error/sanity check and fill the data structure
		if (!document.IsArray())
			throw StravaException_t(string("Stream_t::PasreStreamArray").append(", returned DOM no array\n"));

		//loop over activities in array;
		activity.streams.reserve(document.GetArray().Size());
		for (auto& v : document.GetArray())
		{
			try {
				StreamType_t streamtype = GetStreamType(v);
				Data_t datatype = DataTypeFromStreamType(streamtype);
				Stream_t *stream = nullptr;
				switch (datatype)
				{
				case RideWeather::Data_t::kInt:
					stream = new StreamRaw_t<int>(v);
					break;
				case RideWeather::Data_t::kDouble:
					stream = new StreamRaw_t<double>(v);
					break;
				case RideWeather::Data_t::kBool:
					stream = new StreamRaw_t<Bool>(v);
					break;
				case RideWeather::Data_t::kPoint:
					stream = new StreamRaw_t<Point_t>(v);
					break;
				default:
					throw StravaException_t(string("Stream_t::PasreStreamArray").append(", stream has invalid dataType\n"));
					break;
				}
				activity.streams.insert_or_assign(streamtype,stream);
			}
			catch (StravaException_t & ex)
			{
				std::cerr << "Stream_t::ParseStreamArray: Error parsing streams" << std::endl;
				std::cerr << ex.what() << std::endl;
			}
		}
	}


	void Stream_t::ParseDom()
	{
		stream_type = GetStreamType(*dom);
		original_size = ParseInt64("original_size");
		ParseString(series_type, "series_type");
		string tmp;
		ParseString(tmp, "resolution");
		if (type.compare("low")==0)
			resolution = Resolution_t::low;
		else if (type.compare("medium")==0)
			resolution = Resolution_t::medium;
		else if (type.compare("high")==0)
			resolution = Resolution_t::high;
		else
			resolution = Resolution_t::default;

		//parse data
		if (!dom->HasMember("data"))
			throw StravaException_t("Stream_t missing data list");
		if (!(*dom)["data"].IsArray())
			throw StravaException_t("Steam_t data not array");
		ParseData();
	}

	void Photo_t::ParseDom()
	{
		//DebugPrintDom();
		id = ParseInt64("id");
		resource_state = ParseInt64("resource_state");
		ParseString(unique_id, "unique_id");
		activity_id = ParseInt64("activity_id");
		ParseString(ref, "ref");
		ParseString(uid, "uid");
		ParseString(caption, "caption");
		ParseString(type, "type");
		source = ParseInt64("source");
		ParseTimeS(uploaded_at, "uploaded_at");
		ParseTimeS(created_at, "created_at");
		location = Point_t((*dom)["location"]);

		if (!dom->HasMember("urls"))
			throw StravaException_t("Photo_t missing urls object");
		Value& url = (*dom)["urls"];
		for (auto v = url.MemberBegin(); v != url.MemberEnd(); ++v)
		{
			string n(v->name.GetString());
			URL_t val(v->value.GetString());
			urls.insert(std::pair<string, URL_t>(n, val));
		}
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

	void Lap_t::ParseDom()
	{
		//DebugPrintDom();
		if (dom->HasMember("activity") && (*dom)["activity"].IsNull())
			activity = 0;
		else activity = (*dom)["activity"]["id"].GetInt64();

		if (dom->HasMember("athlete") && (*dom)["athlete"].IsNull())
			athlete = 0;
		else athlete = (*dom)["athlete"]["id"].GetInt64();

		average_cadence = ParseDoubleIf("average_cadence");
		average_speed = ParseDoubleIf("average_speed");
		elapsed_time = ParseInt64If("elapsed_time");
		end_index = ParseInt64If("end_index");
		id = ParseInt64("id");
		lap_index = ParseInt64("lap_index");
		max_speed = ParseDoubleIf("max_speed");
		moving_speed = ParseInt64If("moving_speed");
		ParseString(name, "name");
		resource_state = ParseInt64("resource_state");
		split = ParseInt64If("split");
		ParseTimeS(start_date, "start_date");
		ParseTimeS(start_date_local, "start_date_local");
		total_elevation_gain = ParseDoubleIf("total_elevation_gain");
		delete dom; dom = nullptr; delete document; document = nullptr;
	}

}//namespace RideWeather