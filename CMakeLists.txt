cmake_minimum_required(VERSION 3.12)

project(VerletSFML VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2 COMPONENTS "graphics" "window" "system" REQUIRED)

add_executable(${PROJECT_NAME} "src/main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE "sfml-graphics" "sfml-window" "sfml-system")
