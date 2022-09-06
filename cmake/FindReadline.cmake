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

FIND_PACKAGE(PkgConfig QUIET)

PKG_CHECK_MODULES(_Readline_PC QUIET readline)

FIND_PATH(
    Readline_INCLUDE_DIRS
    NAMES readline/readline.h
    PATHS 
        ${Readline_ROOT_DIR}
        ${_Readline_PC_INCLUDE_DIRS}
    PATH_SUFFIXES 
        include
)

FIND_LIBRARY(
    Readline_readline_LIBRARY
    NAMES readline
    PATHS 
        ${Readline_ROOT_DIR}
        ${_Readline_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Readline_history_LIBRARY
    NAMES history
    PATHS 
        ${Readline_ROOT_DIR}
        ${_Readline_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    Readline
    HANDLE_COMPONENTS
    REQUIRED_VARS 
        Readline_INCLUDE_DIRS
        Readline_readline_LIBRARY
        Readline_history_LIBRARY
)

MARK_AS_ADVANCED(
    Readline_INCLUDE_DIRS
    Readline_readline_LIBRARY
    Readline_history_LIBRARY
)

IF(Readline_FOUND)
    SET(Readline_LIBRARIES
        ${Readline_readline_LIBRARY}
        ${Readline_history_LIBRARY}
    )

    IF(NOT TARGET Readline::Readline)
        ADD_LIBRARY(Readline::Readline INTERFACE IMPORTED)
        SET_TARGET_PROPERTIES(
            Readline::Readline
            PROPERTIES
                INTERFACE_LINK_LIBRARIES "${Readline_LIBRARIES}"
                INTERFACE_INCLUDE_DIRECTORIES "${Readline_INCLUDE_DIRS}"
        )
    ENDIF()
ENDIF()