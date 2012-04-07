configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

file(GLOB wtclasses_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB wtclasses_headers "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
list(APPEND wtclasses_headers ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

if(NOT HAVE_WRASTERIMAGE)
    list(REMOVE_ITEM wtclasses_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/PaintedCaptcha.cpp)
    list(REMOVE_ITEM wtclasses_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/PaintedCaptcha.hpp)
endif()

if(NOT HAVE_RECAPTCHA)
    list(REMOVE_ITEM wtclasses_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/Recaptcha.cpp)
    list(REMOVE_ITEM wtclasses_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/Recaptcha.hpp)
endif()

foreach(wtclasses_header ${wtclasses_headers})
    get_filename_component(name ${wtclasses_header} NAME)
    configure_file(${wtclasses_header} ${local_headers_dir}/${name})
endforeach()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(wtclasses SHARED ${wtclasses_sources})
add_library(wtclasses_static STATIC ${wtclasses_sources})

set(libs wt boost_filesystem boost_thread boost_signals boost_system ssl)

target_link_libraries(wtclasses ${libs})
target_link_libraries(wtclasses_static ${libs})

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

