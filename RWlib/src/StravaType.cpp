#include "StravaType.h"
#define RAPIDJSON_SSE42
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;

void Abort(const char *str)
{
	fprintf(stderr, "Error: %s\nAborting...\n", str);
	exit(-1);
}


Athlete_t::Athlete_t(const char* json) 
{
	//First create rapidjson object
	Document document;
	Value field;
	document.Parse<ParseFlag::kParseFullPrecisionFlag | ParseFlag::kParseCommentsFlag | ParseFlag::kParseNanAndInfFlag>(json);
	//generic error checking
	if (document.HasParseError())
	{
		size_t of=document.GetErrorOffset();
		ParseErrorCode ec=document.GetParseError();
		fprintf(stderr, "Athlete_t, at offset %zu, parsing error %s\n", of, GetParseError_En(ec));
	}


	//go over returned dom and do error/sanity check and fill the data structure
	if (!document.IsObject())
		Abort("Athlete_t::Athlete_t(), returned DOM no object");
	if (!document.HasMember("id"))
		Abort("Athlete missing id");
	field=document["id"];
	if (!field.IsInt())
		Abort("Athlete_t id not integer");
	
}


Athlete_t::~Athlete_t()
{
	delete [] firstname;
	delete[] lastname;
	delete profile_medium;
	delete profile;
	delete[] city;
	delete[] state;
	delete[] country;
	delete[] created_at;
	delete[] updated_at;
	delete[] date_preference;
	delete[] email;

}
