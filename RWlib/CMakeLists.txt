cmake_minimum_required(VERSION 3.0)
project(RWlib)
find_package(RapidJSON)

file(GLOB CPP_FILES src/*.cpp)
file(GLOB H_FILES src/*.h)
file(GLOB HI_FILES include/*.h)

IF(WIN32)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT D:/boost_1_63_0)
set(BOOT_LIBRARYDIR D:/boost_1_63_0/lib )
include_directories(${RapidJSON_SOURCE_DIR}/include)
ENDIF()
find_package(Boost REQUIRED filesystem system date_time )

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS} ${CPR_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR} )

add_library(RWlib ${HI_FILES} ${H_FILES} ${CPP_FILES})
target_link_libraries(RWlib ${CPR_LIBRARIES} ${Boost_DATE_TIME_LIBRARY_DEBUG})

add_executable(RWlib_Test test/ReadJson.cpp test/main.cpp test/ReadJson.h)
target_link_libraries(RWlib_Test RWlib)


set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
