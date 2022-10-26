add_subdirectory(gtest)
enable_testing()

add_executable(runAllTests
  testgameserver.cpp
)

target_link_libraries(runAllTests
  gmock gtest gtest_main
  simple
)

add_test(NAME AllTests COMMAND runAllTests)

