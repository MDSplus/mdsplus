
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
    PATTERN "*.in" EXCLUDE
    PATTERN "Makefile*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    PATTERN "docs" EXCLUDE
)