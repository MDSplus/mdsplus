# Generated automatically from Makefile.in by configure.
SHELL=/bin/sh
top_srcdir=..
JAVADIR = $(top_srcdir)/java/classes
JAVAC = /usr/bin/javac
JAR = /usr/bin/jar

SOURCES = ActionData.java\
ActionEditor.java\
ApdData.java\
ArgEditor.java\
ArrayData.java\
AtomicData.java\
AxisEditor.java\
ByteArray.java\
ByteData.java\
CallData.java\
CompoundData.java\
ConditionData.java\
ConglomData.java\
Data.java\
DataEditor.java\
DataId.java\
DataListener.java\
Database.java\
DatabaseException.java\
DependencyData.java\
DimensionData.java\
DispatchData.java\
DispatchEditor.java\
DisplayData.java\
DisplayNci.java\
DoubleArray.java\
DoubleData.java\
Editor.java\
EventData.java\
ExprEditor.java\
FloatArray.java\
FloatData.java\
FunctionData.java\
IllegalDataException.java\
IntArray.java\
IntData.java\
LabeledExprEditor.java\
MethodData.java\
MethodEditor.java\
ModifyData.java\
NidData.java\
Node.java\
NodeBeanInfo.java\
NodeDataPropertyEditor.java\
NodeDisplayData.java\
NodeDisplayNci.java\
NodeEditor.java\
NodeId.java\
NodeInfo.java\
NodeInfoPropertyEditor.java\
NodeModifyData.java\
NodePropertyEditor.java\
OctaArray.java\
OctaData.java\
ParameterData.java\
PathData.java\
ProcedureData.java\
ProcedureEditor.java\
ProgramData.java\
ProgramEditor.java\
QuadArray.java\
QuadData.java\
RangeData.java\
RangeEditor.java\
RoutineData.java\
RoutineEditor.java\
ShortArray.java\
ShortData.java\
SignalData.java\
StringData.java\
SyntaxException.java\
TaskEditor.java\
Tree.java\
TreeDialog.java\
TreeNode.java\
UnsupportedDataException.java\
WindowData.java\
WindowEditor.java\
WithErrorData.java\
WithUnitsData.java\
jTraverser.java\

CLASSES = *.class

all: $(top_srcdir)/bin/ $(top_srcdir)/bin/jTraverser $(top_srcdir)/java/ $(top_srcdir)/java/classes \
     $(top_srcdir)/java/classes/jTraverser.jar

clean: ; rm $(CLASSES) $(top_srcdir)/bin/jTraverser $(top_srcdir)/java/classes/jTraverser.jar

depend: ; @ :

$(top_srcdir)/java/ : ; mkdir $(top_srcdir)/java/

$(top_srcdir)/java/classes : ; mkdir $(top_srcdir)/java/classes

$(top_srcdir)/java/classes/jTraverser.jar: jTraverser.class
	$(JAR) c0f $@ $(CLASSES)

jTraverser.class: 	$(SOURCES)
	env CLASSPATH=$(top_srcdir)/java/classes/MRJClasses.zip $(JAVAC) -g $(SOURCES)

$(top_srcdir)/bin/ : ; mkdir $(top_srcdir)/bin/

$(top_srcdir)/bin/jTraverser : jTraverser.template
	sed "s|MAKEJAVADIR|`cd $(top_srcdir)/java/;pwd`|g" jTraverser.template | sed "s|libdir|`cd ${exec_prefix}/lib;pwd`|g" > $@
	chmod a+x $@
