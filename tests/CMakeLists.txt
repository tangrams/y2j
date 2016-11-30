include_directories(${PROJECT_SOURCE_DIR})

add_executable(all_tests
  main.cpp
  numbers.cpp
  complex.cpp
  scalars.cpp
)

target_link_libraries(all_tests y2j)
