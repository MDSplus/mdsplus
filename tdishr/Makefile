# Generated automatically from Makefile.in by configure.
SHELL=/bin/sh
CC=cc
top_srcdir=..
exec_prefix=${prefix}
CFLAGS= -I$(top_srcdir)/include
LDFLAGS=
LIBS=-lresolv -lm -lc 
INSTALL=../conf/install-sh -c

SOURCES = \
CvtConvertFloat.c \
DummyRoutines.c \
IsRoprand.c \
TdiAbs.c \
TdiAdd.c \
TdiAnd.c \
TdiArray.c \
TdiBinary.c \
TdiBound.c \
TdiBuild.c \
TdiCall.c \
TdiChar.c \
TdiCompileDependency.c \
TdiCompile.c \
TdiConstant.c \
TdiConvert.c \
TdiCull.c \
TdiCvtArgs.c \
TdiCvtDxDx.c \
TdiDecompileDependency.c \
TdiDecompileR.c \
TdiDecompile.c \
TdiDecompress.c \
TdiDefCat.c \
TdiDefFunction.c \
TdiDim.c \
TdiDivide.c \
TdiDoTask.c \
TdiDtypeRange.c \
TdiEq.c \
TdiEvaluate.c \
TdiExponent.c \
TdiExpt.c \
TdiExtFunction.c \
TdiFaultHandler.c \
TdiGetArgs.c \
TdiGetData.c \
TdiGetDbi.c \
TdiGetNci.c \
TdiGetShape.c \
TdiGetSlope.c \
TdiHash.c \
TdiIand.c \
TdiIntrinsic.c \
TdiIo.c \
TdiItoX.c \
TdiLex.c \
TdiMakeFunctionTable.c \
TdiMasterData.c \
TdiMath1.c \
TdiMath2.c \
TdiMatrix.c \
TdiMaxVal.c \
TdiMinMax.c \
TdiOpcDef.c \
TdiPack.c \
TdiPower.c \
TdiSame.c \
TdiScalar.c \
TdiSetRange.c \
TdiShowVm.c \
TdiSort.c \
TdiSql.c \
TdiSquare.c \
TdiStatement.c \
TdiSubscript.c \
TdiTrans.c \
TdiTrim.c \
TdiUnary.c \
TdiUndef.c \
TdiVar.c \
TdiVector.c \
TdiXxxOf.c \
TdiXxx.c \
TdiYaccSubs.c \
TdiYacc.c

OBJECTS = $(SOURCES:.c=.o)
EXPORTS = TdiShr.export

all: $(top_srcdir)/lib/ $(top_srcdir)/lib/libTdiShr.so $(top_srcdir)/lib/libTdiShr.a

depend:
	@makedepend -- $(CFLAGS) -- $(SOURCES)

clean:
	- rm *.c
	- rm $(top_srcdir)/lib/libTdiShr.*

install:
	if [ ! -d ${exec_prefix}/lib ] ; then \
                mkdir ${exec_prefix}/lib; \
                chmod 755 ${exec_prefix}/lib; \
        fi;
	$(INSTALL) -m 644  $(top_srcdir)/lib/libTdiShr.a ${exec_prefix}/lib
	$(INSTALL) -m 755  $(top_srcdir)/lib/libTdiShr.so ${exec_prefix}/lib

$(top_srcdir)/lib/libTdiShr.so : $(OBJECTS) 
	$(LD) -o $@ -shared -rpath $(exec_prefix)/lib $(OBJECTS) -L$(top_srcdir)/lib/  -lTreeShr -lMdsShr $(LIBS)

$(top_srcdir)/lib/libTdiShr.a : $(OBJECTS)
	$(AR) -r $@ $(OBJECTS)
	ranlib $@

$(top_srcdir)/lib/ :
	mkdir $(top_srcdir)/lib/

