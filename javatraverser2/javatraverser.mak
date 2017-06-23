JARDIR = ../java/classes
CLASSPATH = -classpath .;$(JARDIR)/mdsDataProvider.jar
JAVAC = "$(JDK_DIR)/bin/javac.exe"
JAR = "$(JDK_DIR)/bin/jar.exe"

.SUFFIXES: .class .java
.java.class:
	$(JAVAC) $*.java

DEVICE_SRC= \
  devicebeans/Database.java \
  devicebeans/DeviceApply.java \
  devicebeans/DeviceApplyBeanInfo.java \
  devicebeans/DeviceButtons.java \
  devicebeans/DeviceButtonsBeanInfo.java \
  devicebeans/DeviceButtonsCustomizer.java \
  devicebeans/DeviceCancel.java \
  devicebeans/DeviceCancelBeanInfo.java \
  devicebeans/DeviceChannel.java \
  devicebeans/DeviceChannelBeanInfo.java \
  devicebeans/DeviceChannelCustomizer.java \
  devicebeans/DeviceChoice.java \
  devicebeans/DeviceChoiceBeanInfo.java \
  devicebeans/DeviceChoiceCustomizer.java \
  devicebeans/DeviceCloseListener.java \
  devicebeans/DeviceComponent.java \
  devicebeans/DeviceControl.java \
  devicebeans/DeviceCustomizer.java \
  devicebeans/DeviceDispatch.java \
  devicebeans/DeviceDispatchBeanInfo.java \
  devicebeans/DeviceDispatchField.java \
  devicebeans/DeviceField.java \
  devicebeans/DeviceFieldBeanInfo.java \
  devicebeans/DeviceFieldCustomizer.java \
  devicebeans/DeviceLabel.java \
  devicebeans/DeviceLabelBeanInfo.java \
  devicebeans/DeviceLabelCustomizer.java \
  devicebeans/DeviceMultiComponent.java \
  devicebeans/DeviceOk.java \
  devicebeans/DeviceOkBeanInfo.java \
  devicebeans/DeviceParameters.java \
  devicebeans/DeviceParametersBeanInfo.java \
  devicebeans/DeviceReset.java \
  devicebeans/DeviceResetBeanInfo.java \
  devicebeans/DeviceSetup.java \
  devicebeans/DeviceSetupBeanInfo.java \
  devicebeans/DeviceTable.java \
  devicebeans/DeviceTableBeanInfo.java \
  devicebeans/DeviceTableCustomizer.java \
  devicebeans/DeviceUpdateListener.java
  
DEVWAV_SRC= \
  devicebeans/devicewave/DeviceWave.java \
  devicebeans/devicewave/DeviceWaveBeanInfo.java \
  devicebeans/devicewave/DeviceWaveCustomizer.java \
  devicebeans/devicewave/DeviceWaveDisplay.java \
  devicebeans/devicewave/DeviceWaveDisplayBeanInfo.java \
  devicebeans/devicewave/DeviceWaveDisplayCustomizer.java \
  devicebeans/devicewave/DeviceWaveParameters.java \
  devicebeans/devicewave/DeviceWaveParametersBeanInfo.java

JTRAVERSER_SRC= \
  jtraverser/DataChangeEvent.java \
  jtraverser/DataChangeListener.java \
  jtraverser/MdsView.java \
  jtraverser/Node.java \
  jtraverser/TreeView.java \
  jtraverser/TreeManager.java \
  jtraverser/dialogs/ActionList.java \
  jtraverser/dialogs/AddNodeDialog.java \
  jtraverser/dialogs/CheckBoxList.java \
  jtraverser/dialogs/DataDialog.java \
  jtraverser/dialogs/Dialogs.java \
  jtraverser/dialogs/FlagsDialog.java \
  jtraverser/dialogs/GraphPanel.java \
  jtraverser/dialogs/OpenTreeDialog.java \
  jtraverser/dialogs/SubTreeList.java \
  jtraverser/dialogs/TagsDialog.java \
  jtraverser/editor/ArgEditor.java \
  jtraverser/editor/ArrayEditor.java \
  jtraverser/editor/Editor.java \
  jtraverser/editor/ExprEditor.java \
  jtraverser/editor/MethodEditor.java \
  jtraverser/editor/ParameterEditor.java \
  jtraverser/editor/ProcedureEditor.java \
  jtraverser/editor/ProgramEditor.java \
  jtraverser/editor/PythonEditor.java \
  jtraverser/editor/SegmentEditor.java \
  jtraverser/editor/SignalEditor.java \
  jtraverser/editor/RangeEditor.java \
  jtraverser/editor/RoutineEditor.java \
  jtraverser/editor/usage/ActionEditor.java \
  jtraverser/editor/usage/AxisEditor.java \
  jtraverser/editor/usage/DataEditor.java \
  jtraverser/editor/usage/DispatchEditor.java \
  jtraverser/editor/usage/NumericEditor.java \
  jtraverser/editor/usage/SignalEditor.java \
  jtraverser/editor/usage/TaskEditor.java \
  jtraverser/editor/usage/TextEditor.java \
  jtraverser/editor/usage/WindowEditor.java \
  jtraverser/jTraverserFacade.java \
  jTraverser.java

TOOLS_SRC= \
  jtraverser/tools/CompileTree.java \
  jtraverser/tools/DecompileTree.java

MDSIP_SRC= \
  mds/Mds.java \
  mds/MdsEvent.java \
  mds/MdsListener.java \
  mds/MdsException.java \
  mds/MdsShr.java \
  mds/Shr.java \
  mds/TCL.java \
  mds/TdiShr.java \
  mds/TreeShr.java \
  mds/UpdateEvent.java \
  mds/UpdateEventListener.java \
  mds/data/DATA.java \
  mds/data/DTYPE.java \
  mds/data/OPC.java \
  mds/data/TREE.java \
  mds/data/descriptor/ARRAY.java \
  mds/data/descriptor/Descriptor.java \
  mds/data/descriptor/Descriptor_A.java \
  mds/data/descriptor/Descriptor_APD.java \
  mds/data/descriptor/Descriptor_CA.java \
  mds/data/descriptor/Descriptor_D.java \
  mds/data/descriptor/Descriptor_R.java \
  mds/data/descriptor/Descriptor_S.java \
  mds/data/descriptor/Descriptor_XD.java \
  mds/data/descriptor/Descriptor_XS.java \
  mds/data/descriptor_apd/Dictionary.java \
  mds/data/descriptor_apd/List.java \
  mds/data/descriptor_a/COMPLEXArray.java \
  mds/data/descriptor_a/CStringArray.java \
  mds/data/descriptor_a/Complex32Array.java \
  mds/data/descriptor_a/Complex64Array.java \
  mds/data/descriptor_a/EmptyArray.java \
  mds/data/descriptor_a/FLOATArray.java \
  mds/data/descriptor_a/Float32Array.java \
  mds/data/descriptor_a/Float64Array.java \
  mds/data/descriptor_a/Int128Array.java \
  mds/data/descriptor_a/Int16Array.java /
  mds/data/descriptor_a/Int32Array.java /
  mds/data/descriptor_a/Int64Array.java /
  mds/data/descriptor_a/Int8Array.java /
  mds/data/descriptor_a/INTEGER_UNSIGNEDArray.java \
  mds/data/descriptor_a/INTEGERArray.java \
  mds/data/descriptor_a/NUMBERArray.java /
  mds/data/descriptor_a/NidArray.java /
  mds/data/descriptor_a/Uint128Array.java /
  mds/data/descriptor_a/Uint16Array.java /
  mds/data/descriptor_a/Uint32Array.java /
  mds/data/descriptor_a/Uint64Array.java /
  mds/data/descriptor_a/Uint8Array.java /
  mds/data/descriptor_r/Action.java /
  mds/data/descriptor_r/Call.java /
  mds/data/descriptor_r/Condition.java /
  mds/data/descriptor_r/Conglom.java /
  mds/data/descriptor_r/Dependency.java /
  mds/data/descriptor_r/Dim.java /
  mds/data/descriptor_r/Dispatch.java /
  mds/data/descriptor_r/Function.java /
  mds/data/descriptor_r/Method.java /
  mds/data/descriptor_r/Opaque.java /
  mds/data/descriptor_r/Param.java /
  mds/data/descriptor_r/Procedure.java /
  mds/data/descriptor_r/Program.java /
  mds/data/descriptor_r/Range.java /
  mds/data/descriptor_r/Routine.java /
  mds/data/descriptor_r/Signal.java /
  mds/data/descriptor_r/Slope.java /
  mds/data/descriptor_r/Window.java /
  mds/data/descriptor_r/With_Error.java /
  mds/data/descriptor_r/With_Units.java /
  mds/data/descriptor_r/function/BINARY.java \
  mds/data/descriptor_r/function/BUILD.java /
  mds/data/descriptor_r/function/CAST.java \
  mds/data/descriptor_r/function/COMPRESSION.java \
  mds/data/descriptor_r/function/CONST.java \
  mds/data/descriptor_r/function/Fun.java \
  mds/data/descriptor_r/function/MODIFIER.java \
  mds/data/descriptor_r/function/UNARY.java \
  mds/data/descriptor_s/COMPLEX.java /
  mds/data/descriptor_s/CString.java /
  mds/data/descriptor_s/Complex32.java /
  mds/data/descriptor_s/Complex64.java /
  mds/data/descriptor_s/Event.java /
  mds/data/descriptor_s/FLOAT.java /
  mds/data/descriptor_s/Float32.java /
  mds/data/descriptor_s/Float64.java /
  mds/data/descriptor_s/Ident.java /
  mds/data/descriptor_s/Int128.java /
  mds/data/descriptor_s/Int16.java /
  mds/data/descriptor_s/Int32.java /
  mds/data/descriptor_s/Int64.java /
  mds/data/descriptor_s/Int8.java /
  mds/data/descriptor_s/INTEGER_UNSIGNED.java \
  mds/data/descriptor_s/INTEGER.java \
  mds/data/descriptor_s/Missing.java \
  mds/data/descriptor_s/NUMBER.java \
  mds/data/descriptor_s/Nid.java \
  mds/data/descriptor_s/NODE.java \
  mds/data/descriptor_s/Path.java \
  mds/data/descriptor_s/Pointer.java \
  mds/data/descriptor_s/Uint128.java \
  mds/data/descriptor_s/Uint16.java \
  mds/data/descriptor_s/Uint32.java \
  mds/data/descriptor_s/Uint64.java \
  mds/data/descriptor_s/Uint8.java \
  mds/mdsip/MdsIp.java \
  mds/mdsip/Message.java

DEVICE_GIF= \
  devicebeans/DeviceApply.gif \
  devicebeans/DeviceButtons.gif \
  devicebeans/DeviceCancel.gif \
  devicebeans/DeviceChannel.gif \
  devicebeans/DeviceChoice.gif \
  devicebeans/DeviceDispatch.gif \
  devicebeans/DeviceField.gif \
  devicebeans/DeviceOk.gif \
  devicebeans/DeviceReset.gif \
  devicebeans/DeviceSetup.gif

DEVICE_GIF= \
  devicebeans/devicewave/DeviceWave.gif

TRAV_GIF= \
  jtraverser/action.gif \
  jtraverser/any.gif \
  jtraverser/axis.gif \
  jtraverser/compound.gif \
  jtraverser/device.gif \
  jtraverser/dispatch.gif \
  jtraverser/numeric.gif \
  jtraverser/signal.gif \
  jtraverser/structure.gif \
  jtraverser/subtree.gif \
  jtraverser/task.gif \
  jtraverser/text.gif \
  jtraverser/window.gif

MDSIP_CLS=$(MDSIP_SRC:.java=*.class)
DEVICE_CLS=$(DEVICE_SRC:.java=*.class)
DEVWAV_CLS=$(DEVWAV_SRC:.java=*.class)
TRAV_CLS=$(TRAV_SRC:.java=*.class)

all : $(MDSIP_SRC) $(DEVICE_SRC) $(TRAV_SRC)
	$(JAVAC) $(CLASSPATH) $(MDSIP_SRC) $(DEVICE_SRC) $(TRAV_SRC)
	$(JAR) -c0f $(JARDIR)/MDSIP.jar $(MDSIP_SRC)
	$(JAR) -c0mf devicebeans/MANIFEST.mf $(JARDIR)/devicebeans.jar $(DEVICE_SRC) $(DEVICE_GIF)
	$(JAR) -c0mf jtraverser/MANIFEST.mf $(JARDIR)/jTraverser.jar $(TRAV_SRC) $(TRAV_GIF)
