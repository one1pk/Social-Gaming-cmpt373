add_executable(gameclient
    gameclient.cpp
)

set_target_properties(gameclient
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
                    PREFIX ""
)

target_link_libraries(gameclient
    game
    networking
)

install(TARGETS gameclient
    RUNTIME DESTINATION bin
)

