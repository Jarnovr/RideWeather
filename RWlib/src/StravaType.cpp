#include "StravaType.h"
#define RAPIDJSON_SSE42

#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include <iostream>
using namespace rapidjson;




Athlete_t::Athlete_t(const string& json) :Athlete_t()
{
	string tmp;
	
	//set type str
	type_str.assign("Athlete_t");
	Document document;
	//First create rapidjson object
	document.Parse<ParseFlag::kParseFullPrecisionFlag | ParseFlag::kParseCommentsFlag | ParseFlag::kParseNanAndInfFlag>(json.c_str());
	//generic error checking
	if (document.HasParseError())
	{
		cerr << "JSON parse error: " << GetParseError_En(document.GetParseError());
		cerr << "(" << document.GetErrorOffset() << ")" << endl;
		
		throw StravaException_t("Athlete_t::Athlete_t() JSON parser error.\n");
	}		
	
	//go over returned dom and do error/sanity check and fill the data structure
	if (!document.IsObject())
		throw StravaException_t("Athlete_t::Athlete_t(), returned DOM no object\n");
	dom.Swap(document);

	id = ParseInt64("id");
	resource_state = ParseInt("resource_state");

	if (resource_state <= 0 || resource_state > 3)
		throw StravaException_t("Invalid resource_state.\n");
	
	switch (resource_state)
	{
	case 3:
		follower_count = ParseInt("follower_count");
		friend_count = ParseInt("friend_count");
		mutual_friend_count = ParseInt("mutual_friend_count");
		athlete_type = AthleteType_t(ParseInt("athlete_type"));
		ParseString(date_preference, "date_preference");
		ParseString(tmp, "measurement_preference");
		if (!tmp.compare("feet"))
			measurement_preference = MeasurementType_t::feet;
		else if (!tmp.compare("meters"))
			measurement_preference = MeasurementType_t::meters;
		else
			throw StravaException_t("Athlete_t measurement_preference not feet or meters");
		ParseString(email, "email");
		ftp = ParseInt("ftp");
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

Zones_t::Zones_t(const string & json): Zones_t::Zones_t()
{
	//set type str
	type_str.assign("Zones_t");

	Document document;
	//First create rapidjson object
	document.Parse<ParseFlag::kParseFullPrecisionFlag | ParseFlag::kParseCommentsFlag | ParseFlag::kParseNanAndInfFlag>(json.c_str());
	//generic error checking
	if (document.HasParseError())
	{
		cerr << "JSON parse error: " << GetParseError_En(document.GetParseError());
		cerr << "(" << document.GetErrorOffset() << ")" << endl;

		throw StravaException_t("Zones_t::Zones_t() JSON parser error.\n");
	}

	//go over returned dom and do error/sanity check and fill the data structure
	if (!document.IsObject())
		throw StravaException_t("Zones_t::Zones_t(), returned DOM no object\n");
	dom.Swap(document); //Move into object

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
		tmp.min = (short) v["min"].GetInt();
		tmp.max = (short) v["max"].GetInt();
		heart_zones.push_back(tmp);
	}
	
	if (dom.HasMember("power"))
	{
		if (!dom["power"].HasMember("zones"))
			throw StravaException_t("power missing zones");
		for (auto& v : dom["power"]["zones"].GetArray())
		{
			zone_t tmp;
			tmp.min = (short)v["min"].GetInt();
			tmp.max = (short)v["max"].GetInt();
			power_zones.push_back(tmp);
		}
	}

}

Total_t::Total_t(const string & json):Total_t()
{
	//set type str
	type_str.assign("Total_t");

	Document document;
	//First create rapidjson object
	document.Parse<ParseFlag::kParseFullPrecisionFlag | ParseFlag::kParseCommentsFlag | ParseFlag::kParseNanAndInfFlag>(json.c_str());
	//generic error checking
	if (document.HasParseError())
	{
		cerr << "JSON parse error: " << GetParseError_En(document.GetParseError());
		cerr << "(" << document.GetErrorOffset() << ")" << endl;

		throw StravaException_t("Total_t::Total_t() JSON parser error.\n");
	}

	//go over returned dom and do error/sanity check and fill the data structure
	if (!document.IsObject())
		throw StravaException_t("Total_t::Total_t(), returned DOM no object\n");
	
	dom.Swap(document);
	ParseDom();
	
}

Total_t::Total_t(rapidjson::Value& DOM)
{
	dom.Swap(DOM);
	ParseDom();
}

void Total_t::ParseDom()
{
	//Parse content
	count = ParseInt("count");
	distance = ParseDouble("distance");
	moving_time = ParseInt64("moving_time");
	elapsed_time = ParseInt64("elapsed_time");
	elevation_gain = ParseDouble("elevation gain");
	achievement_count = ParseInt("achievement_count");
}

Totals_t::Totals_t(const string & json)
{
	//set type str
	type_str.assign("Totals_t");

	Document document;
	//First create rapidjson object
	document.Parse<ParseFlag::kParseFullPrecisionFlag | ParseFlag::kParseCommentsFlag | ParseFlag::kParseNanAndInfFlag>(json.c_str());
	//generic error checking
	if (document.HasParseError())
	{
		cerr << "JSON parse error: " << GetParseError_En(document.GetParseError());
		cerr << "(" << document.GetErrorOffset() << ")" << endl;

		throw StravaException_t("Totals_t::Totals_t() JSON parser error.\n");
	}

	//go over returned dom and do error/sanity check and fill the data structure
	if (!document.IsObject())
		throw StravaException_t("Totals_t::Totals_t(), returned DOM no object\n");
	dom.Swap(document);
	biggest_ride_distance = ParseDouble("biggest_ride_distance");
	biggest_climb_elevation_gain = ParseDouble("biggest_climb_elevation");

	if (!dom.HasMember("recent_ride_totals"))
		throw StravaException_t("Totals_t missing recent_ride_totals");
	recent_ride_totals=Total_t(dom["recent_ride_totals"]);
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

Activity_t::Activity_t(const string & json):Activity_t()
{
	string tmp;

	//set type str
	type_str.assign("Activity_t");
	Document document;
	//First create rapidjson object
	document.Parse<ParseFlag::kParseFullPrecisionFlag | ParseFlag::kParseCommentsFlag | ParseFlag::kParseNanAndInfFlag>(json.c_str());
	//generic error checking
	if (document.HasParseError())
	{
		cerr << "JSON parse error: " << GetParseError_En(document.GetParseError());
		cerr << "(" << document.GetErrorOffset() << ")" << endl;

		throw StravaException_t("Activity_t::Activity_t() JSON parser error.\n");
	}

	//go over returned dom and do error/sanity check and fill the data structure
	if (!document.IsObject())
		throw StravaException_t("Activity_t::Activity_t(), returned DOM no object\n");
	dom.Swap(document);

	id = ParseInt64("id");
	resource_state = ParseInt("resource_state");

	if (resource_state <= 0 || resource_state > 3)
		throw StravaException_t("Invalid resource_state.\n");

	switch (resource_state)
	{
	case 3:
		calories = ParseDouble("calories");
		ParseString(description, "description");
		if (!dom.HasMember("gear"))
			throw StravaException_t("Activity_t has no member gear.\n");
		gear = new Gear_t(dom["gear"]);
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
		if (!dom.HasMember("best_efforts"))
			throw StravaException_t("Athlete_t missing best_efforts list");
		if (!dom["best_efforts"].IsArray())
			throw StravaException_t("Athlete_t best_efforts not array");
		for (auto& v : dom["best_efforts"].GetArray())
		{
			best_efforts.push_back(Effort_t(v));
		}
		ParseString(device_name, "device_name");
		ParseString(embed_token, "embed_token");
		//skip photos for now

	case 2:
		ParseString(external_id, "external_id");
		upload_id = ParseInt64("upload_id");
		ParseString(name, "name");
		distance = ParseDouble("distance");
		moving_time = ParseInt("moving_time");
		elapsed_time = ParseInt("elapsed_time");
		total_elevation_gain = ParseDouble("total_elevation_gain");
		elev_high = ParseDouble("elev_high");
		elev_low = ParseDouble("elev_low");
		ParseString(typeS, "type");
		type = ActivityType(typeS);
		ParseString(start_date, "start_date");
		ParseString(start_date_local, "start_date_local");
		ParseString(timezone_act,"timezone");
		if (!dom.HasMember("start_latlng"))
			throw StravaException_t("Activity_t missing start_latlng");
		start_latlng = Point_t(dom["start_latlng"]);
		if (!dom.HasMember("end_latlng"))
			throw StravaException_t("Activity_t missing end_latlng");
		end_latlng = Point_t(dom["end_latlng"]);
		achievement_count = ParseInt("achievement_count");
		kudos_count = ParseInt("kudos_count");
		comment_count = ParseInt("comment_count");
		athlete_count = ParseInt("athlete_count");
		photo_count = ParseInt("photo_count");
		total_photo_count = ParseInt("total_photo_count");
		if (!dom.HasMember("map"))
			throw StravaException_t("Activity_t missing map");
		map = new Map_t(dom["map"]);
		trainer = ParseBool("trainer");
		commute = ParseBool("commute");
		manual = ParseBool("manual");
		private_act = ParseBool("private");
		flagged = ParseBool("flagged");
		workout_type = static_cast<Workout_t> (ParseInt("workout_type"));
		ParseString(gear_id,"gear_id");
		average_speed = ParseDouble("average_speed");
		max_speed = ParseDouble("max_speed");
		average_cadence = ParseDouble("average_cadence");
		average_temp = ParseDouble("average_temp");
		average_watts = ParseDouble("average_watts");
		max_watts = ParseInt("max_watts");
		weighted_avereage_watts = ParseInt("weighted_average_watts");
		kilojoules = ParseDouble("kilojoules");
		device_watts = ParseBool("device_watts");
		has_heartrate = ParseBool("has_heartrate");
		if (has_heartrate)
		{
			average_heartrate = ParseDouble("average_heartrate");
			max_heartrate = ParseInt("max_heartrate");
		}
		suffer_score = ParseInt("suffer_score");
		has_kudoed = ParseBool("has_kudoed");
		
	case 1:;
	}

}

Activity_t::~Activity_t()
{
	delete map;
	delete gear;
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

Point_t::Point_t(const rapidjson::Value & dom):Point_t()
{
	if (!dom.IsArray() || dom.GetArray().Size()!=2 )
		throw StravaException_t("Point_t is not array of length 2");
	latitude = dom.GetArray()[0].GetDouble();
	longtitude = dom.GetArray()[1].GetDouble();
}
