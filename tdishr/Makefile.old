include $(MAKE_OPTIONS)

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

EXPORTS = TdiShr.export

tdishr : ../shlib ../shlib/libTdiShr.$(SHARE_TYPE) ../shlib/libTdiShr.a

../shlib/libTdiShr.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -lm -L../shlib -lMdsShr -lTreeShr

../shlib/libTdiShr.a : $(OBJECTS)
	$(AR) -r $@ $(OBJECTS)

../shlib :
	mkdir ../shlib

clean :
	- rm *.o
	- rm ../shlib/libTdiShr.*
	- rm .depend

#TdiLex.c : TdiLex.x
#	lex -t TdiLex.x > $@

#tdiyacc.h : TdiYacc.y
#	yacc -d TdiYacc.y ; mv y.tab.h $@

.depend :
	find *.c -exec $(MAKE_DEPEND)

include $(DEPEND)
