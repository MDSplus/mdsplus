# FindSybase.cmake
#
# Finds Sybase (or mssql, or freetds)
#
# This will define the following variables
#
#   Sybase_FOUND
#   Sybase_INCLUDE_DIRS
#   Sybase_LIBRARIES
#   Sybase_COMPILE_DEFINITIONS
#
# and the following imported targets
#
#   Sybase::Sybase
#
# The following variables can be set as arguments
#
#   Sybase_ROOT
#

set(Sybase_COMPILE_DEFINITIONS SYBASE)

find_path(
    Sybase_INCLUDE_DIRS
    NAMES sybdb.h
    PATH_SUFFIXES 
        include
        include/freetds
)

find_library(
    Sybase_LIBRARIES
    NAMES sybdb
    PATH_SUFFIXES 
        lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Sybase
    REQUIRED_VARS 
        Sybase_LIBRARIES
        Sybase_INCLUDE_DIRS
)

if(Sybase_FOUND)

    if(NOT TARGET Sybase::Sybase)

        add_library(Sybase::Sybase INTERFACE IMPORTED)
        
        set_target_properties(
            Sybase::Sybase
            PROPERTIES
                INTERFACE_COMPILE_DEFINITIONS "${Sybase_COMPILE_DEFINITIONS}"
                INTERFACE_INCLUDE_DIRECTORIES "${Sybase_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES "${Sybase_LIBRARIES}"
        )

    endif()

endif()

mark_as_advanced(
    Sybase_ROOT
    Sybase_COMPILE_DEFINITIONS
    Sybase_INCLUDE_DIRS
    Sybase_LIBRARIES
)