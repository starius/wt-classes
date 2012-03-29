file(GLOB wtclasses_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB wtclasses_headers "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

add_library(wtclasses SHARED ${wtclasses_sources})
add_library(wtclasses_static STATIC ${wtclasses_sources})

target_link_libraries(wtclasses
    wt
    boost_filesystem boost_thread
    boost_signals boost_system
    ssl
)

set_target_properties(wtclasses PROPERTIES
    VERSION ${VERSION}
    SOVERSION ${SONAME}
)

if(NOT WIN32)
    set_target_properties(wtclasses_static PROPERTIES
        OUTPUT_NAME wtclasses)
endif()

install(TARGETS wtclasses DESTINATION lib)
install(TARGETS wtclasses_static DESTINATION lib)
install(FILES ${wtclasses_headers} DESTINATION include/Wt/Wc)

