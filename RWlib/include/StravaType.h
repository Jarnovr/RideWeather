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
#include "boost/date_time.hpp"
#include "boost/filesystem.hpp"
#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "enum.h"
using std::string;

namespace RideWeather
{
	typedef boost::posix_time::ptime Times_t;
	
	class StravaException_t : public std::runtime_error
	{
	public:
		StravaException_t(const string& str) : std::runtime_error(str) {};
	};

	class Strava_t {
	protected:
		rapidjson::Document *document;
		rapidjson::Value *dom;
		string type_str;
		Strava_t() { document = new rapidjson::Document; dom = new rapidjson::Value; }
		virtual ~Strava_t() { if (dom!=nullptr) { delete dom; dom = nullptr; }
			if (document!=nullptr) { delete document; document = nullptr; }
			};

		void ParseString(string& dest, string var)
		{
			if (!dom->HasMember(var))
				throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
			if ((*dom)[var].IsNull())
			{
				dest.assign(""); 
				return;
			}
			if (!(*dom)[var].IsString())
				throw StravaException_t(type_str.append(" ").append(var).append(" not a astring\n"));
			dest.assign((*dom)[var].GetString(), (*dom)[var].GetStringLength());
		}

		void ParseStringIf(string& dest, string var)
		{
			if (dom->HasMember(var))
			{
				if ((*dom)[var].IsNull())
				{
					dest.assign("");
					return;
				}
				if (!(*dom)[var].IsString())
					throw StravaException_t(type_str.append(" ").append(var).append(" not a astring\n"));
				dest.assign((*dom)[var].GetString(), (*dom)[var].GetStringLength());
			}
			else
				dest.assign("");
		}

		void ParseTimeS(Times_t& dest, string var)
		{
			if (!dom->HasMember(var))
				throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
			if ((*dom)[var].IsNull())
				return;
			if (!(*dom)[var].IsString())
				throw StravaException_t(type_str.append(" ").append(var).append(" not a astring\n"));
			string tmp((*dom)[var].GetString(), (*dom)[var].GetStringLength());
			//cut off last character (Z), as it is not part of format string
			dest = boost::date_time::parse_delimited_time<boost::posix_time::ptime>(tmp.substr(0, tmp.length() - 1), 'T');
		}


		int ParseInt(string var)
		{
			if (!dom->HasMember(var))
				throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
			if ((*dom)[var].IsNull())
				return 0;
			if (!(*dom)[var].IsInt())
				throw StravaException_t(type_str.append(" ").append(var).append(" not a int\n"));
			return (*dom)[var].GetInt();
		}
		int ParseIntIf(string var)
		{
			if (dom->HasMember(var))
			{
				if ((*dom)[var].IsNull())
					return 0; 
				if (!(*dom)[var].IsInt())
					throw StravaException_t(type_str.append(" ").append(var).append(" not a int\n"));
				return (*dom)[var].GetInt();
			}
			else
				return 0;
		}

		ptrdiff_t ParseInt64(string var)
		{
			if (!dom->HasMember(var))
				throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
			if ((*dom)[var].IsNull())
				return 0;
			if (!(*dom)[var].IsInt64())
				throw StravaException_t(type_str.append(" ").append(var).append(" not a int64\n"));
			return (*dom)[var].GetInt64();
		}
		ptrdiff_t ParseInt64If(string var)
		{
			if (dom->HasMember(var))
			{
				if ((*dom)[var].IsNull())
					return 0;
				if (!(*dom)[var].IsInt64())
					throw StravaException_t(type_str.append(" ").append(var).append(" not a int64\n"));
				return (*dom)[var].GetInt64();
			}
			else
				return 0;
		}

		double ParseDouble(string var)
		{
			if (!dom->HasMember(var))
				throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
			if ((*dom)[var].IsNull())
				return 0.0;

			if (!(*dom)[var].IsDouble())
				return 0.0;
			return (*dom)[var].GetDouble();
		}
		double ParseDoubleIf(string var)
		{
			if (dom->HasMember(var))
			{
				if ((*dom)[var].IsNull())
					return 0.0;
				if (!(*dom)[var].IsDouble())
					throw StravaException_t(type_str.append(" ").append(var).append(" not a double\n"));
				return (*dom)[var].GetDouble();
			}
			else
				return 0.0;

		}

		bool ParseBool(string var)
		{
			if (!dom->HasMember(var))
				throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
			if (!(*dom)[var].IsBool())
				throw StravaException_t(type_str.append(" ").append(var).append(" not a bool\n"));
			return (*dom)[var].GetBool();
		}

		bool ParseBoolIf(string var)
		{
			if (dom->HasMember(var))
			{
				if (!(*dom)[var].IsBool())
					throw StravaException_t(type_str.append(" ").append(var).append(" not a bool\n"));
				return (*dom)[var].GetBool();
			}
			else
				return false;
		}
		void DebugPrintDom()
		{
#ifndef NDEBUG
			rapidjson::StringBuffer buffer;
			buffer.Clear();
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			dom->Accept(writer);
			std::cerr << type_str << ": " << buffer.GetString() << std::endl;
#endif // !NDEBUG
		}


		void ParseJson(const string & json);
		virtual void ParseDom() = 0;
	};



	class Athlete_t;

	class AccessToken_t : public Strava_t {
	public:
		char access_token[41];
		string token_type;
		std::shared_ptr<Athlete_t> athlete;
		AccessToken_t() : athlete(nullptr) { type_str.assign("AccessToken"); };
		AccessToken_t(const AccessToken_t& at) { type_str.assign("AccessToken"); strncpy(access_token, at.access_token, 40); token_type = at.token_type; athlete = at.athlete; };
		AccessToken_t(const string& json) :AccessToken_t() { ParseJson(json); ParseDom(); };
		AccessToken_t(const boost::filesystem::path& filename);
	public:
		void ParseDom();
	};


	typedef string URL_t;
	
	class Club_t;
	class Bike_t;
	class Shoe_t;
	class Activity_t;
	enum class MeasurementType_t { feet, meters };
	enum class StreamType_t;
	class Stream_t;

	class Athlete_t : public Strava_t {
	public:
		enum class Connection_t { null, pending, accepted, blocked };
		enum class AthleteType_t : int { cyclist = 0, runner = 1 };

		ptrdiff_t id;//summary
		ptrdiff_t resource_state;//summary
		string firstname;//summary
		string lastname;//summary
		URL_t profile_medium;//summary
		URL_t profile;//summary
		string city;//summary
		string state;//summary
		string country;//summary
		char sex;//summary
		Connection_t friend_;//summary
		Connection_t follower;//summary
		bool premium;//summary
		Times_t created_at;//summary
		Times_t updated_at;//summary
		ptrdiff_t follower_count;//detailed only
		ptrdiff_t friend_count;//detailed only
		ptrdiff_t mutual_friend_count;//deiled only
		AthleteType_t athlete_type;//detailed only
		string date_preference;//format string//detailed only
		MeasurementType_t measurement_preference;//detailed only
		ptrdiff_t ftp;//detailed only
		double weight;//detailed only
		std::list<Club_t> clubs;//detailed only
		std::list<Bike_t> bikes;//detailed only
		std::list<Shoe_t> shoes;//detailed only
		std::map<ptrdiff_t,Activity_t> activities;
		std::vector<ptrdiff_t> activity_ids;
		Times_t last_activity;
		Athlete_t() :id(0), resource_state(0), sex('\0'),
			friend_(Connection_t::null), follower(Connection_t::null), premium(false),
			follower_count(0), friend_count(0),
			mutual_friend_count(0), athlete_type(AthleteType_t(0)),
			measurement_preference(MeasurementType_t::meters), ftp(0), weight(0.0), last_activity(boost::gregorian::date(1990,1,1)) {
			type_str.assign("Athlete_t");
		};
		Athlete_t(const string& json) : Athlete_t() { ParseJson(json); ParseDom(); };
		Athlete_t(rapidjson::Value& DOM) : Athlete_t() { dom->CopyFrom(DOM, document->GetAllocator()); 	ParseDom(); };
		virtual ~Athlete_t() {};
		void MakeActivityIds();
		bool isActIdListOutDated() const;
	protected:
		void ParseDom();
	};

	class Zones_t : public Strava_t {
	public:
		bool heart_rate_custom;
		struct zone_t { short min; short max; };
		std::vector<zone_t> heart_zones;
		std::vector<zone_t> power_zones;
		Zones_t() : heart_rate_custom(false) { type_str.assign("Zones_t"); };
		Zones_t(const string& json) : Zones_t() { ParseJson(json); ParseDom(); };
	protected:
		void ParseDom();
	};

	class Total_t : public Strava_t {
	public:
		ptrdiff_t count;
		double distance;
		ptrdiff_t moving_time;
		ptrdiff_t elapsed_time;
		double elevation_gain;
		ptrdiff_t achievement_count;
		Total_t() : count(0), distance(0.0), moving_time(0), elapsed_time(0),
			elevation_gain(0.0), achievement_count(0) {
			type_str.assign("Total_t");
		};
		Total_t(const string& json) : Total_t() { ParseJson(json); ParseDom(); };
		Total_t(rapidjson::Value& DOM) : Total_t() { dom->CopyFrom(DOM, document->GetAllocator()); 	ParseDom(); };
	protected:
		void ParseDom();
	};

	class Totals_t :public Strava_t {
	public:
		double biggest_ride_distance;
		double biggest_climb_elevation_gain;
		Total_t recent_ride_totals;
		Total_t recent_run_totals;
		Total_t recent_swim_totals;
		Total_t ytd_ride_totals;
		Total_t ytd_run_totals;
		Total_t ytd_swim_totals;
		Total_t all_ride_totals;
		Total_t all_run_totals;
		Total_t all_swim_totals;
		Totals_t() : biggest_ride_distance(0.0), biggest_climb_elevation_gain(0.0) { type_str.assign("Totals_t"); };
		Totals_t(const string& json) : Totals_t() { ParseJson(json); ParseDom(); };
	protected:
		void ParseDom();
	};

	class Point_t {
	public:
		double latitude;
		double longtitude;
		Point_t() : latitude(0.0), longtitude(0.0) { };
		Point_t(const double lat, const double lon) : latitude(lat), longtitude(lon) {};
		Point_t(const rapidjson::Value& dom);
	};

	class Photo_t;
	class Map_t;
	class Gear_t;
	class SegmentEffort_t;
	class Splits_t;
	class Effort_t;
	class Lap_t;



	enum class Data_t { kInt, kDouble, kBool, kPoint, kNull };
	enum class StreamType_t {
		time, latlng, distance, altitude, velocity_smooth,
		heartrate, cadence, watts, temp, moving, grade_smooth, null
	};

	class Stream_t : public Strava_t {
	public:
		enum class Resolution_t { low, medium, high, def };
		string type;
		string  series_type;
		ptrdiff_t original_size;
		Resolution_t resolution;
		StreamType_t stream_type;
		Data_t data_type;
		Stream_t() : original_size(0), resolution(Resolution_t::low), stream_type(StreamType_t::null),
			data_type(Data_t::kNull) {
			type_str.assign("Stream_t");
		};
		virtual void* GetData() = 0;
		virtual size_t GetSize() = 0;
		static Data_t DataTypeFromStreamType(StreamType_t stream_type);
		static StreamType_t GetStreamType(rapidjson::Value& dom);
		static void ParseStreamArray(const string &json, Activity_t &activity);
		virtual ~Stream_t() { ; };
	protected:
		void ParseDom();
		virtual void ParseData() = 0;
	};

	class Bool
	{
	public:
		Bool() : m_value() {}
		Bool(bool value) : m_value(value) {}
		operator bool() const { return m_value; }
		// the following operators are to allow bool* b = &v[0]; (v is a vector here).
		bool* operator& () { return &m_value; }
		const bool * const operator& () const { return &m_value; }
	private:
		bool m_value;
	};


	template <typename T>
	class StreamRaw_t : public Stream_t {
	public:
		std::vector<T> data;
		StreamRaw_t() : Stream_t() {};
		StreamRaw_t(const string& json) : StreamRaw_t() { ParseJson(json); ParseDom(); };
		StreamRaw_t(rapidjson::Value& DOM) : StreamRaw_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
		virtual ~StreamRaw_t() { ; };
		void* GetData()
		{
			return (void*)data.data();
		};
		size_t GetSize() { return data.size(); };
	protected:
		void ParseData();
	};


	template <typename T>
	inline void StreamRaw_t<T>::ParseData()
	{
		throw StravaException_t("Stream_t general ParseData called");
	}

	template <>
	inline void StreamRaw_t<int>::ParseData()
	{
		data_type = Data_t::kInt;
		data.reserve(original_size);

		for (auto& v : (*dom)["data"].GetArray())
		{
			data.push_back(v.GetInt());
		}
	}

	template <>
	inline void StreamRaw_t<double>::ParseData()
	{
		data_type = Data_t::kDouble;
		data.reserve(original_size);

		for (auto& v : (*dom)["data"].GetArray())
		{
			data.push_back(v.GetDouble());
		}
	}

	template <>
	inline void StreamRaw_t<Bool>::ParseData()
	{
		data_type = Data_t::kBool;
		data.reserve(original_size);

		for (auto& v : (*dom)["data"].GetArray())
		{
			data.push_back(v.GetBool());
		}
	}

	template <>
	inline void StreamRaw_t<Point_t>::ParseData()
	{
		data_type = Data_t::kPoint;
		data.reserve(original_size);

		for (auto& v : (*dom)["data"].GetArray())
		{
			data.push_back(Point_t(v));
		}
	}

	BETTER_ENUM(ActivityType_t, char,
		All, Ride, Run, Swim, Hike, Walk, AlpineSki,
		BackcountrySki, Canoeing, Crossfit, EBikeRide, Elliptical, IceSkate,
		InlineSkate, Kayaking, Kitesurf, NordicSki, RockClimbing, RollerSki,
		Rowing, Snowboard, Snowshoe, StairStepper, StandUpPaddling, Surfing,
		VirtualRide, WeightTraining, Windsurf, Workout, Yoga, other);

	ActivityType_t ActivityType(const string& str);
	string ActivityType(const ActivityType_t type);

	class Activity_t :public Strava_t {
	public:
		enum class Workout_t { default_workout = 0, race = 1, longrun = 2, workout = 3, default_ride = 10, race_ride = 11, workout_ride = 12 };
		ptrdiff_t id;
		ptrdiff_t resource_state;
		string external_id;
		ptrdiff_t upload_id;
		std::shared_ptr<Athlete_t> athlete;
		string name;
		string description;
		double distance;//meters
		ptrdiff_t moving_time;//seconds
		ptrdiff_t elapsed_time;//seconds
		double total_elevation_gain;//meters
		double elev_high;
		double elev_low;
		ActivityType_t type;
		string typeS;
		Times_t start_date;
		Times_t start_date_local;
		string timezone_act;
		Point_t start_latlng;
		Point_t end_latlng;
		ptrdiff_t achievement_count;
		ptrdiff_t kudos_count;
		ptrdiff_t comment_count;
		ptrdiff_t athlete_count;
		ptrdiff_t photo_count;
		ptrdiff_t total_photo_count;
		std::list<Photo_t> photos;
		std::shared_ptr<Map_t> map;
		bool trainer;
		bool commute;
		bool manual;
		bool private_act;
		string device_name;
		string embed_token;
		bool flagged;
		Workout_t workout_type;
		string gear_id;
		std::shared_ptr<Gear_t> gear;
		double average_speed;//m/s
		double max_speed;//m/s
		double average_cadence;
		double average_temp;
		double average_watts;
		ptrdiff_t max_watts;
		ptrdiff_t weighted_avereage_watts;
		double kilojoules;
		bool device_watts;
		bool has_heartrate;
		double average_heartrate;
		double max_heartrate;
		double calories; //kilocalories, kJ (ride), pace(runs)
		ptrdiff_t suffer_score;
		bool has_kudoed;
		std::list<SegmentEffort_t> segment_efforts;
		std::list<Splits_t> splits_metric;
		std::list<Splits_t> splits_standard;
		std::list<Lap_t> laps;
		std::list<Effort_t> best_efforts;
		std::unordered_map<StreamType_t, Stream_t*> streams;
		Activity_t() : id(0), resource_state(0), upload_id(0), athlete(nullptr), distance(0.0),
			moving_time(0), elapsed_time(0), total_elevation_gain(0.0), elev_high(0.0), elev_low(0.0),
			type(ActivityType_t::other), start_latlng(0.0, 0.0), end_latlng(0.0, 0.0),
			achievement_count(0), kudos_count(0), comment_count(0), athlete_count(0), photo_count(0),
			total_photo_count(0), map(nullptr), trainer(false), commute(false), manual(false),
			private_act(false), flagged(false), workout_type(Workout_t::default_workout),
			gear(nullptr), average_speed(0.0), max_speed(0.0), average_cadence(0.0),
			average_temp(0.0), average_watts(0.0), max_watts(0), weighted_avereage_watts(0),
			kilojoules(0.0), device_watts(false), has_heartrate(false), average_heartrate(0.0),
			max_heartrate(0), calories(0.0), suffer_score(0), has_kudoed(0) {
			type_str.assign("Activity_t");//set type str
		};
		Activity_t(const string& json) :Activity_t() { ParseJson(json); ParseDom(); };
		Activity_t(rapidjson::Value& DOM) : Activity_t() { dom->CopyFrom(DOM, document->GetAllocator()); 	ParseDom(); };
		virtual ~Activity_t() {
			//cleanup streams
			for (auto &v : streams)
			{
				auto streamtype = v.first;
				auto datatype = Stream_t::DataTypeFromStreamType(streamtype);
				switch (datatype)
				{
				case Data_t::kInt:
					delete (StreamRaw_t<int>*)(v.second);
					break;
				case Data_t::kDouble:
					delete (StreamRaw_t<double>*)(v.second);
					break;
				case Data_t::kBool:
					delete (StreamRaw_t<Bool>*)(v.second);
					break;
				case Data_t::kPoint:
					delete (StreamRaw_t<Point_t>*)(v.second);
					break;
				default:					
					break;
				}				
			}
		}
	protected:
		void ParseDom();
	};

	class Achievement_t : public Strava_t {
	public:
		enum class AchievementType_t { overall = 2, pr = 3, year_overall = 5 };
		AchievementType_t type_id;
		string  type;
		ptrdiff_t rank;
		Achievement_t() : type_id(AchievementType_t::overall), rank(0) {
			type_str.assign("Achievement_t");//set type str
		};
		Achievement_t(const string & json) : Achievement_t() { ParseJson(json); ParseDom(); };
		Achievement_t(rapidjson::Value & DOM) : Achievement_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
	protected:
		void ParseDom();
	};

	class Club_t : public Strava_t {
	public:
		enum class ClubType_t { casual_club, racing_team, shop, company, club_other };
		enum class Sport_t { cycling, running, triathlon, sport_other };
		enum class Membership_t { null, member, pending };
		ptrdiff_t id;
		ptrdiff_t resource_state;
		string name;
		URL_t profile_medium;
		URL_t profile;
		URL_t cover_photo;
		URL_t cover_photo_small;
		string description;
		ClubType_t club_type;
		Sport_t sport_type;
		string city;//summary
		string state;//summary
		string country;//summary
		bool private_club;
		ptrdiff_t member_count;
		bool featured;
		bool verified;
		Membership_t membership;
		bool admin;
		bool owner;
		ptrdiff_t following_count;
		URL_t url;
		Club_t() : id(0), resource_state(0), club_type(ClubType_t::club_other), sport_type(Sport_t::sport_other),
			private_club(false), member_count(0), featured(false), verified(false),
			membership(Membership_t::null), admin(false), owner(false), following_count(0) {
			type_str.assign("Club_t");
		};
		Club_t(const string & json) : Club_t() { ParseJson(json); ParseDom(); };
		Club_t(rapidjson::Value& DOM) : Club_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
	protected:
		void ParseDom();
	};

	class Gear_t : public Strava_t {
	public:
		string id;
		bool primary;
		string name;
		double distance;
		string description;
		ptrdiff_t resource_state;
		Gear_t() : primary(false), distance(0.0), resource_state(0) { type_str.assign("Gear_t"); };
		Gear_t(const string & json) : Gear_t() { ParseJson(json); ParseDom(); };
		Gear_t(rapidjson::Value& DOM) : Gear_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
	protected:
		void ParseDom();
	};

	class Bike_t : public Gear_t {
	public:
		string brand_name;
		string model_name;
		enum class FrameType_t { mtb = 1, cross = 2, road3, timetrial = 4 };
		FrameType_t frame_type;
		Bike_t() : Gear_t(), frame_type(FrameType_t::mtb) { type_str.assign("Bike_t"); };
		Bike_t(rapidjson::Value& DOM);
	};

	class Shoe_t : public Gear_t {
	public:
		Shoe_t() : Gear_t() { type_str.assign("Shoe_t"); };
		Shoe_t(rapidjson::Value& DOM) : Gear_t(DOM) { type_str.assign("Shoe_t"); delete dom; dom = nullptr; delete document; document = nullptr;};
	};

	class Segment_t;
	class Polyline_t {
	public:
		string PolyString;
		std::vector<Point_t> points;
		Polyline_t() { };
		Polyline_t(const string& str);
	};

	class Map_t : public Strava_t {
	public:
		string id;
		std::unique_ptr<Polyline_t> summary_polyline;
		ptrdiff_t resource_state;
		std::unique_ptr<Polyline_t> polyline;
		Map_t() : resource_state(0) { type_str.assign("Map_t"); };
		Map_t(rapidjson::Value& DOM) : Map_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
	protected:
		void ParseDom();
	};

	class Route_t : public Strava_t {
	public:
		ptrdiff_t id;
		ptrdiff_t resource_state;
		string name;
		string description;
		std::shared_ptr<Athlete_t> athlete;
		double distance;//meters
		double elevation_gain;//meters
		std::shared_ptr<Map_t> map;
		enum class Type_t { ride = 1, run = 2, other = 0 };
		Type_t type;
		enum class SubType_t { road = 1, mtb = 2, cx = 3, trail = 4, mixed = 5, other = 0 };
		SubType_t sub_type;
		bool private_route;
		bool starred;
		ptrdiff_t timestamp;//unix timestap
		std::list<Segment_t> segments;
		Route_t() : id(0), resource_state(0), distance(0.0), elevation_gain(0.0), athlete(nullptr),
			map(nullptr), type(Type_t::other), sub_type(SubType_t::road), private_route(false),
			starred(false), timestamp(0) {
			type_str.assign("Route_t");
		}
		Route_t(rapidjson::Value & DOM) : Route_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
		Route_t(string & json) : Route_t() { ParseJson(json); ParseDom(); };
	protected:
		void ParseDom();
	};


	class Race_t : public Strava_t {
	public:
		ptrdiff_t id;
		ptrdiff_t resource_state;
		string name;
		enum class RunningRace_t { road = 0, trail = 1, track = 2, xc = 3 };
		RunningRace_t running_race_type;
		double distance;
		Times_t start_date_local;
		string city;
		string state;
		string country;
		std::list<ptrdiff_t> route_ids;
		MeasurementType_t measurement_preference;
		URL_t url;
		URL_t website_url;
		Race_t() : id(0), resource_state(0), running_race_type(RunningRace_t::road), distance(0.0),
			measurement_preference(MeasurementType_t::meters) {
			type_str.assign("Race_t");
		};
		Race_t(const string& json) : Race_t() { ParseJson(json); ParseDom(); };
	protected:
		void ParseDom();
	};

	class Segment_t : Strava_t {
	public:
		ptrdiff_t id;
		ptrdiff_t resource_state;
		string name;
		ActivityType_t activity_type;
		double distance;
		double average_grade;
		double maximum_grade;
		double elevation_high;
		double elevation_low;
		Point_t start_latlng;
		Point_t end_latlng;
		ptrdiff_t climb_category;
		string city;//summary
		string state;//summary
		string country;//summary
		bool private_segment;
		bool starred;
		Times_t created_at;
		Times_t updated_at;
		double total_elevation_gain;
		std::shared_ptr<Map_t> map;
		ptrdiff_t effort_count;
		ptrdiff_t athlete_count;
		bool hazardous;
		ptrdiff_t star_count;
		Segment_t() : id(0), resource_state(0), activity_type(ActivityType_t::other), distance(0.0),
			average_grade(0.0), maximum_grade(0.0), elevation_high(0.0), elevation_low(0.0),
			start_latlng({ 0.0, 0.0 }), end_latlng({ 0.0, 0.0 }), climb_category(0), private_segment(false),
			starred(false), total_elevation_gain(0.0), map(nullptr), effort_count(0), athlete_count(0),
			hazardous(false), star_count(0) {
			type_str.assign("Segment_t");
		};
		Segment_t(const string& json) : Segment_t() { ParseJson(json); ParseDom(); };
		Segment_t(rapidjson::Value & DOM) : Segment_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
	protected:
		void ParseDom();
	};

	class SegmentEffort_t : public Strava_t {
	public:
		ptrdiff_t id;
		ptrdiff_t resource_state;
		string name;
		ptrdiff_t activity;
		ptrdiff_t athlete;
		ptrdiff_t elapsed_time;
		ptrdiff_t moving_time;
		Times_t start_date;
		Times_t start_date_local;
		double distance;
		ptrdiff_t start_index;
		ptrdiff_t end_index;
		double average_cadence;
		bool device_watts;
		double average_heartrate;
		double max_heartrate;
		std::shared_ptr<Segment_t> segment;
		ptrdiff_t kom_rank;
		ptrdiff_t pr_rank;
		bool hidden;
		SegmentEffort_t() : id(0), resource_state(0), activity(0), athlete(0), elapsed_time(0),
			moving_time(0), distance(0.0), start_index(0), end_index(0), average_cadence(0.0),
			device_watts(false), average_heartrate(0.0), max_heartrate(0), segment(nullptr), kom_rank(0), pr_rank(0),
			hidden(false) {
			type_str.assign("SegmentEffort_t");
		};
		SegmentEffort_t(const string& json) : SegmentEffort_t() { ParseJson(json); ParseDom(); };
		SegmentEffort_t(rapidjson::Value& DOM) : SegmentEffort_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
	protected:
		void ParseDom();
	};

	class Effort_t : public Strava_t {
	public:
		ptrdiff_t id;
		ptrdiff_t resource_state;
		string	name;
		ptrdiff_t segment;
		ptrdiff_t activity;
		ptrdiff_t athlete;
		ptrdiff_t kom_rank;
		ptrdiff_t pr_rank;
		ptrdiff_t elapsed_time;
		ptrdiff_t moving_time;
		Times_t start_date;
		Times_t start_date_local;
		ptrdiff_t distance;
		std::list<Achievement_t> achievements;
		Effort_t() : id(0), resource_state(0), segment(0), activity(0), athlete(0),
			kom_rank(0), pr_rank(0), elapsed_time(0), moving_time(0), distance(0) {
			type_str.assign("Effort_t");
		};
		Effort_t(rapidjson::Value& DOM) : Effort_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
		Effort_t(const string& json) : Effort_t() { ParseJson(json); ParseDom(); };
	protected:
		void ParseDom();
	};

	class Splits_t : public Strava_t {
	public:
		double average_speed;
		double distance;
		ptrdiff_t	elapsed_time;
		double	elevation_difference;
		ptrdiff_t	pace_zone;
		ptrdiff_t	moving_time;
		ptrdiff_t	split;
		Splits_t() : average_speed(0.0), distance(0.0), elapsed_time(0), elevation_difference(0),
			pace_zone(0), moving_time(0), split(0) {
			type_str.assign("Splits_t");
		};
		Splits_t(const string& json) : Splits_t() { ParseJson(json); ParseDom(); };
		Splits_t(rapidjson::Value& DOM) : Splits_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
	protected:
		void ParseDom();
	};

	

	class Photo_t : public Strava_t {
	public:
		ptrdiff_t id;
		string unique_id;
		ptrdiff_t activity_id;
		ptrdiff_t resource_state;
		URL_t ref;
		string uid;
		std::map<string, URL_t> urls;
		string caption;
		string type;
		ptrdiff_t source;
		Times_t uploaded_at;
		Times_t created_at;
		Point_t location;
		Photo_t() : id(0), activity_id(0), resource_state(0), source(0) { type_str.assign("Photo_t"); };
		Photo_t(const string& json) : Photo_t() { ParseJson(json); ParseDom(); };
		Photo_t(rapidjson::Value& DOM) : Photo_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
	protected:
		void ParseDom();
	};

	class Lap_t : public Strava_t {
	public:
		ptrdiff_t activity;
		ptrdiff_t athlete;
		double average_cadence;
		double average_speed;
		ptrdiff_t elapsed_time;
		ptrdiff_t end_index;
		ptrdiff_t id;
		ptrdiff_t lap_index;
		double max_speed;
		ptrdiff_t moving_speed;
		string name;
		ptrdiff_t resource_state;
		ptrdiff_t split;
		Times_t start_date;
		Times_t start_date_local;
		ptrdiff_t start_index;
		double total_elevation_gain;
		Lap_t() : activity(0), athlete(0), average_cadence(0.0), average_speed(0.0),
			elapsed_time(0), end_index(0), id(0), lap_index(0), max_speed(0.0), moving_speed(0),
			resource_state(0), split(0), start_index(0), total_elevation_gain(0.0) {
			type_str.assign("Lap_t");
		};
		Lap_t(rapidjson::Value& DOM) : Lap_t() { dom->CopyFrom(DOM, document->GetAllocator());  ParseDom(); };
		Lap_t(const string& json) : Lap_t() { ParseJson(json); ParseDom(); };
	protected:
		void ParseDom();
	};
}//namespace RideWeather