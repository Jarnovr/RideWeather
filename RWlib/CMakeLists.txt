cmake_minimum_required(VERSION 3.0)
project(RWlib)

file(GLOB CPP_FILES src/*.cpp)
file(GLOB H_FILES src/*.h)
file(GLOB HI_FILES include/*.h)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
add_library(RWlib ${HI_FILES} ${H_FILES} ${CPP_FILES})


set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
