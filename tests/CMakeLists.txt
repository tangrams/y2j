include_directories(${PROJECT_SOURCE_DIR})

add_executable(all_tests
  aliases.cpp
  complex.cpp
  errors.cpp
  main.cpp
  numbers.cpp
  scalars.cpp
  tags.cpp
)

target_link_libraries(all_tests y2j)
