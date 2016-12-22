#include "StravaType.h"
#define RAPIDJSON_SSE42
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include <iostream>
using namespace rapidjson;

Athlete_t::Athlete_t(const string json) :Athlete_t()
{
	//First create rapidjson object
	Document document;
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

	if (!document.HasMember("id"))
		throw StravaException_t("Athlete missing id\n");
	if (!document["id"].IsInt())
		throw StravaException_t("Athlete_t id not integer\n");
	id = document["id"].GetInt64();

	if (!document.HasMember("resource_state"))
		throw StravaException_t("Athlete missing resource_state\n");
	if (!document["resource_state"].IsInt())
		throw StravaException_t("Athlete_t resource_state not integer\n");
	resource_state = document["resource_state"].GetInt();
	if (resource_state <= 0 || resource_state > 3)
		throw StravaException_t("Invalid resource_state.\n");
	
	switch (resource_state)
	{
	case 3:

	case 2:
		if (!document.HasMember("firstname"))
			throw StravaException_t("Athlete missing firstname\n");
		if (!document["firstname"].IsString())
			throw StravaException_t("Athlete_t firstname not string\n");
		firstname.assign(document["firstname"].GetString(), document["firstname"].GetStringLength());
		
		if (!document.HasMember("lastname"))
			throw StravaException_t("Athlete missing lastname\n");
		if (!document["lastname"].IsString())
			throw StravaException_t("Athlete_t lastname not string\n");
		lastname.assign(document["lastname"].GetString(), document["lastname"].GetStringLength());


		if (!document.HasMember("profile_medium"))
			throw StravaException_t("Athlete missing profile_medium\n");
		if (!document["profile_medium"].IsString())
			throw StravaException_t("Athlete_t profile_medium not string\n");
		profile_medium.assign(document["profile_medium"].GetString(), document["profile_medium"].GetStringLength());
		
		if (!document.HasMember("profile"))
			throw StravaException_t("Athlete missing profile\n");
		if (!document["profile"].IsString())
			throw StravaException_t("Athlete_t profile not string\n");
		profile.assign(document["profile"].GetString(), document["profile"].GetStringLength());

		if (!document.HasMember("city"))
			throw StravaException_t("Athlete missing city\n");
		if (!document["city"].IsString())
			throw StravaException_t("Athlete_t city not string\n");
		city.assign(document["city"].GetString(), document["city"].GetStringLength());


		if (!document.HasMember("state"))
			throw StravaException_t("Athlete missing state\n");
		if (!document["state"].IsString())
			throw StravaException_t("Athlete_t state not string\n");
		state.assign(document["state"].GetString(), document["state"].GetStringLength());


		if (!document.HasMember("country"))
			throw StravaException_t("Athlete missing country\n");
		if (!document["country"].IsString())
			throw StravaException_t("Athlete_t country not string\n");
		country.assign(document["country"].GetString(), document["country"].GetStringLength());


		if (!document.HasMember("sex"))
			throw StravaException_t("Athlete missing sex\n");
		if (!document["sex"].IsString())
			throw StravaException_t("Athlete_t sex not char\n");
		sex = *document["sex"].GetString();

		if (!document.HasMember("friend"))
			throw StravaException_t("Athlete missing friend\n");
		if (document["friend"].IsNull())
			friend_ = Connection_t::null;
		else
		{
			if (!document["friend"].IsString())
				throw StravaException_t("Athlete_t friend not string\n");
			if (!strcmp(document["friend"].GetString(), "pending"))
				friend_ = Connection_t::pending;
			else if (!strcmp(document["friend"].GetString(), "accepted"))
				friend_ = Connection_t::accepted;
			else if (!strcmp(document["friend"].GetString(), "blocked"))
				friend_ = Connection_t::blocked;
			else friend_ = Connection_t::null;
		}

		if (!document.HasMember("follower"))
			throw StravaException_t("Athlete missing follower\n");
		if (document["follower"].IsNull())
			follower = Connection_t::null;
		else
		{
			if (!document["follower"].IsString())
				throw StravaException_t("Athlete_t follower not string\n");
			if (!strcmp(document["follower"].GetString(), "pending"))
				follower = Connection_t::pending;
			else if (!strcmp(document["follower"].GetString(), "accepted"))
				follower = Connection_t::accepted;
			else if (!strcmp(document["follower"].GetString(), "blocked"))
				follower = Connection_t::blocked;
			else follower = Connection_t::null;
		}
		if (!document.HasMember("premium"))
			throw StravaException_t("Athlete missing premium\n");
		if (!document["premium"].IsBool())
			throw StravaException_t("Athlete_t premium not bool\n");
		premium = document["premium"].GetBool();




	case 1:;
	}
	
}


Athlete_t::~Athlete_t()
{

}
