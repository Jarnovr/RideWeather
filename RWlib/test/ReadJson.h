#pragma once
#include "StravaType.h"

void ReadAthlete(const char *fn);
void ReadActivity(const char *fn);
RideWeather::AccessToken_t ReadToken(const string& fn);