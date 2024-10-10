# FindReadline.cmake
#
# Finds libreadline and libhistory
#
# This will define the following variables
#
#   Readline_FOUND
#   Readline_INCLUDE_DIRS
#   Readline_LIBRARIES
#   Readline_readline_LIBRARY
#   Readline_history_LIBRARY
#
# and the following imported targets
#
#   Readline::Readline
#
# The following variables can be set as arguments
#
#   Readline_ROOT_DIR
#

find_package(PkgConfig QUIET)

pkg_check_modules(_Readline_PC QUIET readline)

find_path(
    Readline_INCLUDE_DIRS
    NAMES readline/readline.h
    PATHS 
        ${Readline_ROOT_DIR}
        ${_Readline_PC_INCLUDE_DIRS}
    PATH_SUFFIXES 
        include
)

find_library(
    Readline_readline_LIBRARY
    NAMES readline
    PATHS 
        ${Readline_ROOT_DIR}
        ${_Readline_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Readline_history_LIBRARY
    NAMES history
    PATHS 
        ${Readline_ROOT_DIR}
        ${_Readline_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Readline
    REQUIRED_VARS 
        Readline_readline_LIBRARY
        Readline_history_LIBRARY
        Readline_INCLUDE_DIRS
)

if(Readline_FOUND)

    set(Readline_LIBRARIES
        ${Readline_readline_LIBRARY}
        # ${Readline_history_LIBRARY}
    )

    if(NOT TARGET Readline::Readline)

        add_library(Readline::Readline INTERFACE IMPORTED)

        set_target_properties(
            Readline::Readline
            PROPERTIES
                INTERFACE_LINK_LIBRARIES "${Readline_LIBRARIES}"
                INTERFACE_INCLUDE_DIRECTORIES "${Readline_INCLUDE_DIRS}"
        )

    endif()
    
endif()

mark_as_advanced(
    Readline_INCLUDE_DIRS
    Readline_LIBRARIES
    Readline_readline_LIBRARY
    Readline_history_LIBRARY
)