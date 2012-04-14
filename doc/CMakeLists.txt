set(HTML_SUBDIR ${DOC_SUBDIR}/reference/html CACHE PATH "HTML docs subdir")

FIND_PACKAGE(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.in
        ${PROJECT_BINARY_DIR}/Doxyfile)
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/doc/html/index.html
        COMMAND ${DOXYGEN_EXECUTABLE}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${local_headers} ${PROJECT_BINARY_DIR}/Doxyfile
            ${PROJECT_SOURCE_DIR}/*.doc ${PROJECT_SOURCE_DIR}/examples/*.doc)
    add_custom_target(doc ALL DEPENDS ${PROJECT_BINARY_DIR}/doc/html/index.html)
    option(INSTALL_DOC "Install documentation" ON)
    if(INSTALL_DOC)
        install(DIRECTORY ${PROJECT_BINARY_DIR}/doc/html/
            DESTINATION ${HTML_SUBDIR})
    endif(INSTALL_DOC)
endif(DOXYGEN_FOUND)

