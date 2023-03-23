
add_subdirectory(MDSplus)

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/MDSplus/compound.py")

    message("Generating python/MDSplus/compound.py")

    execute_process(
        COMMAND ${Python_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/gen-compound.py
            --opcodes=${CMAKE_SOURCE_DIR}/tdishr/opcodes.csv
            --input=${CMAKE_CURRENT_SOURCE_DIR}/MDSplus/compound.py.in
            --output=${CMAKE_CURRENT_BINARY_DIR}/MDSplus/compound.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

endif()

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
