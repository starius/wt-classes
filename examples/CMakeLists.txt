set(EXAMPLES_SUBDIR ${DOC_SUBDIR}/examples CACHE PATH "Examples docs subdir")
set(BINARY_EXAMPLES_SUBDIR ${CMAKE_INSTALL_PREFIX}/lib/libwtclasses/examples
    CACHE PATH "Binary examples docs subdir")
set(SWFSTORE_FILE /usr/share/javascript/yui/swfstore/swfstore.swf
    CACHE PATH "Path to swfstore.swf")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/swfstore.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/swfstore.cpp ESCAPE_QUOTES)

file(GLOB examples_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(REMOVE_ITEM examples_sources ${CMAKE_CURRENT_SOURCE_DIR}/all.cpp)
list(REMOVE_ITEM examples_sources ${CMAKE_CURRENT_SOURCE_DIR}/swfstore.cpp)

if(EXISTS ${SWFSTORE_FILE})
    set(HAVE_SWFSTORE 1)
    list(APPEND examples_sources ${CMAKE_CURRENT_BINARY_DIR}/swfstore.cpp)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/all.cpp.in.in
    ${CMAKE_CURRENT_BINARY_DIR}/all.cpp.in)

if(NOT WC_HAVE_WRASTERIMAGE)
    list(REMOVE_ITEM examples_sources ${CMAKE_CURRENT_SOURCE_DIR}/captcha.cpp)
endif()

if(NOT WC_HAVE_RECAPTCHA)
    list(REMOVE_ITEM examples_sources ${CMAKE_CURRENT_SOURCE_DIR}/recaptcha.cpp)
endif()

if(NOT WC_HAVE_GRAVATAR)
    list(REMOVE_ITEM examples_sources ${CMAKE_CURRENT_SOURCE_DIR}/gravatar.cpp)
endif()

if(NOT WC_HAVE_PAGER)
    list(REMOVE_ITEM examples_sources ${CMAKE_CURRENT_SOURCE_DIR}/pager.cpp)
endif()

include(FindPythonInterp)

list(REMOVE_DUPLICATES examples_sources)

if(PYTHONINTERP_FOUND)
    if(WC_HAVE_WRASTERIMAGE)
        set(wrasterimage "--wrasterimage")
    endif()
    list(SORT examples_sources)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/all.cpp
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/make-all.py
            --cpp \"${examples_sources}\"
            --template ${CMAKE_CURRENT_BINARY_DIR}/all.cpp.in
            ${wrasterimage}
            > ${CMAKE_CURRENT_BINARY_DIR}/all.cpp
        DEPENDS ${examples_sources}
    )
    list(APPEND examples_sources ${CMAKE_CURRENT_BINARY_DIR}/all.cpp)
endif(PYTHONINTERP_FOUND)

#set(EXAMPLES_CONNECTOR wthttp CACHE STRING "Wt connector used for examples")
option(EXAMPLES_STATIC_WC "Link examples to wt-classes statically" OFF)
option(INSTALL_EXAMPLES "Install examples" ON)

foreach(example_path ${examples_sources})
    include_directories(${local_include_dir})
    get_filename_component(example ${example_path} NAME_WE)
    add_executable(${example}.wt ${example_path} ${local_headers})
    target_link_libraries(${example}.wt ${EXAMPLES_CONNECTOR})
    if(EXAMPLES_STATIC_WC AND BUILD_STATIC_WC)
        target_link_libraries(${example}.wt wtclasses_static)
    else()
        target_link_libraries(${example}.wt wtclasses)
    endif()
    target_link_libraries(${example}.wt wt${WT_LIB_SUFFIX})
    if(INSTALL_EXAMPLES)
        install(FILES ${example_path} DESTINATION ${EXAMPLES_SUBDIR})
        install(TARGETS ${example}.wt DESTINATION ${BINARY_EXAMPLES_SUBDIR})
        install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${BINARY_EXAMPLES_SUBDIR}/${example}.wt
            $ENV{DESTDIR}${EXAMPLES_SUBDIR}/${example}.wt)")
    endif()
endforeach()

