cmake_minimum_required(VERSION 3.0)
project(RWcgi)

if(WIN32)
set(CGICC C:/Users/jarno_000/Documents/Build/cgicc-3.2.16/win/x64/Release/cgicc.lib)
include_directories(C:/Users/jarno_000/Documents/Build/cgicc-3.2.16/)
else()
find_library(CGICC cgicc)
endif()

file(GLOB CPP_FILES src/*.cpp)
file(GLOB H_FILES src/*.h)
file(GLOB HI_FILES include/*.h)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${CGICC})
add_executable(RWcgi ${CPP_FILES} ${H_FILES})
target_link_libraries(RWcgi ${CGICC})