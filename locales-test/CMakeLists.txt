configure_file(${CMAKE_CURRENT_SOURCE_DIR}/locales-test.in
    ${CMAKE_CURRENT_BINARY_DIR}/locales-test @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/setup.py)

find_program(rst2man NAMES rst2man)
if(${rst2man})
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/locales-test.1
        COMMAND ${rst2man} ${CMAKE_CURRENT_SOURCE_DIR}/locales-test.1.rst
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/locales-test.1.rst)
    set(MAN_SUBDIR ${CMAKE_INSTALL_PREFIX}/share/man/man1
        CACHE PATH "Man pages subdir")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/locales-test.1
        DESTINATION ${MAN_SUBDIR})
endif()

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/locales-test DESTINATION bin)

