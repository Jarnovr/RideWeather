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

namespace RideWeather
{

	void Strava_t::ParseJson(const string & json)
	{
		//First create rapidjson object
		document.Parse<ParseFlag::kParseFullPrecisionFlag | ParseFlag::kParseCommentsFlag | ParseFlag::kParseNanAndInfFlag>(json.c_str());
		//generic error checking
		if (document.HasParseError())
		{
			cerr << "JSON parse error: " << GetParseError_En(document.GetParseError());
			cerr << "(" << document.GetErrorOffset() << ")" << endl;

			throw StravaException_t(type_str.append("::").append(type_str).append("JSON parser error.\n"));
		}

		//go over returned dom and do error/sanity check and fill the data structure
		if (!document.IsObject())
			throw StravaException_t(type_str.append("::").append(type_str).append(", returned DOM no object\n"));
		//dom.Swap(document);
		dom.CopyFrom(document,document.GetAllocator());	
	}

	void Athlete_t::ParseDom()
	{
		string tmp;
		rapidjson::StringBuffer buffer;
		buffer.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		dom.Accept(writer);
		cerr<<buffer.GetString()<<endl;

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

			if (!dom.HasMember("clubs"))
				throw StravaException_t("Athlete_t missing clubs list");
			if (!dom["clubs"].IsArray())
				throw StravaException_t("Athlete_t clubs not array");
			for (auto& v : dom["clubs"].GetArray())
			{
				clubs.push_back(Club_t(v));
			}

			if (!dom.HasMember("bikes"))
				throw StravaException_t("Athlete_t missing bikes list");
			if (!dom["bikes"].IsArray())
				throw StravaException_t("Athlete_t bikes not array");
			for (auto& v : dom["bikes"].GetArray())
			{
				bikes.push_back(Bike_t(v));
			}
			if (!dom.HasMember("shoes"))
				throw StravaException_t("Athlete_t missing shoes list");
			if (!dom["shoes"].IsArray())
				throw StravaException_t("Athlete_t shoes not array");
			for (auto& v : dom["shoes"].GetArray())
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

			if (!dom.HasMember("sex"))
				throw StravaException_t("Athlete missing sex\n");
			if (!dom["sex"].IsString())
				throw StravaException_t("Athlete_t sex not char\n");
			sex = *dom["sex"].GetString();

			if (!dom.HasMember("friend"))
				throw StravaException_t("Athlete missing friend\n");
			if (dom["friend"].IsNull())
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

			if (!dom.HasMember("follower"))
				throw StravaException_t("Athlete missing follower\n");
			if (dom["follower"].IsNull())
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
			ParseString(created_at, "created_at");
			ParseString(updated_at, "updated_at");
		case 1:;
		}

	}

	void Zones_t::ParseDom()
	{

		if (!dom.HasMember("heart_rate"))
			throw StravaException_t("Zones_t missing heart_rate");
		if (!dom["heart_rate"].HasMember("custom_zones"))
			throw StravaException_t("Zones_t heart rate has no member custom_zones");
		heart_rate_custom = dom["heart_rate"]["custom_zones"].GetBool();

		if (!dom["heart_rate"].HasMember("zones"))
			throw StravaException_t("Heart_rate missing zones");
		for (auto& v : dom["heart_rate"]["zones"].GetArray())
		{
			zone_t tmp;
			tmp.min = (short)v["min"].GetInt64();
			tmp.max = (short)v["max"].GetInt64();
			heart_zones.push_back(tmp);
		}

		if (dom.HasMember("power"))
		{
			if (!dom["power"].HasMember("zones"))
				throw StravaException_t("power missing zones");
			for (auto& v : dom["power"]["zones"].GetArray())
			{
				zone_t tmp;
				tmp.min = (short)v["min"].GetInt64();
				tmp.max = (short)v["max"].GetInt64();
				power_zones.push_back(tmp);
			}
		}

	}

	void Total_t::ParseDom()
	{
		//Parse content
		count = ParseInt64("count");
		distance = ParseDouble("distance");
		moving_time = ParseInt64("moving_time");
		elapsed_time = ParseInt64("elapsed_time");
		elevation_gain = ParseDouble("elevation gain");
		achievement_count = ParseInt64("achievement_count");
	}

	void Totals_t::ParseDom()
	{
		biggest_ride_distance = ParseDouble("biggest_ride_distance");
		biggest_climb_elevation_gain = ParseDouble("biggest_climb_elevation");

		if (!dom.HasMember("recent_ride_totals"))
			throw StravaException_t("Totals_t missing recent_ride_totals");
		recent_ride_totals = Total_t(dom["recent_ride_totals"]);
		if (!dom.HasMember("recent_run_totals"))
			throw StravaException_t("Totals_t missing recent_run_totals");
		recent_run_totals = Total_t(dom["recent_run_totals"]);
		if (!dom.HasMember("recent_swim_totals"))
			throw StravaException_t("Totals_t missing recent_swim_totals");
		recent_swim_totals = Total_t(dom["recent_swim_totals"]);

		if (!dom.HasMember("ytd_ride_totals"))
			throw StravaException_t("Totals_t missing ytd_ride_totals");
		ytd_ride_totals = Total_t(dom["ytd_ride_totals"]);
		if (!dom.HasMember("ytd_run_totals"))
			throw StravaException_t("Totals_t missing ytd_run_totals");
		ytd_run_totals = Total_t(dom["ytd_run_totals"]);
		if (!dom.HasMember("ytd_swim_totals"))
			throw StravaException_t("Totals_t missing ytd_swim_totals");
		ytd_swim_totals = Total_t(dom["ytd_swim_totals"]);

		if (!dom.HasMember("all_ride_totals"))
			throw StravaException_t("Totals_t missing all_ride_totals");
		all_ride_totals = Total_t(dom["all_ride_totals"]);
		if (!dom.HasMember("all_run_totals"))
			throw StravaException_t("Totals_t missing all_run_totals");
		all_run_totals = Total_t(dom["all_run_totals"]);
		if (!dom.HasMember("all_swim_totals"))
			throw StravaException_t("Totals_t missing all_swim_totals");
		all_swim_totals = Total_t(dom["all_swim_totals"]);
	}

	void Activity_t::ParseDom()
	{
		string tmp;

		id = ParseInt64("id");
		resource_state = ParseInt64("resource_state");

		if (resource_state <= 0 || resource_state > 3)
			throw StravaException_t("Invalid resource_state.\n");

		switch (resource_state)
		{
		case 3:
			calories = ParseDouble("calories");
			ParseString(description, "description");
			if (!dom.HasMember("gear"))
				throw StravaException_t("Activity_t has no member gear.\n");
			gear = make_shared<Gear_t>(dom["gear"]);
			if (!dom.HasMember("segment_efforts"))
				throw StravaException_t("Athlete_t missing segment_efforts list");
			if (!dom["segment_efforts"].IsArray())
				throw StravaException_t("Athlete_t segment_efforts not array");
			for (auto& v : dom["segment_efforts"].GetArray())
			{
				segment_efforts.push_back(SegmentEffort_t(v));
			}
			if (!dom.HasMember("splits_metric"))
				throw StravaException_t("Athlete_t missing splits_metric list");
			if (!dom["splits_metric"].IsArray())
				throw StravaException_t("Athlete_t splits_metric not array");
			for (auto& v : dom["splits_metric"].GetArray())
			{
				splits_metric.push_back(Splits_t(v));
			}

			if (!dom.HasMember("splits_standard"))
				throw StravaException_t("Athlete_t missing splits_standard list");
			if (!dom["splits_standard"].IsArray())
				throw StravaException_t("Athlete_t splits_standard not array");
			for (auto& v : dom["splits_metric"].GetArray())
			{
				splits_standard.push_back(Splits_t(v));
			}

			if (!dom.HasMember("laps"))
				throw StravaException_t("Athlete_t missing laps list");
			if (!dom["laps"].IsArray())
				throw StravaException_t("Athlete_t laps not array");
			for (auto& v : dom["laps"].GetArray())
			{
				laps.push_back(Lap_t(v));
			}

			if (!dom.HasMember("best_efforts"))
				throw StravaException_t("Athlete_t missing best_efforts list");
			if (!dom["best_efforts"].IsArray())
				throw StravaException_t("Athlete_t best_efforts not array");
			for (auto& v : dom["best_efforts"].GetArray())
			{
				best_efforts.push_back(Effort_t(v));
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
			elev_high = ParseDouble("elev_high");
			elev_low = ParseDouble("elev_low");
			ParseString(typeS, "type");
			type = ActivityType(typeS);
			ParseString(start_date, "start_date");
			ParseString(start_date_local, "start_date_local");
			ParseString(timezone_act, "timezone");
			if (!dom.HasMember("start_latlng"))
				throw StravaException_t("Activity_t missing start_latlng");
			start_latlng = Point_t(dom["start_latlng"]);
			if (!dom.HasMember("end_latlng"))
				throw StravaException_t("Activity_t missing end_latlng");
			end_latlng = Point_t(dom["end_latlng"]);
			achievement_count = ParseInt64("achievement_count");
			kudos_count = ParseInt64("kudos_count");
			comment_count = ParseInt64("comment_count");
			athlete_count = ParseInt64("athlete_count");
			photo_count = ParseInt64("photo_count");
			total_photo_count = ParseInt64("total_photo_count");
			if (!dom.HasMember("map"))
				throw StravaException_t("Activity_t missing map");
			map = make_shared<Map_t>(dom["map"]);
			trainer = ParseBool("trainer");
			commute = ParseBool("commute");
			manual = ParseBool("manual");
			private_act = ParseBool("private");
			flagged = ParseBool("flagged");
			workout_type = static_cast<Workout_t> (ParseInt64("workout_type"));
			ParseStringIf(gear_id, "gear_id");
			average_speed = ParseDouble("average_speed");
			max_speed = ParseDouble("max_speed");
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
				max_heartrate = ParseInt64("max_heartrate");
			}
			suffer_score = ParseIntIf("suffer_score");
			has_kudoed = ParseBool("has_kudoed");

		case 1:;
		}

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
		if (!str.compare("Walt"))
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


	Point_t::Point_t(const rapidjson::Value & dom) :Point_t()
	{
		if (!dom.IsArray() || dom.GetArray().Size() != 2)
			throw StravaException_t("Point_t is not array of length 2");
		latitude = dom.GetArray()[0].GetDouble();
		longtitude = dom.GetArray()[1].GetDouble();
	}

	void Achievement_t::ParseDom()
	{
		type_id = static_cast<AchievementType_t>(ParseInt64("type_id"));
		ParseString(type, "type");
		rank = ParseInt64("rank");
	}

	void Club_t::ParseDom()
	{
		string tmp;
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
	}

	void Gear_t::ParseDom()
	{
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
		//Note the Gear_t constructor swaps in the dom.
		type_str.assign("Bike_t");
		ParseStringIf(brand_name, "brand_name");
		ParseStringIf(model_name, "model_name");
		frame_type = static_cast<FrameType_t>(ParseInt64If("frame_type"));
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
		ParseString(id,"id");
		resource_state = ParseInt64("resource_state");
		string tmp;

		switch (resource_state)
		{
		case 3:
			ParseStringIf(tmp, "polyline");
			polyline = std::make_unique<Polyline_t>(tmp);
		case 2:
			ParseString(tmp, "summary_polyline");
			summary_polyline = std::make_unique<Polyline_t>(tmp);
		}
	}

	void AccessToken_t::ParseDom()
	{
		string tmp;
		ParseString(tmp, "access_token");
		if (tmp.length() != 40)
			throw StravaException_t("AccessToken_t::AccesToken_t() access_token not 40 chars.");
		strncpy(access_token, tmp.c_str(), 40);

		ParseString(token_type, "token_type");

		if (!dom.HasMember("athlete"))
			throw StravaException_t("AccessToken_t::AccessToken_t() has no member athlete");
		athlete = make_shared<Athlete_t>(dom["athlete"]);
	}

	void Route_t::ParseDom()
	{
		string tmp;
		resource_state = ParseInt64("resource_state");

		switch (resource_state)
		{
		case 3:
			if (!dom.HasMember("segments"))
				throw StravaException_t("Route_t missing segments list");
			if (!dom["segments"].IsArray())
				throw StravaException_t("Route_t segments not array");
			for (auto& v : dom["segments"].GetArray())
			{
				segments.push_back(Segment_t(v));
			}
		case 2:
			ParseString(name, "name");
			ParseString(description, "description");
			if (!dom.HasMember("athlete"))
				throw StravaException_t("AccessToken_t::AccessToken_t() has no member athlete");
			athlete = make_shared<Athlete_t>(dom["athlete"]);
			distance = ParseDouble("distance");
			elevation_gain = ParseDouble("elevation_gain");
			if (!dom.HasMember("map"))
				throw StravaException_t("AccessToken_t::AccessToken_t() has no member map");
			map = make_shared<Map_t>(dom["map"]);

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
	}

	void Race_t::ParseDom()
	{
		id = ParseInt64("id");
		resource_state = ParseInt64("resource_state");
		string tmp;
		switch (resource_state)
		{
		case 3:
			if (!dom.HasMember("route_ids"))
				throw StravaException_t("Route_t missing route_ids list");
			if (!dom["route_ids"].IsArray())
				throw StravaException_t("Route_t route_ids not array");
			for (auto& v : dom["route_ids"].GetArray())
			{
				route_ids.push_back(v.GetInt64());
			}
			ParseString(website_url, "website_url");
		case 2:
			ParseString(name, "name");
			running_race_type = static_cast<RunningRace_t>(ParseInt64("running_race_type"));
			distance = ParseDouble("distance");
			ParseString(start_date_local, "start_date_local");
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
	}

	void Segment_t::ParseDom()
	{
		resource_state = ParseInt64("resource_state");

		string tmp;
		switch (resource_state)
		{
		case 3:
			ParseString(created_at, "created_at");
			ParseString(updated_at, "updated_at");
			total_elevation_gain = ParseDouble("total_elevation_gain");
			if (!dom.HasMember("map"))
				throw StravaException_t("Segment_t() no member map");
			map = make_shared<Map_t>(dom["map"]);
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
			if (!dom.HasMember("start_latlng"))
				throw StravaException_t("Segment_t() no member start_latlng");
			start_latlng = Point_t(dom["start_latlng"]);
			if (!dom.HasMember("end_latlng"))
				throw StravaException_t("Segment_t() no member end_latlng");
			end_latlng = Point_t(dom["end_latlng"]);
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
	}

	void SegmentEffort_t::ParseDom()
	{
		resource_state = ParseInt64("resource_state");

		string tmp;
		switch (resource_state)
		{
		case 3:
		case 2:
			ParseString(name, "name");
			if (!dom.HasMember("activity"))
				throw StravaException_t("SegmentEffort_t() no member activity");
			activity = dom["activity"]["id"].GetInt64();
			if (!dom.HasMember("athlete"))
				throw StravaException_t("SegmentEffort_t() no member athlete");
			athlete = dom["athlete"]["id"].GetInt64();
			elapsed_time = ParseInt64("elapsed_time");
			moving_time = ParseInt64("moving_time");
			ParseString(start_date, "start_date");
			ParseString(start_date_local, "start_date_local");
			distance = ParseDouble("distance");
			start_index = ParseInt64("start_index");
			end_index = ParseInt64("end_index");
			average_cadence = ParseDoubleIf("average_cadence");
			device_watts = ParseBoolIf("device_watts");
			average_heartrate = ParseDoubleIf("average_heartrate");
			max_heartrate = ParseIntIf("max_heartrate");
			if (!dom.HasMember("segment"))
				throw StravaException_t("SegmentEffort_t() no member segment");
			segment = make_shared<Segment_t>(dom["segment"]);
			kom_rank = ParseInt64("kom_rank");
			pr_rank = ParseInt64("pr_rank");
			hidden = ParseBool("hidden");
		case 1:
			id = ParseInt64("id");
		}

	}

	void Effort_t::ParseDom()
	{
		resource_state = ParseInt64("resource_state");
		id = ParseInt64("id");
		ParseString(name, "name");
		if (dom.HasMember("segment") && dom["segment"].IsNull())
			segment = 0;
		else segment = dom["segment"]["id"].GetInt64();

		if (dom.HasMember("activity") && dom["activity"].IsNull())
			activity = 0;
		else activity = dom["activity"]["id"].GetInt64();

		if (dom.HasMember("athlete") && dom["athlete"].IsNull())
			athlete = 0;
		else athlete = dom["athlete"]["id"].GetInt64();

		kom_rank = ParseInt64("kom_rank");
		pr_rank = ParseInt64("pr_rank");
		elapsed_time = ParseInt64("elapsed_time");
		moving_time = ParseInt64("moving_time");

		ParseString(start_date, "start_date");
		ParseString(start_date_local, "start_date_local");
		distance = ParseInt64("distance");
	}

	void Splits_t::ParseDom()
	{
		average_speed = ParseDouble("average_speed");
		distance = ParseDouble("distance");
		elapsed_time = ParseInt64("elapsed_time");
		elevation_difference = ParseInt64("elevation_difference");
		pace_zone = ParseInt64("pace_zone");
		moving_time = ParseInt64("moving_time");
		split = ParseInt64("split");
	}



	Data_t GetStreamType(rapidjson::Value & DOM)
	{
		string type;
		StreamType_t stream_type;

		if (!DOM.HasMember("type"))
			throw StravaException_t("GetStreamType missing type.\n");
		if (!DOM["type"].IsString())
			throw StravaException_t("GetStreamType type not a astring\n");
		type.assign(DOM["type"].GetString(), DOM["type"].GetStringLength());
		if (!type.compare("time"))
			stream_type = StreamType_t::time;
		else if (!type.compare("latlng"))
			stream_type = StreamType_t::latlng;
		else if (!type.compare("distance"))
			stream_type = StreamType_t::distance;
		else if (!type.compare("altitude"))
			stream_type = StreamType_t::altitude;
		else if (!type.compare("velocity_smooth"))
			stream_type = StreamType_t::velocity_smooth;
		else if (!type.compare("heartrate"))
			stream_type = StreamType_t::heartrate;
		else if (!type.compare("cadence"))
			stream_type = StreamType_t::cadence;
		else if (!type.compare("watts"))
			stream_type = StreamType_t::watts;
		else if (!type.compare("temp"))
			stream_type = StreamType_t::temp;
		else if (!type.compare("moving"))
			stream_type = StreamType_t::moving;
		else if (!type.compare("grade_smooth"))
			stream_type = StreamType_t::grade_smooth;
		else
			throw StravaException_t("Stream_t StreamType not known.");
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
		}
		return Data_t::kNull;
	}

	void Photo_t::ParseDom()
	{
		id = ParseInt64("id");
		resource_state = ParseInt64("resource_state");
		ParseString(unique_id, "unique_id");
		activity_id = ParseInt64("activity_id");
		ParseString(ref, "ref");
		ParseString(uid, "uid");
		ParseString(caption, "caption");
		ParseString(type, "type");
		source = ParseInt64("source");
		ParseString(uploaded_at, "uploaded_at");
		ParseString(created_at, "created_at");
		location = Point_t(dom["location"]);

		if (!dom.HasMember("urls"))
			throw StravaException_t("Photo_t missing urls object");
		Value& url = dom["urls"];
		for (auto v = url.MemberBegin(); v != url.MemberEnd(); ++v)
		{
			string n(v->name.GetString());
			URL_t val(v->value.GetString());
			urls.insert(std::pair<string, URL_t>(n, val));
		}
	}

	void Lap_t::ParseDom()
	{
		if (dom.HasMember("activity") && dom["activity"].IsNull())
			activity = 0;
		else activity = dom["activity"]["id"].GetInt64();

		if (dom.HasMember("athlete") && dom["athlete"].IsNull())
			athlete = 0;
		else athlete = dom["athlete"]["id"].GetInt64();

		average_cadence = ParseDouble("average_cadence");
		average_speed = ParseDouble("average_speed");
		elapsed_time = ParseInt64("elapsed_time");
		end_index = ParseInt64("end_index");
		id = ParseInt64("id");
		lap_index = ParseInt64("lap_index");
		max_speed = ParseDouble("max_speed");
		moving_speed = ParseInt64If("moving_speed");
		ParseString(name, "name");
		resource_state = ParseInt64("resource_state");
		split = ParseInt64("split");
		ParseString(start_date, "start_date");
		ParseString(start_date_local, "start_date_local");
		total_elevation_gain = ParseDouble("total_elevation_gain");
	}

}//namespace RideWeather