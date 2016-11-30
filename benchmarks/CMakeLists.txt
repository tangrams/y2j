set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "")
set(BENCHMARK_ENABLE_LTO OFF CACHE BOOL "")
add_subdirectory(google-benchmark)

include_directories(google-benchmark/include ${PROJECT_SOURCE_DIR})

add_executable(parse parse.cpp)

target_link_libraries(parse y2j benchmark)

add_custom_command(TARGET parse POST_BUILD COMMAND ${CMAKE_COMMAND} -E
  copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR})
