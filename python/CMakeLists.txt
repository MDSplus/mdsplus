
add_subdirectory(MDSplus)

install(
    DIRECTORY .
    DESTINATION python
    FILE_PERMISSIONS ${MODE_755}
    PATTERN "gen-*" EXCLUDE
    PATTERN "*.in" EXCLUDE
    PATTERN "Makefile*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    # We install the _case.py files, but not the individual _test.py ones
    PATTERN "tests/*_test.py" EXCLUDE
    PATTERN "docs" EXCLUDE
)

# if(WIN32)
#     set(_ext ".exe")
# endif()

# execute_process(
#     COMMAND ${Python_EXECUTABLE}
#         ${CMAKE_SOURCE_DIR}/scripts/generate-launch-targets.py
#         --name "python"
#         --type "python"
#         --executable "${Python_EXECUTABLE}"
#         --working-dir "${CMAKE_SOURCE_DIR}"
#         --runtime-path "${CMAKE_BINARY_DIR}/lib"
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )