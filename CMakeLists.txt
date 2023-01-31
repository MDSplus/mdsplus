cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Allow for custom CMake modules
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Disable "Built target" messages for Makefiles
set(CMAKE_TARGET_MESSAGES OFF CACHE BOOL "")

project(MDSplus)

enable_language(Fortran)

enable_testing()

###
### Options
###

# message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
# message("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

# --x-includes=
# --x-libraries=
# --build= CMAKE_BUILD_TYPE?
# --host= CMAKE_SYSTEM_NAME?
# --target= CMAKE_SYSTEM_PROCESSOR?

# replaces --disable-shared
option(BUILD_SHARED_LIBS
    "Build shared libraries"
    ON
)

if(WIN32)
    # replaces --disable-largefile
    option(ENABLE_LARGEFILE
        "Enable support for large files"
        ON
    )
endif()

if(NOT WIN32)
    # replaces --enable-perf
    option(ENABLE_PERF
        "Collect performance statistics (linux only)"
        OFF
    )
endif()

# replaces --disable-java
option(ENABLE_JAVA
    "Build java libraries and applications"
    ON
)

# (SLW) TODO
# replaces --with-java_only
option(JAVA_ONLY "")

# replaces --enable-d3d
option(ENABLE_D3D
    "Build d3d ptdata access library located in $D3DLIB_PATH."
    OFF
)

# (SLW) TODO Investigate
# replaces --disable-xmltest
option(ENABLE_XMLTEST
    "Try to compile and run a test LIBXML program."
    ON
)

# replaces --enable-valgrind
option(ENABLE_VALGRIND
    "Enable Valgrind on the unit tests."
    ON
)

# TODO: doxygen dot, man, rtf, xml, chm, chi, html
# replaces --disable-doxygen-doc/dot/man/rtf/xml/chm/chi/html
option(ENABLE_DOXYGEN
    "Generate doxygen documentation."
    ON
)

# replaces --enable-werror
option(ENABLE_WERROR
    "Treat all warnings as errors (-Werror)."
    OFF
)

# replaces --disable-wreturns
option(ENABLE_WRETURNS
    "Enable warnings about unused return values from functions (-Wreturns)."
    ON
)

# replaces --enable-wconversion
option(ENABLE_WCONVERSION
    "Enable warnings about implicit conversions (-Wconversion)."
    OFF
)

# replaces --enable-sanitize=
set(SANITIZER "address"
    CACHE STRING
    "Enable compile sanitizer with flavor, options are: address or thread. Default is address."
)
set_property(
    CACHE SANITIZER
    PROPERTY STRINGS "address" "thread" ""
)

# replaces --with-jars
set(JARS ""
    CACHE STRING
    "???"
)

# replaces --with-jdk
set(JDK_DIR ""
    CACHE STRING
    "Location of Java JDK"
)

# replaces --with-java_target=
set(JAVA_TARGET_VERSION ""
    CACHE STRING
    "Version of compiled mdsobjects classes (i.e. 6)"
)

# replaces --with-java_bootclasspath=
set(JAVA_BOOTCLASSPATH ""
    CACHE STRING
    "The bootclasspath option for use with JAVA_TARGET_VERSION (i.e. /usr/lib/jvm/java-1.6.0-openjdk-1.6.0.0/jre/lib/rt.jar)"
)

# replaces --with-gsi=globus-location:flavor
set(GSI ""
    CACHE STRING
    "Set in the form of globus-location:flavor. Enables globus GSI for mdsip communications."
)

# replaces --with-srb=
set(SRB_DIR ""
    CACHE STRING
    "Location of SRB to build against"
)

# replaces --with-labview=
set(LABVIEW_DIR ""
    CACHE STRING
    "Location of labview (i.e. /usr/local/natinst/Labview)"
)

# replaces --with-idl=
option(ENABLE_IDL
    "Build idlmdsevent and idlmdswidgets against the IDL installed in IDL_DIR. If IDL_DIR is not set, the system will be searched for an installation."
    ON
)

set(IDL_DIR "" CACHE STRING
    "Location of IDL, or empty to indicate the system should be searched for an IDL installation. Setting this also sets ENABLE_IDL=ON."
)

if(IDL_DIR AND NOT ENABLE_IDL)
    message("Setting ENABLE_IDL=ON because IDL_DIR was specified")
    set(ENABLE_IDL ON)
endif()

# replaces --with-readline=
set(READLINE_DIR ""
    CACHE STRING
    "Location of the readline library, if not set the system will be searched."
)

# replaces --with-xml-prefix, --with-xml-exec-prefix
set(LIBXML2_DIR ""
    CACHE STRING
    "Location of libxml2 library"
)

# TODO:
# replaces --with-x
option(ENABLE_X
    ""
    OFF
)

# TODO:
# replaces --with-valgrind-lib
set(VALGRIND_LIB_DIR ""
    CACHE STRING
    ""
)

# TODO:
# replaces --with-winebottle
set(WINEBOTTLE ""
    CACHE STRING
    ""
)

###
### Global Configuration
###

set(CMAKE_C_FLAGS $ENV{CFLAGS})
set(CMAKE_CXX_FLAGS $ENV{CXXFLAGS})
set(CMAKE_Fortran_FLAGS $ENV{FCFLAGS})

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(
    # (SLW) TODO: Move to testing/backends/check or remove altogether
    -DHAVE_CONFIG_H # Do we have mdsplus/mdsconfig.h
)

if(MSVC)
    add_compile_options(/Wall)
else()
    add_compile_options(-Wall -Wextra)
endif()

if(ENABLE_WERROR)
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

if(NOT ENABLE_WRETURNS) # -Wunused-result is on by default in gcc and clang
    if(NOT MSVC)
        add_compile_options(-Wno-unused-result)
    endif()
endif()

# if(ENABLE_WCONVERSION)
#     add_compile_options(-Wconversion)
# endif()

if(ENABLE_LARGEFILE)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        ADD_DEFINITIONS(
            -D_FILE_OFFSET_BITS=64
            -D_LARGE_FILES
        )
    endif()
endif()

###
### Dependencies
###

if(ENABLE_JAVA)

    if(JDK_DIR)
        set(JAVA_HOME "${JDK_DIR}")
    endif()

    find_package(Java COMPONENTS Runtime Development REQUIRED)
    find_package(JNI REQUIRED)
    
    include(UseJava)

    find_program(mvn_EXECUTABLE mvn)

endif()

if(ENABLE_IDL)

    if(IDL_DIR)
        set(IDL_ROOT_DIR "${IDL_DIR}")
    elseif(NOT IDL_FOUND)
        set(IDL_ROOT_DIR 
            /usr/local/harris/idl
            /usr/local/exelis/idl
            /usr/local/itt/idl
            /usr/local/rsi/idl
            ${CMAKE_SOURCE_DIR}/3rd-party-apis/idl
        )
    endif()

    # cmake/FindIDL.cmake
    find_package(IDL REQUIRED)

endif()

if(ENABLE_D3D)

    find_library(
        LIBD3
        NAMES d3
        PATHS $ENV{D3DLIB_PATH}
        REQUIRED
    )

    mark_as_advanced(LIBD3)

endif()

if(GSI)

    STRING(REPLACE ":" ";" _gsi_list "${GSI}")
    LIST(GET _gsi_list 0 Globus_ROOT_DIR)
    LIST(GET _gsi_list 1 Globus_FLAVOR)

    # (SLW) TODO: pthr???
        
    # cmake/FindGlobus.cmake
    find_package(Globus)

endif()

find_package(Python COMPONENTS Interpreter REQUIRED)

find_package(Threads REQUIRED)

find_package(Iconv REQUIRED)

# cmake/FindReadline.cmake
set(Readline_ROOT_DIR "${READLINE_DIR}")
find_package(Readline REQUIRED)

# cmake/Findlibxml2.cmake
find_package(libxml2 REQUIRED)

# cmake/FindMotif.cmake
find_package(Motif)

# cmake/FindSybase.cmake
find_package(Sybase)

# cmake/FindHDF5.cmake
set(HDF5_ROOT_DIR $ENV{HDF5_DIR})
find_package(HDF5)

find_library(LIBM m QUIET)
find_library(LIBZ z QUIET)
find_library(LIBLZMA lzma QUIET)
find_library(LIBBLAS blas QUIET)
find_library(LIBRT rt QUIET)
find_library(LIBCURL curl QUIET)
set(LIBDL "${CMAKE_DL_LIBS}")

mark_as_advanced(LIBM LIBZ LIBLZMA LIBBLAS LIBRT LIBCURL LIBDL)

###
### Check for includes, Libraries, and Symbols
###

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)

# C99
check_include_file("stdint.h" HAVE_STDINT_H) # TODO: Remove

# POSIX
check_include_file("alloca.h" HAVE_ALLOCA_H) # TODO: Remove?
check_include_file("grp.h" HAVE_GRP_H)
check_include_file("pthread.h" HAVE_PTHREAD_H)
check_include_file("pwd.h" HAVE_PWD_H)
check_include_file("sys/resource.h" HAVE_SYS_RESOURCE_H)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("unistd.h" HAVE_UNISTD_H)
check_include_file("sys/filio.h" HAVE_SYS_FILIO_H)
check_symbol_exists(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
check_symbol_exists(fork "unistd.h" HAVE_FORK)
check_symbol_exists(getaddrinfo "netdb.h" HAVE_GETADDRINFO)
check_symbol_exists(getpwuid "pwd.h" HAVE_GETPWUID)
check_symbol_exists(getrusage "sys/resource.h" HAVE_GETRUSAGE)
check_symbol_exists(mkstemp "stdlib.h" HAVE_MKSTEMP)
check_symbol_exists(pthread_lock_global_np "pthread.h" HAVE_PTHREAD_LOCK_GLOBAL_NP)

# FreeTDS / Sybase
check_include_files("sybdb.h;sybfront.h" HAVE_SYBASE) # TODO: Improve

# VxWorks Real-Time Operating System
check_include_file("vxWorks.h" HAVE_VXWORKS_H)

# Readline
check_include_file("readline/readline.h" HAVE_READLINE_READLINE_H)
check_symbol_exists("rl_set_signals" "readline/readline.h" HAVE_RL_SET_SIGNALS)

# Valgrind
check_include_files("valgrind/valgrind.h" HAVE_VALGRIND_H)

# Linux Kernel
check_include_files("scsi/sg.h" HAVE_SCSI_SG_H)

# TODO HAVE_VISIBILITY
set(HAVE_VISIBILITY 1)

###
### Install
###

set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(LIB_DIR ${CMAKE_BINARY_DIR}/lib)
set(ETC_DIR ${CMAKE_BINARY_DIR}/etc)
set(UID_DIR ${CMAKE_BINARY_DIR}/uid)
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set(RPM_DIR ${CMAKE_BINARY_DIR}/rpm)

file(MAKE_DIRECTORY 
    ${BIN_DIR}
    ${LIB_DIR}
    ${ETC_DIR}
    ${UID_DIR}
    ${BIN_DIR}
    ${RPM_DIR}
)

set(MODE_777
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_WRITE GROUP_EXECUTE
    WORLD_READ WORLD_WRITE WORLD_EXECUTE
)

set(MODE_755
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ             GROUP_EXECUTE
    WORLD_READ             WORLD_EXECUTE
)

set(MODE_644
    OWNER_READ OWNER_WRITE
    GROUP_READ            
    WORLD_READ            
)

install(
    DIRECTORY
        desktop
        epics
        idl
        include
        LabView
        matlab
        nodejs
        php
        pixmaps
        pydevices
        tdi
        trees
        xml
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_BINARY_DIR}/uid
    DESTINATION .
    USE_SOURCE_PERMISSIONS
    PATTERN "*.in" EXCLUDE
    PATTERN "Makefile.*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
)

install(
    DIRECTORY python/MDSplus
    DESTINATION python
    USE_SOURCE_PERMISSIONS
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    PATTERN "*.in" EXCLUDE
    PATTERN "Makefile.*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
)

install(
    FILES
        MDSplus-License.rtf
        MDSplus-License.txt
    DESTINATION .
)

install(
    FILES
        setup.csh
        setup.sh
    DESTINATION .
    PERMISSIONS ${MODE_755}
)

install(
    DIRECTORY
    DESTINATION local/tdi
)

# (SLW) TODO: 
set(LIBPATH "LD_LIBRARY_PATH")

###
### Generate _include/_mdsversion.h
###

include(GitRevision)

# (SLW) TODO
set(LIBPREFIX "Mds")

configure_file(
    ${CMAKE_SOURCE_DIR}/_include/_mdsversion.h.in
    ${CMAKE_BINARY_DIR}/_include/_mdsversion.h
)

###
### Generate include/mdsplus/mdsconfig.h
###

# (SLW) TODO
set(SHARELIB_TYPE "${CMAKE_SHARED_LIBRARY_SUFFIX}")

configure_file(
    ${CMAKE_SOURCE_DIR}/include/mdsplus/mdsconfig.h.in
    ${CMAKE_BINARY_DIR}/include/mdsplus/mdsconfig.h
)

include_directories(
    ${CMAKE_SOURCE_DIR}/_include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/mdsplus

    ${CMAKE_BINARY_DIR}/_include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/include/mdsplus
)

###
### python/generate_tests
###

# file(GLOB
#     _test_case_list
#     "python/MDSplus/tests/*_case.py"
# )

# foreach(_test_case IN LISTS _test_case_list)
#     cmake_path(GET _test_case STEM _test_case_name)
#     ADD_TEST(
#         NAME ${_test_case_name}
#         COMMAND ${Python_EXECUTABLE} ${_test_case}
#     )

#     set(_env_mods
#         "PATH=path_list_prepend:${BIN_DIR}"
#         "PYTHONPATH=set:${CMAKE_SOURCE_DIR}/python"
#         "PYTHONPATH=path_list_append:${CMAKE_BINARY_DIR}/python"
#     )

#     SET_TESTS_PROPERTIES(
#         ${_test_case_name}
#         PROPERTIES
#             ENVIRONMENT_MODIFICATION "${_env_mods}"
                
#     )
# endforeach()

###
### Generate include/*_messages.h
###

file(GLOB_RECURSE
    _messages_xml_list
    "*/*messages.xml"
)

set(_messages_generated_list "python/MDSplus/mdsExceptions.py")

foreach(_messages_xml IN LISTS _messages_xml_list)
    cmake_path(GET _messages_xml STEM _stem)
    LIST(APPEND _messages_generated_list "include/${_stem}.h")
endforeach()

set(_run_gen_messages FALSE)
foreach(_filename IN LISTS _messages_generated_list)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${_filename}")
        message("Missing ${_filename}, will run deploy/gen_messages.py")
        set(_run_gen_messages TRUE)
    endif()
endforeach()

if(_run_gen_messages)
    message("Running deploy/gen_messages.py")
    execute_process(
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/deploy/gen_messages.py
    )
endif()

# message("${_messages_generated_list}")

# add_custom_command(
#     COMMENT "Running deploy/gen_messages.py"
#     OUTPUT ${_messages_generated_list}
#     DEPENDS ${_messages_xml_list}
#     COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/deploy/gen_messages.py
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

###
### Generate python/MDSplus/compound.py
###

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/python/MDSplus/compound.py")
    message("Missing python/MDSplus/compound.py, will run deploy/gen_compound_opcbuiltins.py")
    execute_process(
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/deploy/gen_compound_opcbuiltins.py
    )
endif()

###
### bison / flex
###

# TODO: Find Bison and Flex and gperf?
# TODO: Macro for running bison/flex?

###
### Documentation
###

find_package(Doxygen)

set(doxygen_EXECUTABLE $<TARGET_FILE:Doxygen::doxygen>)

add_custom_target(
    docs
)

###
### Libraries, Executables
###

include(AddStaticCopy)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})

# Fortran Libraries

add_subdirectory(math)
add_subdirectory(d3dshr)

# Core Libraries

add_subdirectory(mdsshr)
add_subdirectory(treeshr) # Requires MdsShr
add_subdirectory(xtreeshr) # Requires TreeShr
add_subdirectory(tdishr) # Requires TreeShr
add_subdirectory(mdstcpip) # Requires TdiShr
add_subdirectory(mdslib) # Requires TdiShr
add_subdirectory(xmdsshr) # Requires TdiShr
add_subdirectory(mdsmisc) # Requires XTreeShr
add_subdirectory(mdsdcl) # Requires MdsIpShr
add_subdirectory(servershr) # Requires Mdsdcl
add_subdirectory(mdssql)
add_subdirectory(mdsvme)

# Language Binding Libraries

add_subdirectory(mdslibidl) # Requires TdiShr
add_subdirectory(javamds) # Requires TdiShr
add_subdirectory(mdsobjects/cpp) # Requires JavaMds
add_subdirectory(mdsobjects/labview) # Requires MdsObjectsCppShr
add_subdirectory(python)
add_subdirectory(java)

# Command Line Tools

add_subdirectory(tdic)
add_subdirectory(tditest)
add_subdirectory(setevent)
add_subdirectory(wfevent)
# actions has actlog

# Graphical Tools

add_subdirectory(actions)
add_subdirectory(dwscope)
add_subdirectory(traverser)
add_subdirectory(idlmdsevent)
add_subdirectory(idlmdswidgets)

# Device Support

add_subdirectory(mitdevices)
add_subdirectory(camshr)
add_subdirectory(remcam) # Requires camshr
add_subdirectory(pydevices)

# Mdsdcl commands

add_subdirectory(tcl)
add_subdirectory(ccl) # Requires CamShr
# camshr has cts_commands

# Globus

add_subdirectory(roam)
# mdstcpip has MdsIpGSI

# Configuration

add_subdirectory(rpm)
add_subdirectory(scripts)
add_subdirectory(manpages)

###
### Testing
###

add_subdirectory(testing)
add_subdirectory(mdsshr/testing)
# add_subdirectory(treeshr/testing)
# add_subdirectory(mdslib/testing)
# add_subdirectory(mdstcpip/testing)
add_subdirectory(tdishr/testing)
# add_subdirectory(tditest/testing)
# add_subdirectory(mdsobjects/cpp/testing)
add_subdirectory(python/MDSplus/tests)

add_subdirectory(examples/demoadc)

if(Motif_FOUND)

    ###
    ### Process *.uil
    ###

    file(GLOB_RECURSE
        _uil_list
        "actions/*.uil"
        "dwscope/*.uil"
        "traverser/*.uil"
        "mitdevices/*.uil"
        "idlmdswidgets/*.uil"
        "xmdsshr/*.uil"
    )

    foreach(_uil IN LISTS _uil_list)
        cmake_path(GET _uil STEM LAST_ONLY _stem)
        set(_uid ${UID_DIR}/${_stem}.uid)
        
        add_custom_command(
            OUTPUT ${_uid}
            DEPENDS ${_uil}
            COMMAND ${uil_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/include -o ${_uid} ${_uil}
        )

        LIST(APPEND _uid_list ${_uid})
    endforeach()

    add_custom_target(
        generate-uids ALL
        DEPENDS ${_uid_list}
    )

endif()

###
### ChangeLog
###

git(GIT_CHANGELOG log --decorate=full --no-merges)

file(WRITE ${CMAKE_BINARY_DIR}/ChangeLog "${GIT_CHANGELOG}")

install(
    FILES ${CMAKE_BINARY_DIR}/ChangeLog
    DESTINATION .
)