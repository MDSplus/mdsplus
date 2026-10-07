# Add the MDSplus Python package subdirectory
add_subdirectory(MDSplus)

# Find Python interpreter for building the wheel
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Define where the wheel will be placed
set(WHEEL_OUTPUT_DIR ${CMAKE_BINARY_DIR}/python)

# Ensure the output directory exists
file(MAKE_DIRECTORY ${WHEEL_OUTPUT_DIR})

# Custom target that depends on the build command
add_custom_target(build_wheel ALL
    ${Python3_EXECUTABLE} -m pip wheel --wheel-dir ${WHEEL_OUTPUT_DIR} --no-deps ${CMAKE_CURRENT_SOURCE_DIR}/MDSplus
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/MDSplus/_version.py
    COMMENT "Building MDSplus wheel"
)

# After building the wheel, install it into the python directory of the project

install(DIRECTORY ${WHEEL_OUTPUT_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        FILES_MATCHING
            PATTERN "*.whl"
            PATTERN "MDSplus" EXCLUDE
            PATTERN "CMakeFiles" EXCLUDE
)
