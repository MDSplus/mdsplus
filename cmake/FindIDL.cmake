# FindIDL.cmake
#
# Finds IDL's export.h
#
# This will define the following variables
#
#   IDL_FOUND
#   IDL_INCLUDE_DIRS
#
# and the following imported targets
#
#   IDL::IDL
#
# The following variables can be set as arguments
#
#   IDL_ROOT_DIR
#

FIND_PATH(
    IDL_INCLUDE_DIRS
    NAMES export.h
    PATHS 
        ${IDL_ROOT_DIR}
    PATH_SUFFIXES
        external
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    IDL
    HANDLE_COMPONENTS
    REQUIRED_VARS 
        IDL_INCLUDE_DIRS
)

MARK_AS_ADVANCED(
    IDL_INCLUDE_DIRS
)

IF(IDL_FOUND)
    IF(NOT TARGET IDL::IDL)
        ADD_LIBRARY(IDL::IDL INTERFACE IMPORTED)
        SET_TARGET_PROPERTIES(
            IDL::IDL PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${IDL_INCLUDE_DIRS}"
        )
    ENDIF()
ENDIF()