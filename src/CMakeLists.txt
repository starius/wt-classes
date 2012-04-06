configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

file(GLOB wtclasses_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB wtclasses_headers "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
list(APPEND wtclasses_headers ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

foreach(wtclasses_header ${wtclasses_headers})
    get_filename_component(name ${wtclasses_header} NAME)
    configure_file(${wtclasses_header} ${local_headers_dir}/${name})
endforeach()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

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

