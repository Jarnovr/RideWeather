cmake_minimum_required(VERSION 3.0)
project(RWcgi)

find_library(CGICC cgicc)

file(GLOB CPP_FILES src/*.cpp)
file(GLOB H_FILES src/*.h)
file(GLOB HI_FILES include/*.h)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
add_executable(RWcgi ${CPP_FILES} ${H_FILES})
target_link_libraries(RWcgi ${CGICC})