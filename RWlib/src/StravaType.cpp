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

Activity_t::Activity_t(const string & json)
{

}
