cmake_minimum_required(VERSION 3.19)
project(Assign_6)
# Project sources
add_library(${PROJECT_NAME}
        SDLViewer.cpp
        SDLViewer.h
        )
#set(CMAKE_CXX_STANDARD 14)


# Directory to external libraries used in the project
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ext/)

# Use C++11 version of the standard
 set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES)

# Include Eigen for linear algebra, stb and gif-h for exporting images
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${THIRD_PARTY_DIR}/eigen" "${THIRD_PARTY_DIR}/stb" "${THIRD_PARTY_DIR}/gif-h")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries(Assign_6 ${SDL2_LIBRARIES})

add_executable(RasterViewer raster.cpp RasterViewer.cpp)
target_link_libraries(RasterViewer PUBLIC ${PROJECT_NAME})

