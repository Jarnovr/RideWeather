#include <map>
#include <array>
#include <vector>
#include <list>
#include <cstddef>
#include <exception>
#include "rapidjson/document.h"
using namespace std;

class StravaException_t : public exception
{
public:
	StravaException_t(const string str) :exception(str.c_str()) {};
};

class Strava_t {
protected:
	rapidjson::Value dom;
	string type_str;
	void ParseString(string& dest, string var)
	{
		if (!dom.HasMember(var.c_str()))
			throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
		if (!dom[var.c_str()].IsString())
			throw StravaException_t(type_str.append(" ").append(var).append(" not a astring\n"));
		dest.assign(dom[var.c_str()].GetString(), dom[var.c_str()].GetStringLength());
	}
	int ParseInt(string var)
	{
		if (!dom.HasMember(var.c_str()))
			throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
		if (!dom[var.c_str()].IsInt())
			throw StravaException_t(type_str.append(" ").append(var).append(" not a int\n"));
		return dom[var.c_str()].GetInt();
	}
	ptrdiff_t ParseInt64(string var)
	{
		if (!dom.HasMember(var.c_str()))
			throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
		if (!dom[var.c_str()].IsInt64())
			throw StravaException_t(type_str.append(" ").append(var).append(" not a int64\n"));
		return dom[var.c_str()].GetInt64();
	}
	double ParseDouble(string var)
	{
		if (!dom.HasMember(var.c_str()))
			throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
		if (!dom[var.c_str()].IsDouble())
			throw StravaException_t(type_str.append(" ").append(var).append(" not a double\n"));
		return dom[var.c_str()].GetDouble();
	}
	bool ParseBool(string var)
	{
		if (!dom.HasMember(var.c_str()))
			throw StravaException_t(type_str.append(" missing ").append(var).append("\n"));
		if (!dom[var.c_str()].IsBool())
			throw StravaException_t(type_str.append(" ").append(var).append(" not a bool\n"));
		return dom[var.c_str()].GetBool();
	}


};



class Athlete_t;

class AccessToken_t {
public:
    char access_token[40];
    Athlete_t *athlete;
};


typedef string URL_t ;
typedef string TimeS_t;

class Club_t;

class Bike_t;
class Shoe_t;
enum class MeasurementType_t { feet, meters };

class Athlete_t: public Strava_t {
public:
    enum class Connection_t {null, pending, accepted, blocked};
    enum class AthleteType_t : int {cyclist=0, runner=1};
    
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
    TimeS_t created_at;//summary
    TimeS_t updated_at;//summary
    ptrdiff_t follower_count;//detailed only
    ptrdiff_t friend_count;//detailed only
    ptrdiff_t mutual_friend_count;//deiled only
    AthleteType_t athlete_type;//detailed only
    string date_preference;//format string//detailed only
    MeasurementType_t measurement_preference;//detailed only
    string email;//detailed only
    ptrdiff_t ftp;//detailed only
    double weight;//detailed only
    list<Club_t> clubs;//detailed only
    list<Bike_t> bikes;//detailed only
    list<Shoe_t> shoes;//detailed only
    Athlete_t() :id(0), resource_state(0), sex('\0'),
        friend_(Connection_t::null),follower(Connection_t::null),premium(false),
        follower_count(0), friend_count(0),
        mutual_friend_count(0), athlete_type(AthleteType_t(0)),
        measurement_preference(MeasurementType_t::meters),ftp(0),weight(0.0){;};
	Athlete_t(const string& json);
	virtual ~Athlete_t() {};
protected:
    
};

class Zones_t: public Strava_t{
public:
    bool heart_rate_custom;
    struct zone_t { short min; short max; };
    vector<zone_t> heart_zones;
    vector<zone_t> power_zones;
	Zones_t() : heart_rate_custom(false) {};
	Zones_t(const string& json);
};

class Total_t: public Strava_t {
public:
    ptrdiff_t count;
    double distance;
    ptrdiff_t moving_time;
    ptrdiff_t elapsed_time;
    double elevation_gain;
    ptrdiff_t achievement_count;
	Total_t() : count(0), distance(0.0), moving_time(0), elapsed_time(0),
		elevation_gain(0.0), achievement_count(0) {};
	Total_t(const string& json);
	Total_t(rapidjson::Value& DOM);
protected:
	void ParseDom();
};

class Totals_t:public Strava_t{
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
	Totals_t() : biggest_ride_distance(0.0), biggest_climb_elevation_gain(0.0) {};
	Totals_t(const string& json);
};

class Point_t {
public:
    double latitude;
    double longtitude;
	Point_t() : latitude(0.0), longtitude(0.0) {};
	Point_t(const double lat, const double lon) : latitude(lat), longtitude(lon) {};
};

class Photo_t;
class Map_t;
class Gear_t;
class SegmentEffort_t;
class Splits_t;
class Effort_t;

enum class ActivityType_t {
    Ride, Run, Swim, Hike, Walk, AlpineSki,
    BackcountrySki, Canoeing, Crossfit, EBikeRide, Elliptical, IceSkate,
    InlineSkate, Kayaking, Kitesurf, NordicSki, RockClimbing, RollerSki,
    Rowing, Snowboard, Snowshoe, StairStepper, StandUpPaddling, Surfing,
    VirtualRide, WeightTraining, Windsurf, Workout, Yoga, other
};

class Activity_t:public Strava_t {
public:
    enum class Workout_t {default_workout=0, race=1, longrun=2, workout=3, default_ride=10, race_ride=11, workout_ride=12};
    ptrdiff_t id;
    ptrdiff_t resource_state;
    string external_id;
    ptrdiff_t upload_id;
    Athlete_t *athlete;
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
    TimeS_t start_date;
    TimeS_t start_date_local;
    string timezone_act;
    Point_t start_latlng;
    Point_t end_latlng;
    ptrdiff_t achievement_count;
    ptrdiff_t kudos_count;
    ptrdiff_t comment_count;
    ptrdiff_t athlete_count;
    ptrdiff_t photo_count;
    ptrdiff_t total_photo_count;
    list<Photo_t> photos;
    Map_t *map;
    bool trainer;
    bool commute;
    bool manual;
    bool private_act;
    string device_name;
    string embed_token;
    bool flagged;
    Workout_t workout_type;
    string gear_id;
    Gear_t *gear;
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
    ptrdiff_t max_heartrate;
    double calories; //kilocalories, kJ (ride), pace(runs)
    ptrdiff_t suffer_score;
    bool has_kudoed;
    list<SegmentEffort_t> segment_efforts;
    list<Splits_t> splits_metric;
    list<Splits_t> splits_standard;
    list<Effort_t> best_efforts;
	Activity_t() : id(0), resource_state(0), upload_id(0), athlete(NULL), distance(0.0),
		moving_time(0), elapsed_time(0), total_elevation_gain(0.0), elev_high(0.0), elev_low(0.0),
		type(ActivityType_t::other), start_latlng(0.0, 0.0), end_latlng(0.0, 0.0),
		achievement_count(0), kudos_count(0), comment_count(0), athlete_count(0), photo_count(0),
		total_photo_count(0), map(NULL), trainer(false), commute(false), manual(false),
		private_act(false), flagged(false), workout_type(Workout_t::default_workout),
		gear(NULL), average_speed(0.0), max_speed(0.0), average_cadence(0.0),
		average_temp(0.0), average_watts(0.0), max_watts(0), weighted_avereage_watts(0),
		kilojoules(0.0), device_watts(false), has_heartrate(false), average_heartrate(0.0),
		max_heartrate(0), calories(0.0), suffer_score(0), has_kudoed(0) {};
	Activity_t(const string& json);

};

class Achievement_t {
    enum AchievementType_t {overall=2, pr=3, year_overall=5};
    AchievementType_t type_id;
    string  type;
    ptrdiff_t rank;
};

class Club_t: public Strava_t {
public:
    enum ClubType_t {casual_club, racing_team, shop, company, club_other};
    enum Sport_t {cycling, runing, triathlon, sport_other};
    enum Membership_t {null, member, pending};
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
	Club_t() : id(0), resource_state(0), club_type(ClubType_t::club_other), sport_type(sport_other),
		private_club(false), member_count(0), featured(false), verified(false),
		membership(Membership_t::member), admin(false), owner(false), following_count(0) {};
	Club_t(rapidjson::Value& DOM) :Club_t() { ; };
};

class Gear_t: Strava_t {
public:
	string id;
    bool primary;
    string name;
    double distance;
    string description;
    ptrdiff_t resource_state;
	Gear_t() : primary(false), distance(0.0), resource_state(0) {};
	Gear_t(rapidjson::Value& DOM) :Gear_t() { ; };
};

class Bike_t : public Gear_t {
public:
    string brand_name;
    string model_name;
    enum FrameType_t {mtb=1, cross=2, road3, timetrial=4};
    FrameType_t frame_type;
	Bike_t() : Gear_t(), frame_type(mtb) {};
	Bike_t(rapidjson::Value& DOM) : Gear_t(DOM) {};
};

class Shoe_t : public Gear_t {
public:
	Shoe_t() : Gear_t() {};
	Shoe_t(rapidjson::Value& DOM): Gear_t(DOM) {};

};

class Segment_t;
class Polyline_t {
    string PolyString;
};

class Map_t {
    ptrdiff_t id;
    Polyline_t summary_polyline;
    ptrdiff_t resource_state;
    Polyline_t polyline;
};

class Route_t {
    ptrdiff_t id;
    ptrdiff_t resource_state;
    string name;
    string description;
    Athlete_t athlete;
    double distance;//meters
    double elevation_gain;//meters
    Map_t map;
    enum Type_t {ride=1, run=2};
    Type_t type;
    enum SubType_t { road=1, mtb=2, cx=3, trail=4,mixed=5};
    SubType_t sub_type;
    bool private_route;
    bool starred;
    ptrdiff_t timestap;//unix timestap
    list<Segment_t> segments;
};


class Race_t {
    ptrdiff_t id;
    ptrdiff_t resource_state;
    string name;
    enum RunningRace_t {road=0,trail=1,track=2,xc=3};
    RunningRace_t running_race_type;
    double distance;
    TimeS_t start_date_local;
    string city;
    string state;
    string country;
    list<ptrdiff_t> route_ids;
    MeasurementType_t measurement_preference;
    URL_t url;
    URL_t website_url;
};

class Segments_t {
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
    Point_t end_lating;
    ptrdiff_t climb_category;
    string city;//summary
    string state;//summary
    string country;//summary
    bool private_segment;
    bool starred;
    TimeS_t created_at;
    TimeS_t updated_at;
    double total_elevation_gain;
    Map_t map;
    ptrdiff_t effort_count;
    ptrdiff_t athlete_count;
    bool hazardous;
    ptrdiff_t star_count;
};

class SegmentEffort_t {
    ptrdiff_t id;
    ptrdiff_t resource_state;
    string name;
    Activity_t *activity;
    Athlete_t *athlete;
    ptrdiff_t elapsed_time;
    ptrdiff_t moving_time;
    TimeS_t start_date;
    TimeS_t start_date_local;
    double distance;
    ptrdiff_t start_index;
    ptrdiff_t end_index;
    double average_cadence;
    bool device_watts;
    double average_heartrate;
    ptrdiff_t max_heartrate;
    Segment_t *segment;
    ptrdiff_t kom_rank;
    ptrdiff_t pr_rank;
    bool hidden;
};

class Effort_t : public Strava_t {
public:
	ptrdiff_t id;
	ptrdiff_t resource_state;
	string	name;
	Segment_t *segment;
	Activity_t *activity;
	Athlete_t *athlete;
	ptrdiff_t kom_rank;
	ptrdiff_t pr_rank;
	ptrdiff_t elapsed_time;
	ptrdiff_t moving_time;
	TimeS_t start_date;
	TimeS_t start_date_local;
	ptrdiff_t distance;
	list<Achievement_t> achievements;
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
};

template <typename T>
class Stream_t {
    enum Resolution_t {low, medium, high};
    enum StreamType_t {time, latlng, distance, altitude, velocity_smooth,
        heartrate, cadence, watts, temp, moving, grade_smooth};
    string type;
    list<T> data;
    string  series_type;
    ptrdiff_t original_size;
    Resolution_t resolution;
    StreamType_t stream_type;
};

class Photo_t : public Strava_t {
public:
	ptrdiff_t id;
	string unique_id;
	ptrdiff_t activity_id;
	ptrdiff_t resource_state;
	URL_t ref;
	string uid;
	map<ptrdiff_t, URL_t> urls;
	string caption;
	string type;
	ptrdiff_t source;
	TimeS_t uploaded_at;
	TimeS_t created_at;
	Point_t location;
};