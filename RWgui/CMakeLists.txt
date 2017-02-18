cmake_minimum_required(VERSION 3.1)
project(RWgui)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
set(CMAKE_PREFIX_PATH "D:/Qt/5.8/msvc2015_64/lib/cmake")
endif()

IF(WIN32)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(BOOST_ROOT D:/boost_1_63_0)
set(BOOT_LIBRARYDIR D:/boost_1_63_0/lib )
include_directories(${RapidJSON_SOURCE_DIR}/include)
ENDIF()
find_package(Boost REQUIRED filesystem)

# Find the QtWidgets library
find_package(Qt5Widgets)

file(GLOB CPP_FILES src/*.cpp)
file(GLOB H_FILES src/*.h)
file(GLOB HI_FILES include/*.h)
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

add_executable(RideWeather ${HI_FILES} ${H_FILES} ${CPP_FILES})
target_link_libraries(RideWeather  ${Boost_FILESYSTEM_LIBRARY_DEBUG} ${Boost_SYSTEM_LIBRARY_DEBUG} RWlib Qt5::Widgets)


set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)


