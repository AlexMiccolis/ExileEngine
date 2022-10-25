cmake_minimum_required(VERSION 3.23)
project(ExileEngine)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Source)

enable_testing()
add_subdirectory(Tests)
