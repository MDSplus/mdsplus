
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/_version.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/_version.py
)

# TODO: Separate out executable files from not?
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
        PATTERN "*.xml"

    PATTERN "docs" EXCLUDE
    PATTERN "tests" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
)