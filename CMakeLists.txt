cmake_minimum_required (VERSION 3.13)
project (RideWeather)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

if(LINUX)
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -Og -g -march=native -Wno-error -Wall")
set(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} -O3 -march=native")
set(CMAKE_C_FLAGS_DEBUG  "${CMAKE_C_FLAGS_DEBUG} -Og -g -march=native -Wno-error -Wall")
endif()

if(WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /arch:AVX2 /Ox /W4" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /arch:AVX2  /W4" )
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#dependencies
# set(RAPIDJSON_BUILD_DOC OFF)
# set(RAPIDJSON_BUILD_EXAMPLES OFF)
# set(RAPIDJSON_BUILD_TESTS OFF)
# set(RAPIDJSON_HAS_STDSTRING ON)
# add_definitions(-DRAPIDJSON_HAS_STDSTRING) 
# add_subdirectory(rapidjson)
set(BUILD_CPR_TESTS OFF )
set(BUILD_CURL_TESTS OFF )
set(BUILD_TESTING OFF )
set(CPR_USE_SYSTEM_GTEST ON )
add_subdirectory(cpr)

##main project
add_subdirectory(RWlib)

#add_subdirectory(RWcgi)
add_subdirectory(RWd)
add_subdirectory(RWgui)

