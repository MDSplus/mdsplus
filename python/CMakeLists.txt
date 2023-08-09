
add_subdirectory(MDSplus)

install(
    DIRECTORY .
    DESTINATION python
    FILE_PERMISSIONS ${MODE_755}
    PATTERN "*.in" EXCLUDE
    PATTERN "Makefile*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    # We install the _case.py files, but not the individual _test.py ones
    PATTERN "tests/*_test.py" EXCLUDE
    PATTERN "docs" EXCLUDE
)