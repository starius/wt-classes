configure_file(${CMAKE_CURRENT_SOURCE_DIR}/locales-test.in
    ${CMAKE_CURRENT_BINARY_DIR}/locales-test @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/setup.py)

option(INSTALL_LOCALES_TEST "Install locales-test" ON)

find_program(rst2man NAMES rst2man)
if(EXISTS ${rst2man})
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/locales-test.1
        COMMAND ${rst2man} ${CMAKE_CURRENT_SOURCE_DIR}/locales-test.1.rst >
            ${CMAKE_CURRENT_BINARY_DIR}/locales-test.1
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/locales-test.1.rst)
    add_custom_target(mans ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/locales-test.1)
    set(MAN_SUBDIR ${CMAKE_INSTALL_PREFIX}/share/man/man1
        CACHE PATH "Man pages subdir")
    if(INSTALL_LOCALES_TEST)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/locales-test.1
            DESTINATION ${MAN_SUBDIR})
    endif(INSTALL_LOCALES_TEST)
endif(EXISTS ${rst2man})

if(INSTALL_LOCALES_TEST)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/locales-test DESTINATION bin)
endif()

