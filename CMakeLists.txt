cmake_minimum_required (VERSION 3.0)
project (RideWeather)

# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_COLOR_MAKEFILE   ON)


add_subdirectory(RWlib)

add_subdirectory(RWcgi)
add_subdirectory(RWd)
add_subdirectory(RWgui)

