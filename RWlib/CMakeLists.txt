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
set(BOOT_LIBRARYDIR D:/boost_1_63_0/stage/lib )
set(RAPIDJSON_BUILD_DOC OFF)
set(RAPIDJSON_BUILD_EXAMPLES OFF)
set(RAPIDJSON_BUILD_TESTS OFF)
set(RAPIDJSON_HAS_STDSTRING ON)
include_directories(${RapidJSON_SOURCE_DIR}/include)
ENDIF()
find_package(Boost REQUIRED filesystem date_time )

include_directories(${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS} ${CPR_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR} )

add_library(RWlib ${HI_FILES} ${H_FILES} ${CPP_FILES})
target_link_libraries(RWlib ${CPR_LIBRARIES}  debug  ${Boost_DATE_TIME_LIBRARY_DEBUG} debug ${Boost_FILESYSTEM_LIBRARY_DEBUG} debug  ${Boost_SYSTEM_LIBRARY_DEBUG} optimized ${Boost_DATE_TIME_LIBRARY_RELEASE} optimized ${Boost_FILESYSTEM_LIBRARY_RELEASE} optimized ${Boost_SYSTEM_LIBRARY_RELEASE})

add_executable(RWlib_Test test/ReadJson.cpp test/main.cpp test/ReadJson.h)
target_link_libraries(RWlib_Test RWlib)


GET_DIRECTORY_PROPERTY(dirs INCLUDE_DIRECTORIES)
set(${PROJECT_NAME}_INCLUDE_DIRS ${dirs}
 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

  if (WIN32)
  
add_custom_command(TARGET RWlib POST_BUILD        # Adds a post-build event to MyTest
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/lib/Debug
    COMMAND copy * "..\\..\\bin\\Debug"   )

	add_custom_command(TARGET RWlib POST_BUILD   
WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/lib/Release"	
	COMMAND copy * "..\\..\\bin\\Release"    )
	
endif()

		
