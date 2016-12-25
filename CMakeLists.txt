cmake_minimum_required (VERSION 3.0)
project (RideWeather)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(CMAKE_BUILD_TYPE Debug)

if(LINUX)
set(CMAKE_CXX_FLAGS_RELEASE  "-O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG  "-Og -g -march=native -Wno-error")
set(CMAKE_C_FLAGS_RELEASE  "-O3 -march=native")
set(CMAKE_C_FLAGS_DEBUG  "-Og -g -march=native -Wno-error")
endif()
 
 
#dependencies
add_subdirectory(rapidjson)

##main project
add_subdirectory(RWlib)

add_subdirectory(RWcgi)
add_subdirectory(RWd)
add_subdirectory(RWgui)

