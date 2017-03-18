#define _CRT_SECURE_NO_WARNINGS
#include "StravaType.h"
#include <iostream>
#include <fstream>


using namespace std;
using namespace RideWeather;

const static string dir{ "C:/Users/jarno_000/Documents/StravaData/" };

void ReadAthlete(const char *fn)
{
	try {
		//open file
		boost::filesystem::ifstream file(fn, std::ios::binary | std::ios::ate);
		// get filesize
		size_t fs = file.tellg();
		file.seekg(0, std::ios::beg);
		//reserver memory
		string json;
		json.reserve(fs);
		//read using iteratior
		json.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		file.close();
		// Create Athlete
		Athlete_t *athlete;
		athlete = new Athlete_t(json);
	}
	catch (StravaException_t& ex)
	{
		cerr << "Error reading athlete.json:" << endl;
		cerr << ex.what() << endl;
	}
    
    
};



void ReadActivity(const char *fn)
{
	try {
		//open file
		boost::filesystem::ifstream file(fn, std::ios::binary | std::ios::ate);
		// get filesize
		size_t fs = file.tellg();
		file.seekg(0, std::ios::beg);
		//reserver memory
		string json;
		json.reserve(fs);
		//read using iteratior
		json.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		file.close();
		Activity_t *activity;
		activity = new Activity_t(json);
	}
	catch (StravaException_t& ex)
	{
		cerr << "Error reading activity.json:" << endl;
		cerr << ex.what() << endl;
	}
};

AccessToken_t ReadToken(const string& fn)
{
	try {
		//open file
		boost::filesystem::ifstream file(fn, std::ios::binary | std::ios::ate);
		// get filesize
		size_t fs = file.tellg();
		file.seekg(0, std::ios::beg);
		//reserver memory
		string json;
		json.reserve(fs);
		//read using iteratior
		json.assign((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		file.close();
		// Create Athlete
		return AccessToken_t(json);		
	}
	catch (StravaException_t& ex)
	{
		cerr << "Error reading token:" << endl;
		cerr << ex.what() << endl;
		return AccessToken_t();
	}
};
