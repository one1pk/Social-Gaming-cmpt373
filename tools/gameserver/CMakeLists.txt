add_executable(gameserver
    gameserver.cpp
)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_definitions(-DPATH_TO_JSON="${CMAKE_CURRENT_SOURCE_DIR}/gameconfigs")

set_target_properties(gameserver
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
                    PREFIX ""
)

find_package(Threads REQUIRED)

target_link_libraries(gameserver
PRIVATE
    game
    networking
    interpreter
<<<<<<< HEAD
    commandProcessing
=======
    nlohmann_json::nlohmann_json
>>>>>>> b36ec27 (Edit cmake)
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS gameserver
    RUNTIME DESTINATION bin
)

