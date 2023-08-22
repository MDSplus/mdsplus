
###
### TreeShr
###

# TODO: Investigate
# find_program(_sed_EXECUTABLE sed)
#
# if(_sed_EXECUTABLE)
#     message("${_sed_EXECUTABLE} -i 's/\\(size_t\\|int\)\\ n\\;\\s*\\/typeof(max_size) n\\; \\\\/g' ${_lex_generated_source_list}")
#     execute_process(
#         COMMAND "${_sed_EXECUTABLE} -i 's/\\(size_t\\|int\)\\ n\\;\\s*\\/typeof(max_size) n\\; \\\\/g' ${_lex_generated_source_list}"
#     )
# endif()

add_library(
    TreeShr
    dummies.c
    RemoteAccess.c
    TreeAddNode.c
    TreeAddTag.c
    TreeCallHook.c
    TreeCleanDatafile.c
    TreeCreatePulseFile.c
    TreeDeleteNode.c
    TreeDeletePulseFile.c
    TreeDoMethod.c
    TreeFindNode.c
    TreeFindTag.c
    TreeFindTagWild.c
    TreeGetDbi.c
    TreeGetNci.c
    TreeGetRecord.c
    TreeGetSetShotId.c
    TreeOpen.c
    TreePerf.c
    TreePutRecord.c
    TreeRemoveNodesTags.c
    TreeRenameNode.c
    TreeSegments.c
    TreeSerializeNci.c
    TreeSetDbi.c
    TreeSetDefault.c
    TreeSetNci.c
    TreeThreadStatic.c
    TreeVerify.c
    TreeFindNodeWild.c
)

target_include_directories(
    TreeShr
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
    TreeShr
    PUBLIC
        MdsShr
)

target_compile_definitions(
    TreeShr
    PRIVATE
        -DLIBPREFIX=Tree
)

mdsplus_add_static_copy(TreeShr _static_target)

install(TARGETS TreeShr ${_static_target})