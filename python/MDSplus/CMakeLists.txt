
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

# if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compound.py")

#     message("Generating python/MDSplus/compound.py")

#     execute_process(
#         COMMAND ${Python_EXECUTABLE}
#             ${CMAKE_CURRENT_SOURCE_DIR}/gen-compound.py
#             --opcodes=${CMAKE_SOURCE_DIR}/tdishr/opcodes.csv
#             --input=${CMAKE_CURRENT_SOURCE_DIR}/compound.py.in
#             --output=${CMAKE_CURRENT_BINARY_DIR}/compound.py
#         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#     )

# endif()
