cmake_minimum_required(VERSION 3.1)
project(RWgui)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
set(CMAKE_PREFIX_PATH "D:/Qt/5.11.1/msvc2017_64/lib/cmake")
endif()

IF(WIN32)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT D:/boost_1_68_0)
set(BOOT_LIBRARYDIR D:/boost_1_68_0/stage/lib )
include_directories(${RapidJSON_SOURCE_DIR}/include)
ENDIF()
find_package(Boost REQUIRED filesystem date_time)

# Find the QtWidgets library
find_package(Qt5Widgets)

find_library(RWLIB_DEBUG RWlib-d PATHS ${LIBRARY_OUTPUT_PATH}/Debug)
find_library(RWLIB_RELEASE RWlib PATHS ${LIBRARY_OUTPUT_PATH}/Release)


file(GLOB CPP_FILES src/*.cpp)
file(GLOB H_FILES src/*.h)
file(GLOB HI_FILES include/*.h)
include_directories(${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS} ${RWlib_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

add_executable(RideWeather ${HI_FILES} ${H_FILES} ${CPP_FILES})
target_link_libraries(RideWeather ${CPR_LIBRARIES} debug ${RWLIB_DEBUG} optimized ${RWLIB_RELEASE} Qt5::Widgets debug  ${Boost_DATE_TIME_LIBRARY_DEBUG} debug ${Boost_FILESYSTEM_LIBRARY_DEBUG} debug ${Boost_SYSTEM_LIBRARY_DEBUG} optimized ${Boost_DATE_TIME_LIBRARY_RELEASE} optimized ${Boost_FILESYSTEM_LIBRARY_RELEASE} optimized ${Boost_SYSTEM_LIBRARY_RELEASE})


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
set(${PROJECT_NAME}_INCLUDE_DIRS ${dirs}
 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

  if (WIN32) 
  
add_custom_command(TARGET RideWeather POST_BUILD        # Adds a post-build event to MyTest
WORKING_DIRECTORY D:\\Qt\\5.11.1\\msvc2017_64\\bin
    COMMAND windeployqt --debug ${CMAKE_EXECUTABLE_OUTPUT_PATH}\\Debug   )
add_custom_command(TARGET RideWeather POST_BUILD        # Adds a post-build event to MyTest
WORKING_DIRECTORY D:\\Qt\\5.11.1\\msvc2017_64\\bin
    COMMAND windeployqt --release ${CMAKE_EXECUTABLE_OUTPUT_PATH}\\Release   )
	endif()