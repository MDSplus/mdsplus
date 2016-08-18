CLEANFILES = $(java_DATA)
AM_JAVACFLAGS = $(JAVAVERSION)
JAVAROOT = $(builddir)
CLASSPATH_ENV = CLASSPATH=.:$(top_builddir)/javascope/jScope.jar
javadir = $(exec_prefix)/java/classes
java_JAVA = $(SOURCES)

# Build the final jar
java_DATA = mdsobjects.jar

mdsobjects.jar: classjava.stamp
	$(JAR) c0f $@ $(java_JAVA:.java=*.class)


# Giant list of source files follow
SOURCES = MDSplus/Action.java\
MDSplus/Apd.java\
MDSplus/Array.java\
MDSplus/Call.java\
MDSplus/Compound.java\
MDSplus/Condition.java\
MDSplus/Connection.java\
MDSplus/Conglom.java\
MDSplus/Data.java\
MDSplus/DataStreamListener.java\
MDSplus/Dependency.java\
MDSplus/Dictionary.java\
MDSplus/Dimension.java\
MDSplus/Dispatch.java\
MDSplus/Empty.java\
MDSplus/Event.java \
MDSplus/Float32.java\
MDSplus/Float32Array.java\
MDSplus/Float64.java\
MDSplus/Float64Array.java\
MDSplus/Function.java\
MDSplus/GetMany.java\
MDSplus/Ident.java\
MDSplus/Int16.java\
MDSplus/Int16Array.java\
MDSplus/Int32.java\
MDSplus/Int32Array.java\
MDSplus/Int64.java\
MDSplus/Int64Array.java\
MDSplus/Int8.java\
MDSplus/Int8Array.java\
MDSplus/List.java\
MDSplus/MdsException.java\
MDSplus/MdsStreamingDataProvider.java\
MDSplus/Method.java\
MDSplus/Procedure.java\
MDSplus/Program.java\
MDSplus/PutMany.java\
MDSplus/Range.java\
MDSplus/REvent.java\
MDSplus/Routine.java\
MDSplus/Scalar.java\
MDSplus/Signal.java\
MDSplus/String.java\
MDSplus/StringArray.java\
MDSplus/Tree.java\
MDSplus/TreeNode.java\
MDSplus/TreeNodeArray.java\
MDSplus/TreePath.java\
MDSplus/Uint16.java\
MDSplus/Uint16Array.java\
MDSplus/Uint32.java\
MDSplus/Uint32Array.java\
MDSplus/Uint64.java\
MDSplus/Uint64Array.java\
MDSplus/Uint8.java\
MDSplus/Uint8Array.java\
MDSplus/Window.java \
MDSplus/Scope.java

