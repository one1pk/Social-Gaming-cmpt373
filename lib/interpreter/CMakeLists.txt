add_library(interpreter
    src/InterpretJson.cpp
)
find_package(glog 0.4.0 REQUIRED)

target_include_directories(interpreter
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(interpreter
        game  
        glog::glog
)

set_target_properties(interpreter
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
)

install(TARGETS interpreter
    ARCHIVE DESTINATION lib
)
