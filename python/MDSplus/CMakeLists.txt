
###
### Generate _version.py
###

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/_version.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/_version.py
)

install(
    DIRECTORY .
    DESTINATION python/MDSplus
    FILE_PERMISSIONS ${MODE_755}
    FILES_MATCHING
        PATTERN "*.py"
        PATTERN "*.conf"
        PATTERN "*.wsgi"
        PATTERN "*.html"
        PATTERN "*.js"
        PATTERN "*.tbl"
        PATTERN "*.toml"
)