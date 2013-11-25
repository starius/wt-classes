configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

file(GLOB wtclasses_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB wtclasses_headers "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
list(APPEND wtclasses_headers ${CMAKE_CURRENT_BINARY_DIR}/config.hpp)

if(NOT WC_HAVE_WRASTERIMAGE)
    list(REMOVE_ITEM wtclasses_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/PaintedCaptcha.cpp)
    list(REMOVE_ITEM wtclasses_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/PaintedCaptcha.hpp)
endif()

if(NOT WC_HAVE_RECAPTCHA)
    list(REMOVE_ITEM wtclasses_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/Recaptcha.cpp)
    list(REMOVE_ITEM wtclasses_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/Recaptcha.hpp)
endif()

if(NOT WC_HAVE_SQLTIME)
    list(REMOVE_ITEM wtclasses_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/TimeDurationDbo.cpp)
    list(REMOVE_ITEM wtclasses_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/TimeDurationDbo.hpp)
endif()

if(NOT WC_HAVE_GRAVATAR)
    list(REMOVE_ITEM wtclasses_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/GravatarImage.cpp)
    list(REMOVE_ITEM wtclasses_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/GravatarImage.hpp)
endif()

if(NOT WC_HAVE_PAGER)
    list(REMOVE_ITEM wtclasses_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/Pager.cpp)
    list(REMOVE_ITEM wtclasses_headers
        ${CMAKE_CURRENT_SOURCE_DIR}/Pager.hpp)
endif()

foreach(wtclasses_header ${wtclasses_headers})
    get_filename_component(name ${wtclasses_header} NAME)
    configure_file(${wtclasses_header} ${local_headers_dir}/${name})
endforeach()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(libs wt ${Boost_LIBRARIES})

if(WC_USE_OPENSSL)
    set(libs ${libs} ${OPENSSL_LIBRARIES})
endif()

option(BUILD_STATIC_WC "Build static version of the library as well" ON)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_library(wtclasses SHARED ${wtclasses_sources})
target_link_libraries(wtclasses ${libs})
if(BUILD_STATIC_WC)
    add_library(wtclasses_static STATIC ${wtclasses_sources})
    target_link_libraries(wtclasses_static ${libs})
endif()

set_target_properties(wtclasses PROPERTIES
    VERSION ${VERSION}
    SOVERSION ${SONAME}
)

if(NOT WIN32 AND BUILD_STATIC_WC)
    set_target_properties(wtclasses_static PROPERTIES
        OUTPUT_NAME wtclasses)
endif()

install(TARGETS wtclasses DESTINATION lib)
if(BUILD_STATIC_WC)
    install(TARGETS wtclasses_static DESTINATION lib)
endif()
install(FILES ${wtclasses_headers} DESTINATION include/Wt/Wc)

