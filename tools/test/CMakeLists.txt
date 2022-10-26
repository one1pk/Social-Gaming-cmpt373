add_executable(test
    test.cpp
)

set_target_properties(test
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
                    PREFIX ""
)



target_link_libraries(test
    game
    networking
    interpreter
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS test
    RUNTIME DESTINATION bin
)
