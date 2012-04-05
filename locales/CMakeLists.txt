set(LOCALES_DIR ${WT_DIR}/Wc/locales CACHE PATH "L9N XML installation dir")

install(DIRECTORY ./ DESTINATION ${LOCALES_DIR}
    FILES_MATCHING PATTERN "*.xml")

add_test(NAME wc-locales-test
    COMMAND ${PROJECT_SOURCE_DIR}/locales-test/locales-test.in
    --prefix=wc --sections captcha common time wbi
    --sources ${PROJECT_SOURCE_DIR}/src/*.cpp
    --locales ${PROJECT_SOURCE_DIR}/locales/*.xml
)

