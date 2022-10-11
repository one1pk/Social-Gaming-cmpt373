add_executable(gameserver
    gameserver.cpp
)

set_target_properties(gameserver
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
                    PREFIX ""
)

target_link_libraries(gameserver
    game
    networking
    interpreter
)

install(TARGETS gameserver
    RUNTIME DESTINATION bin
)

