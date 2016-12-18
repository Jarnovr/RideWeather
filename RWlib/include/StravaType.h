
#include <array>
#include <list>
#include <cstddef>
#include <exception>
using namespace std;

template <typename T>
class StravaException_t : public exception
{
	StravaException_t(const * char str) :exception(str) {};
};



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
enum class MeasurementType_t { feet, meters };

class Athlete_t {
public:
    enum class Connection_t {null, pending, accepted, blocked};
    enum class AthleteType_t : int {cyclist=0, runner=1};
    
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
    Connection_t friend_;//summary
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
    Athlete_t() :id(0), resource_state(0), firstname(NULL), lastname(NULL), profile(NULL),
        profile_medium(NULL),city(NULL),state(NULL),country(NULL),sex('\0'),
        friend_(Connection_t::null),follower(Connection_t::null),premium(false),
        created_at(NULL),updated_at(NULL),follower_count(0), friend_count(0),
        mutual_friend_count(0), athlete_type(AthleteType_t(0)),date_preference(NULL),
        measurement_preference(MeasurementType_t::meters),email(NULL),ftp(0),weight(0.0){;};
	Athlete_t(const char* json);
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

enum class ActivityType_t {
    Ride, Run, Swim, Hike, Walk, AlpineSki,
    BackcountrySki, Canoeing, Crossfit, EBikeRide, Elliptical, IceSkate,
    InlineSkate, Kayaking, Kitesurf, NordicSki, RockClimbing, RollerSki,
    Rowing, Snowboard, Snowshoe, StairStepper, StandUpPaddling, Surfing,
    VirtualRide, WeightTraining, Windsurf, Workout, Yoga, other
};

class Activity_t {
    enum class Workout_t {default_workout=0, race=1, longrun=2, workout=3, default_ride=10, race_ride=11, workout_ride=12};

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
    char *timezone_act;
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

class Club_t {
    enum ClubType_t {casual_club, racing_team, shop, company, club_other};
    enum Sport_t {cycling, runing, triathlon, sport_other};
    enum Membership_t {null, member, pending};
    ptrdiff_t id;
    ptrdiff_t resource_state;
    char *name;
    URL_t profile_medium;
    URL_t profile;
    URL_t cover_photo;
    URL_t cover_photo_small;
    char *description;
    ClubType_t club_type;
    Sport_t sport_type;
    char* city;//summary
    char* state;//summary
    char* country;//summary
    bool private_club;
    ptrdiff_t member_count;
    bool featured;
    bool verified;
    Membership_t membership;
    bool admin;
    bool owner;
    ptrdiff_t following_count;
    URL_t url;
};

class Gear_t {
    char *id;
    bool primary;
    char *name;
    double distance;
    char *description;
    ptrdiff_t resource_state;
};

class Bike_t : public Gear_t {
    char *brand_name;
    char *model_name;
    enum FrameType_t {mtb=1, cross=2, road3, timetrial=4};
    FrameType_t frame_type;
};

class Shoe_t : public Gear_t {
    

};

class Segment_t;
class Polyline_t {
    char *PolyString;
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
    char *name;
    char *description;
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
    char *name;
    enum RunningRace_t {road=0,trail=1,track=2,xc=3};
    RunningRace_t running_race_type;
    double distance;
    TimeS_t start_date_local;
    char *city;
    char *state;
    char *country;
    list<ptrdiff_t> route_ids;
    MeasurementType_t measurement_preference;
    URL_t url;
    URL_t website_url;
};

class Segments_t {
    ptrdiff_t id;
    ptrdiff_t resource_state;
    char *name;
    ActivityType_t activity_type;
    double distance;
    double average_grade;
    double maximum_grade;
    double elevation_high;
    double elevation_low;
    Point_t start_latlng;
    Point_t end_lating;
    ptrdiff_t climb_category;
    char* city;//summary
    char* state;//summary
    char* country;//summary
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
    char *name;
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

template <typename T>
class Stream_t {
    enum Resolution_t {low, medium, high};
    enum StreamType_t {time, latlng, distance, altitude, velocity_smooth,
        heartrate, cadence, watts, temp, moving, grade_smooth};
    char *type;
    list<T> data;
    char * series_type;
    ptrdiff_t original_size;
    Resolution_t resolution;
    StreamType_t stream_type;
};
