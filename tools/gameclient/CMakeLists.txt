add_executable(gameclient
    gameclient.cpp
    ChatWindow.cpp
)

set_target_properties(gameclient
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
                    PREFIX ""
)

find_package(Curses REQUIRED)
find_package(Threads REQUIRED)
find_package(glog 0.4.0 REQUIRED)

target_link_libraries(gameclient
    networking
    ${CURSES_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    glog::glog
)

install(TARGETS gameclient
    RUNTIME DESTINATION bin
)