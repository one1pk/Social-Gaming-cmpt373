add_subdirectory(gtest)
enable_testing()

add_definitions(-DPATH_TO_JSON_TEST="${CMAKE_CURRENT_SOURCE_DIR}/test.json")

add_executable(runAllTests
  test-interpreter.cpp
)

target_link_libraries(runAllTests
  gmock gtest gtest_main

  game
  networking
  interpreter
  ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME AllTests COMMAND runAllTests)

