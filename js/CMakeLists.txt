file(DOWNLOAD http://keith-wood.name/js/jquery.countdown.js
    ${CMAKE_CURRENT_BINARY_DIR}/jquery.countdown.js)

install(DIRECTORY ./ DESTINATION ${WT_DIR}/resources/Wc/js
    FILES_MATCHING PATTERN "*.js")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/jquery.countdown.js
    DESTINATION ${WT_DIR}/resources/Wc/js)

