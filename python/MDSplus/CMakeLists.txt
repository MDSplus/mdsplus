
# Unlike other files we generate, these are placed in the source directory
# This is to facilitate development of the Python library, without the need
# to `make install` after every file change.

###
### Generate _version.py
###

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/_version.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/_version.py
)

###
### Generate compound.py
###

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/compound.py")

    message("Generating python/MDSplus/compound.py")

    execute_process(
        COMMAND ${Python_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/MDSplus/gen-compound.py
            --opcodes=${CMAKE_SOURCE_DIR}/tdishr/opcodes.csv
            --input=${CMAKE_CURRENT_SOURCE_DIR}/compound.py.in
            --output=${CMAKE_CURRENT_SOURCE_DIR}/compound.py
    )

endif()
