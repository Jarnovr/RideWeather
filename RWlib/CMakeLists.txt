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
set(BOOST_ROOT C:/boost_1_62_0/)
set(BOOT_LIBRARYDIR C:/boost_1_62_0/lib )
include_directories(${RapidJSON_SOURCE_DIR}/include)
ENDIF()
find_package(Boost REQUIRED filesystem system )

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR} )

add_library(RWlib ${HI_FILES} ${H_FILES} ${CPP_FILES})


add_executable(RWlib_Test test/ReadJson.cpp test/main.cpp)
target_link_libraries(RWlib_Test RWlib ${Boost_SYSTEM_LIBRARY_DEBUG} ${Boost_FILESYSTEM_LIBRARY_DEBUG})


set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
