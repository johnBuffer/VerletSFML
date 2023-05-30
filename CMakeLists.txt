cmake_minimum_required(VERSION 3.24)

project(VerletSFML)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2 COMPONENTS "graphics" "window" "system" REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE "sfml-graphics" "sfml-window" "sfml-system")
