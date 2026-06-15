
# TODO: Remove this library

###
### MdsTestUtils
###

add_library(
    MdsTestUtils
    CommaInit.h
    FunctionTypes.h
    Singleton.h
    testutils.h
    type_traits.h
    unique_ptr.h
    vector_test.h
    MdsIpInstancer.h
    testutils.cpp
    mdsipmain.c
)

target_link_libraries(
    MdsTestUtils
    PUBLIC
        MdsObjectsCppShr
)

mdsplus_add_static_copy(MdsTestUtils _static_target)

# install(TARGETS MdsTestUtils ${_static_target})
