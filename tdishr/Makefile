CFLAGS = -c -I../include -I. -g -migrate -assume noaligned_objects -ieee_with_inexact

OBJECTS = \
CvtConvertFloat.o \
DummyRoutines.o \
IsRoprand.o \
TdiAbs.o \
TdiAdd.o \
TdiAnd.o \
TdiArray.o \
TdiBinary.o \
TdiBound.o \
TdiBuild.o \
TdiCall.o \
TdiChar.o \
TdiCompileDependency.o \
TdiCompile.o \
TdiConstant.o \
TdiConvert.o \
TdiCull.o \
TdiCvtArgs.o \
TdiCvtDxDx.o \
TdiDecompileDependency.o \
TdiDecompileR.o \
TdiDecompile.o \
TdiDecompress.o \
TdiDefCat.o \
TdiDefFunction.o \
TdiDim.o \
TdiDivide.o \
TdiDoTask.o \
TdiDtypeRange.o \
TdiEq.o \
TdiEvaluate.o \
TdiExponent.o \
TdiExpt.o \
TdiExtFunction.o \
TdiFaultHandler.o \
TdiGetArgs.o \
TdiGetData.o \
TdiGetDbi.o \
TdiGetNci.o \
TdiGetShape.o \
TdiGetSlope.o \
TdiHash.o \
TdiIand.o \
TdiIntrinsic.o \
TdiIo.o \
TdiItoX.o \
TdiLex.o \
TdiMakeFunctionTable.o \
TdiMasterData.o \
TdiMath1.o \
TdiMath2.o \
TdiMatrix.o \
TdiMaxVal.o \
TdiMinMax.o \
TdiOpcDef.o \
TdiPack.o \
TdiPower.o \
TdiSame.o \
TdiScalar.o \
TdiSetRange.o \
TdiShowVm.o \
TdiSort.o \
TdiSql.o \
TdiSquare.o \
TdiStatement.o \
TdiSubscript.o \
TdiTrans.o \
TdiTrim.o \
TdiUnary.o \
TdiUndef.o \
TdiVar.o \
TdiVector.o \
TdiXxxOf.o \
TdiXxx.o \
TdiYaccSubs.o \
TdiYacc.o

libTdiShr.so : $(OBJECTS)
	ld -o $@ -shared -no_archive $(OBJECTS) -lc -lm -L../mdsshr -lMdsShr -lots -L../treeshr -lTreeShr

clean :
	- rm *.o
	- rm *.so
	- rm so_locations

TdiLex.c : TdiLex.x
	lex -t TdiLex.x > $@

tdiyacc.h : TdiYacc.y
	yacc -d TdiYacc.y ; mv y.tab.h $@

CvtConvertFloat.o : /mdsport/include/mdsdescrip.h
IsRoprand.o : /mdsport/include/mdsdescrip.h
TdiAbs.o : /mdsport/include/mdsdescrip.h
TdiAbs.o : /mdsport/include/mdsdescrip.h
TdiAbs.o : /mdsport/include/tdimessages.h
TdiAdd.o : /mdsport/include/mdsdescrip.h
TdiAdd.o : /mdsport/include/mdsdescrip.h
TdiAdd.o : /mdsport/include/tdimessages.h
TdiAnd.o : /mdsport/include/mdsdescrip.h
TdiAnd.o : /mdsport/include/tdimessages.h
TdiArray.o : /mdsport/include/tdimessages.h
TdiArray.o : /mdsport/include/mdsshr.h
TdiArray.o : tdinelements.h
TdiArray.o : tdirefcat.h
TdiArray.o : tdireffunction.h
TdiArray.o : tdirefstandard.h
TdiBinary.o : /mdsport/include/mdsdescrip.h
TdiBinary.o : /mdsport/include/tdimessages.h
TdiBound.o : /mdsport/include/tdimessages.h
TdiBound.o : /mdsport/include/mdsshr.h
TdiBound.o : tdirefcat.h
TdiBound.o : tdireffunction.h
TdiBound.o : tdirefstandard.h
TdiBuild.o : tdirefcat.h
TdiBuild.o : tdireffunction.h
TdiBuild.o : tdirefstandard.h
TdiBuild.o : /mdsport/include/tdimessages.h
TdiBuild.o : /mdsport/include/mdsshr.h
TdiCall.o : tdirefcat.h
TdiCall.o : tdirefstandard.h
TdiCall.o : /mdsport/include/libroutines.h
TdiCall.o : /mdsport/include/strroutines.h
TdiCall.o : /mdsport/include/tdimessages.h
TdiCall.o : /mdsport/include/mdsshr.h
TdiChar.o : /mdsport/include/mdsdescrip.h
TdiChar.o : /mdsport/include/mdsdescrip.h
TdiChar.o : tdinelements.h
TdiChar.o : /mdsport/include/libroutines.h
TdiChar.o : /mdsport/include/strroutines.h
TdiChar.o : /mdsport/include/tdimessages.h
TdiCompile.o : tdirefzone.h
TdiCompile.o : tdirefstandard.h
TdiCompile.o : /mdsport/include/libroutines.h
TdiCompile.o : /mdsport/include/tdimessages.h
TdiCompile.o : /mdsport/include/mdsshr.h
TdiCompileDependency.o : tdirefstandard.h
TdiConstant.o : tdireffunction.h
TdiConstant.o : tdirefstandard.h
TdiConstant.o : /mdsport/include/mdsshr.h
TdiConvert.o : /mdsport/include/mdsdescrip.h
TdiConvert.o : /mdsport/include/mdsdescrip.h
TdiConvert.o : /mdsport/include/tdimessages.h
TdiCull.o : tdirefcat.h
TdiCull.o : tdirefstandard.h
TdiCull.o : tdinelements.h
TdiCull.o : /mdsport/include/tdimessages.h
TdiCull.o : /mdsport/include/mdsshr.h
TdiCvtArgs.o : tdirefcat.h
TdiCvtArgs.o : tdirefstandard.h
TdiCvtArgs.o : /mdsport/include/tdimessages.h
TdiCvtArgs.o : /mdsport/include/mdsshr.h
TdiCvtDxDx.o : /mdsport/include/mdsdescrip.h
TdiCvtDxDx.o : /mdsport/include/mdsdescrip.h
TdiCvtDxDx.o : /mdsport/include/mdsshr.h
TdiDecompile.o : /mdsport/include/tdimessages.h
TdiDecompile.o : tdirefcat.h
TdiDecompile.o : tdirefstandard.h
TdiDecompile.o : /mdsport/include/strroutines.h
TdiDecompile.o : /mdsport/include/tdimessages.h
TdiDecompile.o : /mdsport/include/mdsshr.h
TdiDecompile.o : /mdsport/include/treeshr.h
TdiDecompile.o : /mdsport/include/mds_stdarg.h
TdiDecompileDependency.o : tdirefstandard.h
TdiDecompileDependency.o : /mdsport/include/strroutines.h
TdiDecompileDependency.o : /mdsport/include/tdimessages.h
TdiDecompileDependency.o : /mdsport/include/mdsshr.h
TdiDecompileDependency.o : /mdsport/include/treeshr.h
TdiDecompileDependency.o : /mdsport/include/mds_stdarg.h
TdiDecompileR.o : /mdsport/include/mdsdescrip.h
TdiDecompileR.o : /mdsport/include/mdsdescrip.h
TdiDecompileR.o : opcopcodes.h
TdiDecompileR.o : tdirefcat.h
TdiDecompileR.o : tdireffunction.h
TdiDecompileR.o : tdirefzone.h
TdiDecompileR.o : /mdsport/include/strroutines.h
TdiDecompileR.o : /mdsport/include/mds_stdarg.h
TdiDecompress.o : tdirefstandard.h
TdiDecompress.o : tdirefcat.h
TdiDecompress.o : /mdsport/include/mdsshr.h
TdiDefCat.o : tdirefcat.h
TdiDefFunction.o : /mdsport/include/mds_stdarg.h
TdiDefFunction.o : /mdsport/include/libroutines.h
TdiDefFunction.o : /mdsport/include/tdimessages.h
TdiDefFunction.o : /mdsport/include/mdsdescrip.h
TdiDefFunction.o : /mdsport/include/mdsdescrip.h
TdiDefFunction.o : opcbuiltins.h
TdiDim.o : /mdsport/include/mdsdescrip.h
TdiDim.o : /mdsport/include/mdsdescrip.h
TdiDim.o : /mdsport/include/tdimessages.h
TdiDivide.o : /mdsport/include/mdsdescrip.h
TdiDivide.o : /mdsport/include/mdsdescrip.h
TdiDivide.o : /mdsport/include/tdimessages.h
TdiDoTask.o : /mdsport/include/mdsdescrip.h
TdiDoTask.o : /mdsport/include/mdsdescrip.h
TdiDoTask.o : tdirefstandard.h
TdiDoTask.o : /mdsport/include/libroutines.h
TdiDoTask.o : /mdsport/include/strroutines.h
TdiDoTask.o : /mdsport/include/tdimessages.h
TdiDoTask.o : /mdsport/include/mdsshr.h
TdiDoTask.o : /mdsport/include/treeshr.h
TdiDtypeRange.o : tdinelements.h
TdiDtypeRange.o : tdirefcat.h
TdiDtypeRange.o : tdirefstandard.h
TdiDtypeRange.o : /mdsport/include/tdimessages.h
TdiDtypeRange.o : /mdsport/include/mdsshr.h
TdiEq.o : /mdsport/include/mdsdescrip.h
TdiEq.o : /mdsport/include/mdsdescrip.h
TdiEq.o : /mdsport/include/tdimessages.h
TdiEvaluate.o : tdirefstandard.h
TdiEvaluate.o : /mdsport/include/tdimessages.h
TdiEvaluate.o : /mdsport/include/mdsshr.h
TdiEvaluate.o : /mdsport/include/treeshr.h
TdiExponent.o : /mdsport/include/mdsdescrip.h
TdiExponent.o : /mdsport/include/mdsdescrip.h
TdiExponent.o : tdinelements.h
TdiExponent.o : /mdsport/include/tdimessages.h
TdiExpt.o : /mdsport/include/mdsdescrip.h
TdiExpt.o : /mdsport/include/mdsdescrip.h
TdiExpt.o : /mdsport/include/dbidef.h
TdiExpt.o : /mdsport/include/libroutines.h
TdiExpt.o : /mdsport/include/mdsshr.h
TdiExpt.o : /mdsport/include/treeshr.h
TdiExtFunction.o : tdirefstandard.h
TdiExtFunction.o : /mdsport/include/libroutines.h
TdiExtFunction.o : /mdsport/include/strroutines.h
TdiExtFunction.o : /mdsport/include/tdimessages.h
TdiExtFunction.o : /mdsport/include/mdsshr.h
TdiExtFunction.o : /mdsport/include/mds_stdarg.h
TdiGetArgs.o : /mdsport/include/tdimessages.h
TdiGetArgs.o : /mdsport/include/mdsshr.h
TdiGetArgs.o : tdirefcat.h
TdiGetArgs.o : tdireffunction.h
TdiGetArgs.o : tdirefstandard.h
TdiGetData.o : tdirefcat.h
TdiGetData.o : tdirefstandard.h
TdiGetData.o : /mdsport/include/tdimessages.h
TdiGetData.o : /mdsport/include/mdsshr.h
TdiGetData.o : /mdsport/include/treeshr.h
TdiGetDbi.o : tdirefstandard.h
TdiGetDbi.o : /mdsport/include/dbidef.h
TdiGetDbi.o : /mdsport/include/strroutines.h
TdiGetDbi.o : /mdsport/include/tdimessages.h
TdiGetDbi.o : /mdsport/include/treeshr.h
TdiGetDbi.o : /mdsport/include/mdsshr.h
TdiGetNci.o : tdirefstandard.h
TdiGetNci.o : tdinelements.h
TdiGetNci.o : /mdsport/include/ncidef.h
TdiGetNci.o : /mdsport/include/usagedef.h
TdiGetNci.o : /mdsport/include/strroutines.h
TdiGetNci.o : /mdsport/include/tdimessages.h
TdiGetNci.o : /mdsport/include/treeshr.h
TdiGetNci.o : /mdsport/include/mdsshr.h
TdiGetShape.o : /mdsport/include/tdimessages.h
TdiGetShape.o : /mdsport/include/mdsdescrip.h
TdiGetShape.o : /mdsport/include/mdsdescrip.h
TdiGetShape.o : tdirefcat.h
TdiGetShape.o : /mdsport/include/mdsshr.h
TdiGetSlope.o : /mdsport/include/tdimessages.h
TdiGetSlope.o : tdirefstandard.h
TdiGetSlope.o : /mdsport/include/mdsshr.h
TdiHash.o : tdireffunction.h
TdiIand.o : /mdsport/include/mdsdescrip.h
TdiIand.o : /mdsport/include/mdsdescrip.h
TdiIand.o : /mdsport/include/tdimessages.h
TdiIntrinsic.o : tdirefcat.h
TdiIntrinsic.o : tdireffunction.h
TdiIntrinsic.o : tdirefstandard.h
TdiIntrinsic.o : tdirefzone.h
TdiIntrinsic.o : /mdsport/include/strroutines.h
TdiIntrinsic.o : /mdsport/include/libroutines.h
TdiIntrinsic.o : /mdsport/include/tdimessages.h
TdiIntrinsic.o : /mdsport/include/mdsshr.h
TdiIntrinsic.o : /mdsport/include/mds_stdarg.h
TdiIo.o : tdirefstandard.h
TdiIo.o : /mdsport/include/libroutines.h
TdiIo.o : /mdsport/include/strroutines.h
TdiIo.o : /mdsport/include/mdsshr.h
TdiItoX.o : tdirefcat.h
TdiItoX.o : tdirefstandard.h
TdiItoX.o : tdinelements.h
TdiItoX.o : /mdsport/include/tdimessages.h
TdiItoX.o : /mdsport/include/mdsshr.h
TdiLex.o : /mdsport/include/mdsdescrip.h
TdiLex.o : /mdsport/include/mdsdescrip.h
TdiLex.o : tdirefzone.h
TdiLex.o : tdireffunction.h
TdiLex.o : /mdsport/include/strroutines.h
TdiLex.o : /mdsport/include/tdimessages.h
TdiLex.o : /mdsport/include/treeshr.h
TdiMakeFunctionTable.o : /mdsport/include/mdsdescrip.h
TdiMakeFunctionTable.o : /mdsport/include/mdsdescrip.h
TdiMakeFunctionTable.o : opcbuiltins.h
TdiMakeFunctionTable.o : tdirefzone.h
TdiMakeFunctionTable.o : tdiyacc.h
TdiMakeFunctionTable.o : tdireffunction.h
TdiMakeFunctionTable.o : opcbuiltins.h
TdiMasterData.o : tdirefstandard.h
TdiMasterData.o : /mdsport/include/mdsshr.h
TdiMath1.o : /mdsport/include/mdsdescrip.h
TdiMath1.o : /mdsport/include/mdsdescrip.h
TdiMath1.o : /mdsport/include/tdimessages.h
TdiMath2.o : /mdsport/include/mdsdescrip.h
TdiMath2.o : /mdsport/include/mdsdescrip.h
TdiMath2.o : /mdsport/include/tdimessages.h
TdiMatrix.o : tdirefstandard.h
TdiMatrix.o : tdirefcat.h
TdiMatrix.o : tdinelements.h
TdiMatrix.o : /mdsport/include/tdimessages.h
TdiMatrix.o : /mdsport/include/mdsshr.h
TdiMaxVal.o : /mdsport/include/mdsdescrip.h
TdiMaxVal.o : /mdsport/include/mdsdescrip.h
TdiMaxVal.o : /mdsport/include/tdimessages.h
TdiMinMax.o : tdirefstandard.h
TdiMinMax.o : /mdsport/include/mdsshr.h
TdiOpcDef.o : opcbuiltins.h
TdiPack.o : tdirefcat.h
TdiPack.o : tdirefstandard.h
TdiPack.o : /mdsport/include/tdimessages.h
TdiPack.o : /mdsport/include/mdsshr.h
TdiPower.o : /mdsport/include/mdsdescrip.h
TdiPower.o : /mdsport/include/mdsdescrip.h
TdiPower.o : tdinelements.h
TdiPower.o : /mdsport/include/tdimessages.h
TdiSame.o : /mdsport/include/mdsshr.h
TdiSame.o : tdirefcat.h
TdiSame.o : tdireffunction.h
TdiSame.o : tdirefstandard.h
TdiSame.o : /mdsport/include/libroutines.h
TdiScalar.o : tdirefcat.h
TdiScalar.o : tdireffunction.h
TdiScalar.o : tdirefstandard.h
TdiScalar.o : tdinelements.h
TdiScalar.o : /mdsport/include/tdimessages.h
TdiScalar.o : /mdsport/include/mdsshr.h
TdiSetRange.o : tdirefcat.h
TdiSetRange.o : tdirefstandard.h
TdiSetRange.o : /mdsport/include/tdimessages.h
TdiSetRange.o : /mdsport/include/mdsshr.h
TdiShowVm.o : tdirefstandard.h
TdiShowVm.o : /mdsport/include/libroutines.h
TdiShowVm.o : /mdsport/include/librtl_messages.h
TdiSort.o : tdinelements.h
TdiSort.o : tdirefcat.h
TdiSort.o : tdirefstandard.h
TdiSort.o : /mdsport/include/strroutines.h
TdiSort.o : /mdsport/include/tdimessages.h
TdiSort.o : /mdsport/include/mdsshr.h
TdiSql.o : /mdsport/include/mdsdescrip.h
TdiSql.o : /mdsport/include/mdsdescrip.h
TdiSql.o : tdirefstandard.h
TdiSql.o : /mdsport/include/strroutines.h
TdiSql.o : /mdsport/include/libroutines.h
TdiSql.o : /mdsport/include/tdimessages.h
TdiSql.o : /mdsport/include/mdsshr.h
TdiSql.o : /mdsport/include/mds_stdarg.h
TdiSquare.o : /mdsport/include/mdsdescrip.h
TdiStatement.o : tdirefstandard.h
TdiStatement.o : /mdsport/include/strroutines.h
TdiStatement.o : /mdsport/include/tdimessages.h
TdiStatement.o : /mdsport/include/mdsshr.h
TdiSubscript.o : tdirefcat.h
TdiSubscript.o : tdirefstandard.h
TdiSubscript.o : tdinelements.h
TdiSubscript.o : /mdsport/include/tdimessages.h
TdiSubscript.o : /mdsport/include/mdsshr.h
TdiTrans.o : tdinelements.h
TdiTrans.o : tdirefcat.h
TdiTrans.o : tdireffunction.h
TdiTrans.o : tdirefstandard.h
TdiTrans.o : /mdsport/include/tdimessages.h
TdiTrans.o : /mdsport/include/mdsshr.h
TdiTrim.o : tdinelements.h
TdiTrim.o : tdirefcat.h
TdiTrim.o : tdireffunction.h
TdiTrim.o : tdirefstandard.h
TdiTrim.o : /mdsport/include/strroutines.h
TdiTrim.o : /mdsport/include/tdimessages.h
TdiTrim.o : /mdsport/include/mdsshr.h
TdiTrim.o : /mdsport/include/mds_stdarg.h
TdiUnary.o : /mdsport/include/mdsdescrip.h
TdiUnary.o : /mdsport/include/tdimessages.h
TdiUndef.o : /mdsport/include/tdimessages.h
TdiVar.o : tdirefstandard.h
TdiVar.o : /mdsport/include/libroutines.h
TdiVar.o : /mdsport/include/strroutines.h
TdiVar.o : /mdsport/include/tdimessages.h
TdiVar.o : /mdsport/include/mdsshr.h
TdiVar.o : /mdsport/include/librtl_messages.h
TdiVector.o : tdirefcat.h
TdiVector.o : tdirefstandard.h
TdiVector.o : tdinelements.h
TdiVector.o : /mdsport/include/libroutines.h
TdiVector.o : /mdsport/include/tdimessages.h
TdiVector.o : /mdsport/include/mdsshr.h
TdiXxx.o : tdirefcat.h
TdiXxx.o : tdirefstandard.h
TdiXxx.o : /mdsport/include/strroutines.h
TdiXxx.o : /mdsport/include/tdimessages.h
TdiXxx.o : /mdsport/include/mdsshr.h
TdiXxxOf.o : /mdsport/include/tdimessages.h
TdiXxxOf.o : tdirefstandard.h
TdiXxxOf.o : /mdsport/include/mdsshr.h
TdiYacc.o : /mdsport/include/mdsdescrip.h
TdiYacc.o : /mdsport/include/mdsdescrip.h
TdiYacc.o : tdirefcat.h
TdiYacc.o : tdireffunction.h
TdiYacc.o : tdirefzone.h
TdiYacc.o : /mdsport/include/libroutines.h
TdiYacc.o : /mdsport/include/strroutines.h
TdiYacc.o : /mdsport/include/tdimessages.h
TdiYacc.o : /mdsport/include/mds_stdarg.h
TdiYaccSubs.o : tdirefcat.h
TdiYaccSubs.o : tdireffunction.h
TdiYaccSubs.o : tdirefstandard.h
TdiYaccSubs.o : tdirefzone.h
TdiYaccSubs.o : /mdsport/include/libroutines.h
TdiYaccSubs.o : /mdsport/include/tdimessages.h
TdiYaccSubs.o : /mdsport/include/mdsshr.h
t.o : /mdsport/include/dlfcn.h
tdi$intrinsic.o : TDI$$REF_CAT.H
tdi$intrinsic.o : TDI$$REF_FUNCTION.H
tdi$intrinsic.o : TDI$$REF_STANDARD.H
tdi$intrinsic.o : TDI$$REF_ZONE.H
tdi$intrinsic.o : /mdsport/include/str$routines.h
tdi$intrinsic.o : /mdsport/include/lib$routines.h
tdi$intrinsic.o : /mdsport/include/tdi$messages.h
tdi$intrinsic.o : /mdsport/include/mdsshr.h
tdi$intrinsic.o : /mdsport/include/mds_stdarg.h









