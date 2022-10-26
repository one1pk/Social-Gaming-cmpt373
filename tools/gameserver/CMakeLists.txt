add_executable(gameserver
    gameserver.cpp
)
add_definitions(-DPATH_TO_JSON="${CMAKE_CURRENT_SOURCE_DIR}/gameconfigs")

set_target_properties(gameserver
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
                    PREFIX ""
)

find_package(Threads REQUIRED)

target_link_libraries(gameserver
    game
    networking
    interpreter
    commandProcessing
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS gameserver
    RUNTIME DESTINATION bin
)

