add_executable(gameserver
    gameserver.cpp
)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(glog 0.4.0 REQUIRED)

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
    commandProcessing
    glog::glog
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS gameserver
    RUNTIME DESTINATION bin
)

