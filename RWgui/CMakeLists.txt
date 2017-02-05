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

# Find the QtWidgets library
find_package(Qt5Widgets)

file(GLOB CPP_FILES src/*.cpp)
file(GLOB H_FILES src/*.h)
file(GLOB HI_FILES include/*.h)
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)

add_executable(RWgui ${HI_FILES} ${H_FILES} ${CPP_FILES})
target_link_libraries(RWgui RWlib Qt5::Widgets)


set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
 CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)


