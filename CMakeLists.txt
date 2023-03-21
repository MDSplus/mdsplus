cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Allow for custom CMake modules
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Disable "Built target" messages for Makefiles
set(CMAKE_TARGET_MESSAGES OFF CACHE BOOL "")
mark_as_advanced(CMAKE_TARGET_MESSAGES)

project(MDSplus)

enable_language(Fortran)

# Query version and release information from git
include(GitRevision)

###
### Update Submodules
###

find_package(Git QUIET)

if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule --quiet update --init --recursive
    )
endif()

###
### Options
###

# message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
# message("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

# --x-includes=
# --x-libraries=
# --build=
# --host= CMAKE_SYSTEM_NAME?
# --target= CMAKE_SYSTEM_PROCESSOR?

# replaces --disable-shared
option(BUILD_SHARED_LIBS
    "Build shared libraries [default: ON]"
    ON
)

option(ENABLE_TESTS
    "Build test frameworks and executables [default: ON]"
    ON
)

# replaces --enable-debug
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

    set(CMAKE_BUILD_TYPE "Release"
        CACHE STRING
        "Type of build to configure, can be one of: Debug, Release, RelWithDebInfo, MinSizeRel [default: Release]"
        FORCE
    )

    set_property(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS
            "Debug"
            "Release"
            "MinSizeRel"
            "RelWithDebInfo"
    )

endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    option(
        ENABLE_GPROF
        "Enable profiling with gprof (compile and link with -pg). [default: OFF]"
        OFF
    )
endif()

# replaces --prefix
# TODO: --exec-prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

  set(CMAKE_INSTALL_PREFIX "/usr/local/mdsplus"
      CACHE STRING
      "Where to install files [default: /usr/local/mdsplus]"
      FORCE
  )

endif()

# Must be included after CMAKE_INSTALL_PREFIX
# Defines the following that we use:
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_SYSCONFIGDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

if(WIN32)
    # replaces --disable-largefile
    option(ENABLE_LARGEFILE
        "Enable support for large files [default: ON]"
        ON
    )
endif()

if(NOT WIN32)
    # replaces --enable-perf
    option(ENABLE_PERF
        "Collect performance statistics [default: OFF]"
        OFF
    )
endif()

# replaces --disable-java
option(ENABLE_JAVA
    "Build java libraries and applications [default: ON]"
    ON
)

# (SLW) TODO
# replaces --with-java_only
#option(JAVA_ONLY "")

# replaces --enable-d3d
option(ENABLE_D3D
    "Build MdsD3D library against d3 library located in $D3DLIB_PATH [default: OFF]"
    OFF
)

# (SLW) TODO Investigate
# replaces --disable-xmltest
option(ENABLE_XMLTEST
    "Try to compile and run a test LIBXML program [default: ON]"
    ON
)

# replaces --enable-valgrind
option(ENABLE_VALGRIND
    "Enable Valgrind on the unit tests [default: ON]"
    ON
)

# TODO: doxygen dot, man, rtf, xml, chm, chi, html
# replaces --disable-doxygen-doc/dot/man/rtf/xml/chm/chi/html
option(ENABLE_DOXYGEN
    "Generate doxygen documentation [default: ON]"
    ON
)

# replaces --enable-werror
option(ENABLE_WERROR
    "Treat all warnings as errors (-Werror) [default: ON]"
    OFF
)

# replaces --disable-wreturns
option(ENABLE_WRETURNS
    "Enable warnings about unused return values from functions (-Wreturns) [default: ON]"
    ON
)

# replaces --enable-wconversion
option(ENABLE_WCONVERSION
    "Enable warnings about implicit conversions (-Wconversion) [default: ON]"
    OFF
)

# replaces --enable-sanitize=
set(SANITIZE "address"
    CACHE STRING
    "Enable compile sanitizer with flavor, options are: address or thread. Default is address."
)
set_property(
    CACHE SANITIZE
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
# TODO: Investigate
set(JAVA_TARGET_VERSION ""
    CACHE STRING
    "Version of compiled mdsobjects classes (i.e. 6)"
)

# replaces --with-java_bootclasspath=
# TODO: Investigate
set(JAVA_BOOTCLASSPATH ""
    CACHE STRING
    "The bootclasspath option for use with JAVA_TARGET_VERSION (i.e. /usr/lib/jvm/java-1.6.0-openjdk-1.6.0.0/jre/lib/rt.jar)"
)

# replaces --with-gsi=globus-location:flavor
set(GSI ""
    CACHE STRING
    "Set in the form of flavor or globus-location:flavor. Enables globus GSI for mdsip communications."
)

# replaces --with-srb=
# TODO: Investigate
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

# $ENV{CFLAGS} is added by default
# $ENV{CXXFLAGS} is added by default
# $ENV{FFLAGS} is added by default
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} $ENV{FCFLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(
    ${CMAKE_SOURCE_DIR}/_include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/mdsplus

    ${CMAKE_BINARY_DIR}/_include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/include/mdsplus
)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
mark_as_advanced(CMAKE_EXPORT_COMPILE_COMMANDS)

add_compile_definitions(
    # (SLW) TODO: Move to testing/backends/check or remove altogether
    HAVE_CONFIG_H # Do we have mdsplus/mdsconfig.h
)

if(MSVC)
    add_compile_options(/Wall)
else()
    add_compile_options(-Wall -Wextra)
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")

    # gprof is only supported with gcc
    if(ENABLE_GPROF)
        STRING(TOLOWER "${CMAKE_BUILD_TYPE}" _build_type)
        if(NOT _build_type STREQUAL "relwithdebinfo")
            message(WARNING "It is recommended to use CMAKE_BUILD_TYPE=RelWithDebInfo with ENABLE_GPROF=ON")
        endif()

        add_compile_options(-pg)
        add_link_options(-pg)
    endif()
    
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

if(ENABLE_TESTS)
    enable_testing()
endif()

# if(ENABLE_WCONVERSION)
#     add_compile_options(-Wconversion)
# endif()

if(ENABLE_LARGEFILE)

    # Check for 32-bit target
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        add_compile_definitions(
            _FILE_OFFSET_BITS=64
            _LARGE_FILES
        )
    endif()

endif()

###
### Dependencies
###

# CMake built-in packages to find

if(DEFINED ENV{PYTHON})
    set(Python_EXECUTABLE "$ENV{PYTHON}")
else()
    find_package(Python COMPONENTS Interpreter REQUIRED)
endif()

# Finds the platform specific threading libraries, usually pthreads
find_package(Threads REQUIRED)

# Finds the POSIX iconv() family of functions, which might be built into libc
find_package(Iconv REQUIRED)

# Finds the Basic Linear Algebra Subprograms fortran functions
find_package(BLAS REQUIRED)

# Finds the LZMA compression library
find_package(LibLZMA REQUIRED)

# Finds the ZLIB compression library
find_package(ZLIB REQUIRED)

set(libxml2_ROOT_DIR "${LIBXML2_DIR}")
mark_as_advanced(libxml2_ROOT_DIR)
find_package(LibXml2 REQUIRED)

# Custom packages to find

# cmake/FindLibM.cmake
find_package(LibM REQUIRED)

# cmake/FindReadline.cmake
set(Readline_ROOT_DIR "${READLINE_DIR}")
mark_as_advanced(Readline_ROOT_DIR)
find_package(Readline REQUIRED)

# cmake/FindMotif.cmake
find_package(Motif)

# cmake/FindSybase.cmake
find_package(Sybase)

# cmake/FindHDF5.cmake
set(HDF5_ROOT_DIR "$ENV{HDF5_DIR}")
mark_as_advanced(HDF5_ROOT_DIR)
find_package(HDF5)

# cmake/FindDC1394.cmake
find_package(DC1394)

# Configuration-dependent dependencies

if(ENABLE_TESTS)

    # cmake/FindLibRT.cmake
    find_library(LIBRT rt REQUIRED)
    # find_package(LibRT REQUIRED)
    # -lrt

endif()

if(ENABLE_JAVA)

    if(JDK_DIR)
        set(JAVA_HOME "${JDK_DIR}")
    else()
        set(JAVA_HOME "$ENV{JAVA_HOME}")
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
        D3
        NAMES d3
        PATHS $ENV{D3DLIB_PATH}
        REQUIRED
    )

    mark_as_advanced(D3)

endif()

if(GSI)

    string(REPLACE ":" ";" _gsi_list "${GSI}")
    list(LENGTH _gsi_list _gsi_list_length)

    if(_gsi_list_length LESS 2)
        set(Globus_FLAVOR ${_gsi_list})
    else()
        list(GET _gsi_list 0 Globus_ROOT_DIR)
        list(GET _gsi_list 1 Globus_FLAVOR)
    endif()

    # (SLW) TODO: pthr???

    # cmake/FindGlobus.cmake
    find_package(Globus REQUIRED)

    # Only used by Roam, which is only enabled with Globus
    find_package(CURL REQUIRED)

endif()

###
### Check for includes, Libraries, and Symbols
###

include(CheckIncludeFiles)
include(CheckSymbolExists)

# POSIX
check_include_files("alloca.h" HAVE_ALLOCA_H) # TODO: Remove?
check_include_files("grp.h" HAVE_GRP_H)
check_include_files("pthread.h" HAVE_PTHREAD_H)
check_include_files("pwd.h" HAVE_PWD_H)
check_include_files("sys/resource.h" HAVE_SYS_RESOURCE_H)
check_include_files("sys/types.h" HAVE_SYS_TYPES_H)
check_include_files("unistd.h" HAVE_UNISTD_H)
check_include_files("sys/filio.h" HAVE_SYS_FILIO_H)
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
check_include_files("vxWorks.h" HAVE_VXWORKS_H)

# Readline
check_include_files("readline/readline.h" HAVE_READLINE_READLINE_H)
check_symbol_exists("rl_set_signals" "readline/readline.h" HAVE_RL_SET_SIGNALS)

# Valgrind
check_include_files("valgrind/valgrind.h" HAVE_VALGRIND_H)

# Linux Kernel
check_include_files("scsi/sg.h" HAVE_SCSI_SG_H)

###
### Generate _include/_mdsversion.h
###

configure_file(
    ${CMAKE_SOURCE_DIR}/_include/_mdsversion.h.in
    ${CMAKE_BINARY_DIR}/_include/_mdsversion.h
)

###
### Generate include/mdsplus/mdsconfig.h
###

if(BUILD_SHARED_LIBS)
    set(SHARELIB_TYPE "${CMAKE_SHARED_LIBRARY_SUFFIX}")
else()
    set(SHARELIB_TYPE "${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

# TODO HAVE_VISIBILITY
set(HAVE_VISIBILITY ON)

configure_file(
    ${CMAKE_SOURCE_DIR}/include/mdsplus/mdsconfig.h.in
    ${CMAKE_BINARY_DIR}/include/mdsplus/mdsconfig.h
)

###
### Generate include/opcbuiltins.h
###

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/include/opcbuiltins.h)

    message("Generating include/opcbuiltins.h")

    execute_process(
        COMMAND ${Python_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/include/opcbuiltins.py
            --opcodes=${CMAKE_SOURCE_DIR}/tdishr/opcodes.csv
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/include
    )

endif()

###
### Generate python/MDSplus/tests/*_case.py
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

set(_messages_generated_file_list
    "${CMAKE_BINARY_DIR}/python/MDSplus/mdsExceptions.py"
)

# Determine the header filenames that will be created
foreach(_messages_xml IN LISTS _messages_xml_list)
    cmake_path(GET _messages_xml STEM _stem)
    LIST(APPEND _messages_generated_file_list "${CMAKE_BINARY_DIR}/include/${_stem}.h")
endforeach()

# Check to see if we already have all the files generated
set(_run_gen_messages FALSE)
foreach(_file IN LISTS _messages_generated_file_list)
    if(NOT EXISTS "${_file}")
        message("Missing ${_file}, will run deploy/gen_messages.py")
        set(_run_gen_messages TRUE)
    endif()
endforeach()

# If not, generate them
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

# file(GLOB _directory_list ${CMAKE_SOURCE_DIR}/*)
# foreach(_directory ${_directory_list})
#     if(IS_DIRECTORY ${_directory})
#         string(REPLACE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" _directory "${_directory}")
#         file(MAKE_DIRECTORY ${_directory})
#     endif()
# endforeach()

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
### Generate sources with Yacc/Bison
###

if(NOT _yacc_has_run)

    message("Processing Yacc files")

    find_program(
        yacc_EXECUTABLE
        NAMES bison yacc
        REQUIRED
    )

    file(GLOB_RECURSE
        _yacc_input_list
        "*.y"
    )

    foreach (_file ${_yacc_input_list})

        # Make output directory
        cmake_path(GET _file PARENT_PATH _parent_path)
        string(REPLACE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" _binary_path "${_parent_path}")
        file(MAKE_DIRECTORY "${_binary_path}")

        message("Running ${yacc_EXECUTABLE} --verbose ${_file}")
        execute_process(
            COMMAND ${yacc_EXECUTABLE} --verbose ${_file}
        )

    endforeach()

    # Only run this the first time
    set(_yacc_has_run TRUE CACHE INTERNAL "")

endif()

###
### Generate sources with Lex/Flex
###

if(NOT _lex_has_run)

    message("Processing Lex files")

    find_program(
        lex_EXECUTABLE
        NAMES flex lex
        REQUIRED
    )

    file(GLOB_RECURSE
        _lex_input_list
        "*.l"
    )

    foreach (_file ${_lex_input_list})

        # Make output directory
        cmake_path(GET _file PARENT_PATH _parent_path)
        string(REPLACE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" _binary_path "${_parent_path}")
        file(MAKE_DIRECTORY "${_binary_path}")

        message("Running ${lex_EXECUTABLE} -p ${_file}")
        execute_process(
            COMMAND ${lex_EXECUTABLE} -p ${_file}
        )

    endforeach()

    # Only run this the first time
    set(_lex_has_run TRUE CACHE INTERNAL "")

endif()

###
### Generate Documentation
###

find_package(Doxygen)

if(Doxygen_FOUND)

    add_custom_target(
        docs
    )

endif()

###
### Libraries, Executables
###

# Some libraries need both a Shared and a Static version, this includes a macro that helps streamline
# that and create a second library target with the suffix `-static`. For `MdsShr` for example, it will
# create the target `MdsShr-static`, which could then produce `libMdsShr.a`. If BUILD_SHARED_LIBS=OFF,
# then only the static libraries are built, and the `-static` targets will be ALIAS targets instead.
include(AddStaticCopy)

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
# add_subdirectory(mdsvme)

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
# actlog is in actions/

# Graphical Tools

add_subdirectory(actions)
add_subdirectory(dwscope)
add_subdirectory(traverser)
add_subdirectory(idlmdsevent)
add_subdirectory(idlmdswidgets)

# Device Support

add_subdirectory(mitdevices)
add_subdirectory(camshr)
add_subdirectory(remcam) # Requires CamShr
add_subdirectory(pydevices)

# Mdsdcl commands

add_subdirectory(tcl)
add_subdirectory(ccl) # Requires CamShr
# cts_commands is in camshr/

# Globus

add_subdirectory(roam)
# MdsIpGSI is in mdstcpip/

# Configuration

add_subdirectory(rpm)
add_subdirectory(scripts)
add_subdirectory(manpages)

# Examples

add_subdirectory(examples/demoadc)

if(ENABLE_TESTS)

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

endif()

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

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/uid)

    foreach(_uil IN LISTS _uil_list)
        cmake_path(GET _uil STEM LAST_ONLY _stem)
        set(_uid ${CMAKE_CURRENT_BINARY_DIR}/uid/${_stem}.uid)

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

    install(
        DIRECTORY ${CMAKE_BINARY_DIR}/uid
        DESTINATION .
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

###
### Install the remaining files
###

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
    DESTINATION .
    USE_SOURCE_PERMISSIONS
    PATTERN "*.in" EXCLUDE
    PATTERN "Makefile" EXCLUDE
    PATTERN "Makefile.*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
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
    DIRECTORY tdi
    DESTINATION local/tdi
)