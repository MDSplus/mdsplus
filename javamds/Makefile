include $(MAKE_OPTIONS)


OBJECTS = javamds.o ServerSupport.o JavaData.o JavaTrav.o
EXPORTS = javamds.export

javamds : 	../shlib ../shlib/libJavaMds.$(SHARE_TYPE) \
		${MDS_PATH}/JavaOpen.fun \
		${MDS_PATH}/JavaClose.fun \
		${MDS_PATH}/JavaResample.fun \
		${MDS_PATH}/JavaDim.fun 
	@ echo javamds done

${MDS_PATH}/JavaOpen.fun : JavaOpen.fun
	cp JavaOpen.fun ${MDS_PATH}/JavaOpen.fun

${MDS_PATH}/JavaClose.fun : JavaClose.fun
	cp JavaClose.fun ${MDS_PATH}/JavaClose.fun

${MDS_PATH}/JavaResample.fun : JavaResample.fun
	cp JavaResample.fun ${MDS_PATH}/JavaResample.fun

${MDS_PATH}/JavaDim.fun : JavaDim.fun
	cp JavaDim.fun ${MDS_PATH}/JavaDim.fun


clean :
	rm $(CLASSES)
	rm *.o
	rm ../shlib/libJavaMds.$(SHARE_TYPE)

.depend :
	echo no depend needed

../shlib/libJavaMds.$(SHARE_TYPE) : $(OBJECTS) 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) $(OBJECTS) -L../shlib -lMdsShr -lTreeShr -lTdiShr -lc $(DLD)

../shlib :
	mkdir ../shlib

clean :
	rm *.o
	rm ../shlib/libMdsShr.$(SHARE_TYPE)
	rm .depend





