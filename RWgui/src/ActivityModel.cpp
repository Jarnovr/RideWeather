#include "ACtivityModel.h"
#include "Common.h"

#include <QDateTime>
namespace RideWeather
{
	using namespace std::string_literals;

	int ActivityModel::rowCount(const QModelIndex & parent) const
	{
		if (parent.isValid())
			throw std::runtime_error("rowCount asked for non-root parent. Not Allowed."s);

		return (int) _athlete->activities.size();
	}

	int ActivityModel::columnCount(const QModelIndex & parent) const
	{
		if (parent.isValid())
			throw std::runtime_error("columnCount asked for non-root parent. Not Allowed."s);

		return 7;//number of data fields exposed.
	}


	QVariant ActivityModel::data(const QModelIndex &index, int role) const
	{
		if (_athlete->isActIdListOutDated())
			_athlete->MakeActivityIds();

		switch (role)
		{
		case Qt::DisplayRole:
			switch (index.column())
			{
			case 0: return QVariant(QString::fromStdString(_athlete->activities.at(_athlete->activity_ids[index.row()]).name));
			case 1: {
				auto startdate = _athlete->activities.at(_athlete->activity_ids[index.row()]).start_date;
				return QVariant(ToQDateTime(startdate));
			}
			case 2: return QVariant(_athlete->activities.at(_athlete->activity_ids[index.row()]).distance/1000.0);
			case 3: return QVariant(_athlete->activities.at(_athlete->activity_ids[index.row()]).elapsed_time/60.0);
			case 4: return QVariant(_athlete->activities.at(_athlete->activity_ids[index.row()]).moving_time/60.0);
			case 5: return QVariant(_athlete->activities.at(_athlete->activity_ids[index.row()]).average_speed *3.6);
			case 6: return QVariant(_athlete->activities.at(_athlete->activity_ids[index.row()]).distance / _athlete->activities.at(_athlete->activity_ids[index.row()]).moving_time *3.6);

			default: return QVariant();
			}			
		}

		return QVariant();
	}

	QVariant ActivityModel::headerData(int section, Qt::Orientation orientation,
		int role) const
	{
		switch (orientation)
		{
		case Qt::Horizontal : 
			switch (role)
			{
			case Qt::DisplayRole:
				switch (section)
				{
				case 0: return QVariant("Name");
				case 1: return QVariant("Date");
				case 2: return QVariant("Distance (km)");
				case 3: return QVariant("Elapsed Time (min)"); 
				case 4: return QVariant("Moving Time (min)");
				case 5: return QVariant("Strava Velocity (km/h)");
				case 6: return QVariant("Moving Velocity (km/h)");
				default: return QVariant();
				}				
			default:
				return QVariant();
			}
			break;
		case Qt::Vertical :
			switch (role)
			{
			case Qt::DisplayRole:
				return QVariant(section);
			default:
				return QVariant();
			}			
		}
		return QVariant();
	}

}