cmake_minimum_required(VERSION 3.0)
project(RWlib)
find_package(RapidJSON)

file(GLOB CPP_FILES src/*.cpp)
file(GLOB H_FILES src/*.h)
file(GLOB HI_FILES include/*.h)

find_package(Boost REQUIRED filesystem system )

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR} )

add_library(RWlib ${HI_FILES} ${H_FILES} ${CPP_FILES})


add_executable(RWlib_Test test/ReadJson.cpp test/main.cpp)
target_link_libraries(RWlib_Test RWlib ${Boost_SYSTEM_LIBRARY_RELEASE} ${Boost_FILESYSTEM_LIBRARY_RELEASE})


set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
