
###
### MDSplus/_version.py
###

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/MDSplus/_version.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/MDSplus/_version.py
)

install(
    DIRECTORY MDSplus
    DESTINATION python
    USE_SOURCE_PERMISSIONS
    PATTERN "*.in" EXCLUDE
    PATTERN "Makefile*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    # We install the _case.py files, but not the individual _test.py ones
    PATTERN "tests/*_test.py" EXCLUDE
    PATTERN "docs" EXCLUDE
)
