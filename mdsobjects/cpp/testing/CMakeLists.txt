
###
### MdsTestUtils
###

add_library(
    MdsTestUtils
    testutils/CommaInit.h
    testutils/FunctionTypes.h
    testutils/Singleton.h
    testutils/testutils.h
    testutils/type_traits.h
    testutils/unique_ptr.h
    testutils/vector_test.h
    testutils/MdsIpInstancer.h
    testutils/testutils.cpp
    testutils/mdsipmain.c
)

target_link_libraries(
    MdsTestUtils
    PUBLIC
        MdsObjectsCppShr
)

target_include_directories(
    MdsTestUtils
    PUBLIC
        # TODO: Fix
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

###
### Testing
###

set(_test_source_list
    buildtest.cpp
    MdsExpressionCompileTest.cpp
    MdsExceptionTest.cpp
    MdsDataTest.cpp
    MdsTdiTest.cpp
    MdsScalarTest_NumericConversion.cpp
    MdsScalarTest_NumericLimits.cpp
    MdsScalarTest_ScalarCast.cpp
    MdsStringTest.cpp
    MdsCompoundTest.cpp
    MdsSignalTest.cpp
    MdsDimensionTest.cpp
    MdsWindowTest.cpp
    MdsFunctionTest.cpp
    MdsConglomTest.cpp
    MdsRangeTest.cpp
    MdsTreeSuppression.cpp
    MdsTreeTest.cpp
    MdsTreeNodeTest.cpp
    MdsTreeSegments.cpp
    MdsEventSuppression.cpp
    MdsEventTest.cpp
    MdsConnectionTest.cpp

    # SKIP_TEST is not supported
    # MdsCallTest.cpp

    # These tests are't actually written yet
    # MdsActionTest.cpp
    # MdsApdTest.cpp
)

# TODO: Remove
set(_env_mods 
    "srcdir=set:${CMAKE_CURRENT_SOURCE_DIR}"
)

list(APPEND Valgrind_SUPPRESSION_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/valgrind.supp
)

foreach(_test_source ${_test_source_list})

    cmake_path(GET _test_source STEM _test_name)

    add_executable(
        ${_test_name}
        ${_test_source}
    )

    target_link_libraries(
        ${_test_name}
        PRIVATE
            MdsObjectsCppShr
            MdsTestShr
            MdsTestUtils
    )

    mdsplus_add_test(
        NAME ${_test_name}
        COMMAND $<TARGET_FILE:${_test_name}> ${_argument}
        ENVIRONMENT_MODIFICATION ${_env_mods}
    )

endforeach()
