

#include <iostream>
#include <fstream>
#include "ReadJson.h"
#include "StravaType.h"
#include "StravaApi.h"


int main(int argc, char** argv)
{
   ReadAthlete("athlete.json");
   //ReadAthlete("athlete_error.json");
   ReadActivity("activity.json"); 
   
   RideWeather::AccessToken_t acces_token = ReadToken("token");
   string Athlete_json = RideWeather::StravaGetAthlete(acces_token);
   std::ofstream fid;
   fid.open("C:/Users/jarno_000/Documents/StravaData/athlete.txt", std::ios::out | std::ios::binary);
   fid << Athlete_json;
   fid.close();
   try {
	   RideWeather::Athlete_t athlete(Athlete_json);
   }
   catch (RideWeather::StravaException_t& ex)
   {
	   std::cerr << "Error reading athlete from access token" << std::endl;
	   std::cerr << ex.what() << std::endl;
   }
   
    return 0;
}
