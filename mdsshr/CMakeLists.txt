
###
### MdsShr
###

file(GLOB
    _source_list
    "*.c"
)

add_library(
    MdsShr
    ${_source_list}
)

install(TARGETS MdsShr)

target_link_libraries(
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

if(WIN32)
    TARGET_LINK_LIBRARIES(
        MdsShr
        PUBLIC
            iphlpapi
            ws2_32
    )
endif()

add_static_copy(MdsShr)

###
### Documentation
###

# add_custom_target(
#     mdsshr-docs
#     COMMAND ${doxygen_EXECUTABLE} docs/doxy.conf
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
# )

# add_dependencies(
#     docs mdsshr-docs
# )