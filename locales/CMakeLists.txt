set(LOCALES_DIR ${WT_DIR}/Wc/locales CACHE PATH "L9N XML installation dir")

install(DIRECTORY ./ DESTINATION ${LOCALES_DIR}
    FILES_MATCHING PATTERN "*.xml")

include(FindPythonInterp)

if(PYTHONINTERP_FOUND)
    execute_process(COMMAND ${PYTHON_EXECUTABLE}
        ${PROJECT_SOURCE_DIR}/locales-test/locales-test.in -h
        OUTPUT_QUIET ERROR_QUIET
        RESULT_VARIABLE exit_status)
    if(${exit_status} EQUAL 0)
        add_test(NAME wc-locales-test
            COMMAND ${PROJECT_SOURCE_DIR}/locales-test/locales-test.in
            --prefix=wc --sections captcha common time wbi
            --sources ${PROJECT_SOURCE_DIR}/src/*.cpp
            --locales ${PROJECT_SOURCE_DIR}/locales/*.xml
        )
    endif(${exit_status} EQUAL 0)
endif(PYTHONINTERP_FOUND)

