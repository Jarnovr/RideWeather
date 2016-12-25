#define _CRT_SECURE_NO_WARNINGS
#include "StravaType.h"
#include <iostream>
#include <fstream>


using namespace std;

void ReadAthlete(const char *fn)
{
	try {
		std::ifstream fid;
		fid.open(fn, ios::in | ios::binary | ios::ate);

		// get filesize
		size_t fs = fid.tellg();
		char *json = new char[fs+1];
		fid.seekg(0, ios::beg);
		fid.read(json, fs);
		fid.close();
		json[fs] = '\0';
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
		std::ifstream fid;
		fid.open(fn, ios::in | ios::binary | ios::ate);

		// get filesize
		size_t fs = fid.tellg();
		char *json = new char[fs + 1];
		fid.seekg(0, ios::beg);
		fid.read(json, fs);
		fid.close();
		json[fs] = '\0';
		// Create Athlete
		Activity_t *activity;
		activity = new Activity_t(json);
	}
	catch (StravaException_t& ex)
	{
		cerr << "Error reading activity.json:" << endl;
		cerr << ex.what() << endl;
	}
};
