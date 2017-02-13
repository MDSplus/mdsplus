EXTRA_DIST = servlet-2_3-fcs-classfiles.zip
CLEANFILES = $(java_DATA)

JAVAROOT = $(builddir)
javadir = $(exec_prefix)/java/classes
docsdir = $(exec_prefix)/java/classes/docs

# Create the shell script that runs the program
if MINGW
bin_SCRIPTS = traverser.bat
else
bin_SCRIPTS = jTraverser
CLEANFILES += $(bin_SCRIPTS)
EXTRA_DIST += jTraverser.template
$(bin_SCRIPTS): $(srcdir)/jTraverser.template
	cp $< $@
endif

#$(INSTALL) -D $< $@
# Build the class files
CLASSPATH_ENV = CLASSPATH=$(top_builddir)/javascope/jScope.jar
java_JAVA = $(SOURCES)

# Build the final jar
java_DATA = jTraverser.jar DeviceBeans.jar 


jTraverser.jar: $(GIFS) classjava.stamp
	$(JAR) c0f $@ $(java_JAVA:.java=*.class) $(addprefix -C ${srcdir} ,$(GIFS))

DeviceBeans.jar: classjava.stamp $(GIFS) DeviceBeansManifest.mf
	$(JAR) c0fm $@ ${srcdir}/DeviceBeansManifest.mf $(java_JAVA:.java=*.class) $(addprefix -C ${srcdir} ,$(GIFS))

# Giant list of source files follow

SOURCES = \
	ActionData.java\
	ActionEditor.java\
	ApdData.java\
	ArgEditor.java\
	ArrayData.java\
	AtomicData.java\
	AxisEditor.java\
	ByteArray.java\
	ByteData.java\
	CallData.java\
	CompileTree.java\
	ComplexData.java\
	CompoundData.java\
	ConditionData.java\
	ConglomData.java\
	Convert.java\
	Data.java\
	DataChangeEvent.java\
	DataChangeListener.java\
	DataEditor.java\
	DataId.java\
	DataListener.java\
	Database.java\
	DatabaseException.java\
	DecompileTree.java\
	DependencyData.java\
	DeviceApply.java\
	DeviceButtons.java\
	DeviceCancel.java\
	DeviceChannel.java\
	DeviceChoice.java\
	DeviceCloseListener.java \
	DeviceComponent.java\
	DeviceControl.java\
	DeviceCustomizer.java\
	DeviceDispatch.java\
	DeviceDispatchField.java\
	DeviceField.java\
	DeviceLabel.java\
	DeviceMultiComponent.java\
	DeviceOk.java\
	DeviceParameters.java\
	DeviceParametersBeanInfo.java\
	DeviceReset.java\
	DeviceSetup.java\
	DeviceSetupBeanInfo.java\
	DeviceTable.java\
	DeviceTableBeanInfo.java\
	DeviceTableCustomizer.java\
	DeviceUpdateListener.java \
	DeviceWave.java\
	DeviceWaveDisplay.java\
	DeviceWaveDisplayBeanInfo.java\
	DeviceWaveDisplayCustomizer.java\
	DeviceWaveParameters.java\
	DeviceWaveParametersBeanInfo.java\
	DimensionData.java\
	DispatchData.java\
	DispatchEditor.java\
	DisplayData.java\
	DisplayNci.java\
	DisplayTags.java\
	DoubleArray.java\
	DoubleData.java\
	Editor.java\
	EventData.java\
	ExprEditor.java\
	FloatArray.java\
	FloatData.java\
	FrameRepository.java\
	FunctionData.java\
	IdentData.java\
	IllegalDataException.java\
	IntArray.java\
	IntData.java\
	LabeledExprEditor.java\
	LoadPulse.java \
	MethodData.java\
	MethodEditor.java\
	ModifyData.java\
	NidData.java\
	Node.java\
	NodeBeanInfo.java\
	NodeDataPropertyEditor.java\
	NodeDisplayData.java\
	NodeDisplayNci.java\
	NodeDisplayTags.java\
	NodeEditor.java\
	NodeId.java\
	NodeInfo.java\
	NodeInfoPropertyEditor.java\
	NodeModifyData.java\
	OctaArray.java\
	OctaData.java\
	ParameterData.java\
	ParameterEditor.java\
	PathData.java\
	ProcedureData.java\
	ProcedureEditor.java\
	ProgramData.java\
	ProgramEditor.java\
	PythonEditor.java\
	QuadArray.java\
	QuadData.java\
	RangeData.java\
	RangeEditor.java\
	RemoteTree.java\
	RoutineData.java\
	RoutineEditor.java\
	ShortArray.java\
	ShortData.java\
	SignalData.java\
	SlopeData.java\
	StringArray.java\
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
	DeviceApplyBeanInfo.java \
	DeviceButtonsBeanInfo.java \
	DeviceButtonsCustomizer.java \
	DeviceCancelBeanInfo.java \
	DeviceChannelBeanInfo.java \
	DeviceChannelCustomizer.java \
	DeviceChoiceBeanInfo.java \
	DeviceChoiceCustomizer.java \
	DeviceDispatchBeanInfo.java \
	DeviceFieldBeanInfo.java \
	DeviceFieldCustomizer.java \
	DeviceLabelBeanInfo.java \
	DeviceLabelCustomizer.java \
	DeviceOkBeanInfo.java \
	DeviceResetBeanInfo.java \
	DeviceTableBeanInfo.java \
	DeviceTableCustomizer.java \
	DeviceWaveBeanInfo.java \
	DeviceWaveCustomizer.java \
	FloatArrayEditor.java \
	IntArrayEditor.java \
	LoadFile.java \
	StoreFile.java

GIFS = \
	DeviceApply.gif    \
	DeviceButtons.gif  \
	DeviceCancel.gif   \
	DeviceChannel.gif  \
	DeviceChoice.gif   \
	DeviceDispatch.gif \
	DeviceField.gif    \
	DeviceOk.gif       \
	DeviceReset.gif    \
	DeviceSetup.gif    \
	action.gif         \
	axis.gif           \
	compound.gif       \
	device.gif         \
	dispatch.gif       \
	numeric.gif        \
	signal.gif         \
	structure.gif      \
	subtree.gif        \
	task.gif           \
	text.gif           \
	window.gif

