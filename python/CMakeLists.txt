
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
    PATTERN "docs" EXCLUDE
)