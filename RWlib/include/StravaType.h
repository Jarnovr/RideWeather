
#include <array>
#include <list>
#include <cstddef>
using namespace std;

class Athlete_t;

class AccessToken_t {
public:
	char access_token[40];
	Athlete_t *athlete;

};

typedef char* URL_t ;
typedef char* TimeS_t;

class Club_t;

class Bike_t;
class Shoe_t;

class Athlete_t {
public:
	enum class Connection_t {null, pending, accepted, blocked};
	enum class AthleteType_t : int {cyclist=0, runner=1};
	enum class MeasurementType_t {feet, meters};

	ptrdiff_t id;//summary
	ptrdiff_t resource_state;//summary
	char* firstname;//summary
	char* lastname;//summary
	URL_t profile_medium;//summary
	URL_t profile;//summary
	char* city;//summary
	char* state;//summary
	char* country;//summary
	char sex;//summary
	Connection_t friend;//summary
	Connection_t follower;//summary
	bool premium;//summary
	TimeS_t created_at;//summary
	TimeS_t updated_at;//summary
	ptrdiff_t follower_count;//detailed only
	ptrdiff_t friend_count;//detailed only
	ptrdiff_t mutual_friend_count;//detailed only
	AthleteType_t athlete_type;//detailed only
	char* date_preference;//format string//detailed only
	MeasurementType_t measurement_preference;//detailed only
	char* email;//detailed only
	ptrdiff_t ftp;//detailed only
	double weight;//detailed only
	list<Club_t> clubs;//detailed only
	list<Bike_t> bikes;//detailed only
	list<Shoe_t> shoes;//detailed only
	virtual ~Athlete_t();
protected:
	
};

class Zones_t {
	bool heart_rate_custom;
	struct zone_t { short min; short max; };
	array<zone_t, 5> *heart_zones;
	array<zone_t, 5> *power_zones;
};

class Total_t {
	ptrdiff_t count;
	double distance;
	ptrdiff_t moving_time;
	ptrdiff_t elapsed_time;
	double elevation_gain;
	ptrdiff_t achievement_count;
};

class Totals_t{
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
};

class Point_t {
	double latitude;
	double longtitude;
};

class Photo_t;
class Map_t;
class Gear_t;
class SegmentEffort_t;
class Splits_t;
class Efforts_t;


class Activity_t {
	enum class ActivityType_t {Ride, Run, Swim, Hike, Walk, AlpineSki,
		BackcountrySki, Canoeing, Crossfit, EBikeRide, Elliptical, IceSkate,
		InlineSkate, Kayaking, Kitesurf, NordicSki, RockClimbing, RollerSki,
		Rowing, Snowboard, Snowshoe, StairStepper, StandUpPaddling, Surfing,
		VirtualRide, WeightTraining, Windsurf, Workout, Yoga, other};
	enum class Workout_t {default=0, race=1, longrun=2, workout=3, default_ride=10, race_ride=11, workout_ride=12};

	ptrdiff_t id;
	ptrdiff_t resource_state;
	char *external_id;
	ptrdiff_t upload_id;
	Athlete_t *athlete;
	char *name;
	char *description;
	double distance;//meters
	ptrdiff_t moving_time;//seconds
	ptrdiff_t elapsed_time;//seconds
	double total_elevation_gain;//meters
	double elev_high;
	double elev_low;
	ActivityType_t type;
	char *typeS;
	TimeS_t start_date;
	TimeS_t start_date_local;
	char *timezone;
	Point_t start_latlng;
	Point_t end_lating;
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
	char *device_name;
	char *embed_token;
	bool flagged;
	Workout_t workout_type;
	char *gear_id;
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
	list<Efforts_t> best_efforts;


};

class Achievement_t {
	enum AchievementType_t {overall=2, pr=3, year_overall=5};
	AchievementType_t type_id;
	char * type;
	ptrdiff_t rank;
};