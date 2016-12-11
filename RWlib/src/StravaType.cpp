#include "StravaType.h"
#define RAPIDJSON_SSE42
#include "rapidjson/document.h"



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
