#pragma once

#include <QDateTime>
#include "StravaType.h"

inline const QDateTime ToQDateTime(const RideWeather::Times_t & time) 
{
	return QDateTime::fromSecsSinceEpoch(boost::posix_time::to_time_t(time), Qt::UTC);
}

inline const RideWeather::Times_t ToTimes_t(const QDateTime& dateTime)
{
	return boost::posix_time::from_time_t(dateTime.toSecsSinceEpoch());
}