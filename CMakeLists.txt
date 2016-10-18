cmake_minimum_required (VERSION 3.1.3)

project (y2j)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(libyaml)

include_directories(libyaml/include rapidjson)

add_library(y2j y2j.cpp)

target_link_libraries(y2j yaml)

add_executable(parse-test parse-test.cpp)

target_link_libraries(parse-test y2j)
