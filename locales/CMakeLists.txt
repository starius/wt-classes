set(LOCALES_DIR ${WT_DIR}/Wc/locales CACHE PATH "L9N XML installation dir")

install(DIRECTORY ./ DESTINATION ${LOCALES_DIR}
    FILES_MATCHING PATTERN "*.xml")

