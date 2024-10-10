
###
### TdiShr
###

add_library(
    TdiShr
    CvtConvertFloat.c
    DummyRoutines.c
    IsRoprand.c
    TdiAbs.c
    TdiAdd.c
    TdiAnd.c
    TdiApd.c
    TdiArray.c
    TdiBinary.c
    TdiBound.c
    TdiBuild.c
    TdiCall.c
    TdiChar.c
    TdiCompile.c
    TdiCompileDependency.c
    TdiConstant.c
    TdiConvert.c
    TdiCull.c
    TdiCvtArgs.c
    TdiCvtDxDx.c
    TdiDecompile.c
    TdiDecompileDependency.c
    TdiDecompileR.c
    TdiDecompress.c
    TdiDefCat.c
    TdiDefFunction.c
    TdiDim.c
    TdiDivide.c
    TdiDoTask.c
    TdiDtypeRange.c
    TdiEq.c
    TdiEvaluate.c
    TdiExponent.c
    TdiExpt.c
    TdiExtFunction.c
    TdiExtPython.c
    TdiFaultHandler.c
    TdiGetArgs.c
    TdiGetData.c
    TdiGetDbi.c
    TdiGetNci.c
    TdiGetShape.c
    TdiGetSlope.c
    TdiIand.c
    TdiIntrinsic.c
    TdiIo.c
    TdiItoX.c
    TdiMakeFunctionTable.c
    TdiMasterData.c
    TdiMath1.c
    TdiMath2.c
    TdiMatrix.c
    TdiMaxVal.c
    TdiMinMax.c
    TdiOpcDef.c
    TdiPack.c
    TdiPower.c
    TdiReshape.c
    TdiSame.c
    TdiScalar.c
    TdiSetRange.c
    TdiShowVm.c
    TdiSort.c
    TdiSql.c
    TdiSquare.c
    TdiStatement.c
    TdiSubscript.c
    TdiThreadStatic.c
    TdiTrans.c
    TdiTrim.c
    TdiUnary.c
    TdiUndef.c
    TdiVar.c
    TdiVector.c
    TdiXxx.c
    TdiXxxOf.c
    TdiYaccSubs.c
    TdiHash.c
    TdiLex.c
    TdiYacc.c
)

target_include_directories(
    TdiShr
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        TreeShr
)

target_compile_definitions(
    TdiShr
    PRIVATE
        -DLIBPREFIX=Tdi
)

target_link_libraries(
    TdiShr
    PUBLIC
        TreeShr
)

if(TARGET Sybase::Sybase)
    target_link_libraries(
        TdiShr
        PRIVATE
            Sybase::Sybase
    )
endif()

mdsplus_add_static_copy(TdiShr _static_target)

install(TARGETS TdiShr ${_static_target})