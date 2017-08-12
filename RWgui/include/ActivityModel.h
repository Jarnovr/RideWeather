#pragma once

#include <cstddef>

#include <array>
#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#include <QAbstractTableModel>
#include "StravaType.h"

namespace RideWeather
{

	class ActivityModel : public  QAbstractTableModel
	{
	protected:
		std::shared_ptr<Athlete_t> _athlete;
	public:
		ActivityModel(std::shared_ptr<Athlete_t> athlete) : _athlete(athlete) {
			_athlete->MakeActivityIds();
		};
		
		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		int columnCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation,
			int role = Qt::DisplayRole) const;
	};


}