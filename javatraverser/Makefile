include $(MAKE_OPTIONS)

JFLAGS = -g   -d ../java/classes

CLASSES = ../java/classes/*.java


EXPORTS = javamds.export

javatraverser : ../shlib/libJavaTrav.$(SHARE_TYPE) \
		../java ../java/classes ../java/classes/jTraverser.class 
	@ echo javaTraverser done


../java : ; mkdir ../java

../java/classes : ; mkdir ../java/classes

../java/classes/jTraverser.class :ActionData.java \
               			  ActionEditor.java              DispatchEditor.java            NodeDataPropertyEditor.java    RoutineData.java\
				  ApdData.java                   DisplayData.java               NodeDisplayData.java           RoutineEditor.java\
				  ArgEditor.java                 DisplayNci.java                NodeDisplayNci.java            ShortArray.java\
				  ArrayData.java                 DoubleArray.java               NodeEditor.java                ShortData.java\
				  AtomicData.java                DoubleData.java                NodeId.java                    SignalData.java\
				  AxisEditor.java                Editor.java                    NodeInfo.java                  StringData.java\
				  ByteArray.java                 EventData.java                 NodeInfoPropertyEditor.java    SyntaxException.java\
				  ByteData.java                  ExprEditor.java                NodeModifyData.java            TaskEditor.java\
				  CallData.java                  FloatArray.java                NodePropertyEditor.java        Tree.java\
				  CompoundData.java              FloatData.java                 OctaArray.java                 TreeDialog.java\
				  ConditionData.java             FunctionData.java              OctaData.java                  TreeNode.java\
				  ConglomData.java               IllegalDataException.java      ParameterData.java             UnsupportedDataException.java\
				  Data.java                      IntArray.java                  PathData.java                  WindowData.java\
				  DataEditor.java                IntData.java                   ProcedureData.java             WindowEditor.java\
				  DataId.java                    LabeledExprEditor.java         ProcedureEditor.java           WithErrorData.java\
				  DataListener.java              MethodData.java                ProgramData.java               WithUnitsData.java\
				  Database.java                  MethodEditor.java              ProgramEditor.java             jTraverser.java\
				  DatabaseException.java         ModifyData.java                QuadArray.java\
				  DependencyData.java            NidData.java                   QuadData.java\
				  DimensionData.java             Node.java                      RangeData.java
	javac $(JFLAGS) *.java
	cp *.gif ../java/classes



clean :
	rm $(CLASSES)
	rm *.o
	rm ../shlib/libJavaData.$(SHARE_TYPE)
	rm ../shlib/libJavaTrav.$(SHARE_TYPE)

.depend :
	echo no depend needed


../shlib/libJavaTrav.$(SHARE_TYPE) :  ../shlib ../shlib/libJavaData.$(SHARE_TYPE) JavaTrav.o 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) JavaTrav.o -L../shlib -lMdsShr -lTreeShr -lTdiShr -lJavaData -lc $(DLD)

../shlib/libJavaData.$(SHARE_TYPE) : JavaData.o 
	$(LD) -o $@ $(LD_SHARED_LIB_OPTIONS) JavaData.o -L../shlib -lMdsShr -lTreeShr -lTdiShr -lc $(DLD)
./shlib :
	mkdir ../shlib





