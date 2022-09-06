
###
### MdsShr
###

FILE(GLOB
    _source_list
    "*.c"
)

ADD_LIBRARY(
    MdsShr SHARED
    ${_source_list}
)

INSTALL(TARGETS MdsShr)

TARGET_LINK_LIBRARIES(
    MdsShr
    PUBLIC
        ${LIBM}
        ${LIBDL}
        ${LIBZ}
        ${LIBLZMA}
        libxml2::libxml2
        Iconv::Iconv
        Threads::Threads
)

IF(WIN32)
    TARGET_LINK_LIBRARIES(
        MdsShr
        PUBLIC
            iphlpapi
            ws2_32
    )
ENDIF()

ADD_STATIC_COPY(MdsShr)

###
### Documentation
###

# ADD_CUSTOM_TARGET(
#     mdsshr-docs
#     COMMAND ${DOXYGEN_PROGRAM} docs/doxy.conf
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
# )

# ADD_DEPENDENCIES(
#     docs mdsshr-docs
# )