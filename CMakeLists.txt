cmake_minimum_required(VERSION 3.20)
project(Tanky)

find_package(raylib 4.0 REQUIRED)

set(CMAKE_C_STANDARD 11) # Requires C11 standard
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
