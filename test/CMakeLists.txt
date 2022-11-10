add_subdirectory(gtest)
enable_testing()

add_definitions(-DPATH_TO_JSON_TEST="${CMAKE_CURRENT_SOURCE_DIR}/test_files")

add_executable(runAllTests
  test-interpreter.cpp
)
set_target_properties(runAllTests
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
                    PREFIX ""
                    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test
)

target_link_libraries(runAllTests
  gmock gtest gtest_main

  game
  networking
  interpreter
  ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME AllTests COMMAND runAllTests)

