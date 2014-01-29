if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name,level):
    return __import__(name,globals())
else:
  def _mimport(name,level):
    return __import__(name,globals(),{},[],level)

_apd=_mimport('apd',1)
Apd=_apd.Apd
Dictionary=_apd.Dictionary
List=_apd.List

_array=_mimport('mdsarray',1)
Array=_array.Array
makeArray=_array.makeArray
Float32Array=_array.Float32Array
Float64Array=_array.Float64Array
Int128Array=_array.Int128Array
Int16Array=_array.Int16Array
Int32Array=_array.Int32Array
Int64Array=_array.Int64Array
Int8Array=_array.Int8Array
StringArray=_array.StringArray
Uint128Array=_array.Uint128Array
Uint16Array=_array.Uint16Array
Uint32Array=_array.Uint32Array
Uint64Array=_array.Uint64Array
Uint8Array=_array.Uint8Array

_compound=_mimport('compound',1)
Action=_compound.Action
Call=_compound.Call
Conglom=_compound.Conglom
Dependency=_compound.Dependency
Dimension=_compound.Dimension
Dispatch=_compound.Dispatch
Function=_compound.Function
Method=_compound.Method
Procedure=_compound.Procedure
Program=_compound.Program
Range=_compound.Range
Routine=_compound.Routine
Signal=_compound.Signal
Window=_compound.Window
Opaque=_compound.Opaque

_data=_mimport('mdsdata',1)
Data=_data.Data
EmptyData=_data.EmptyData
getUnits=_data.getUnits
getError=_data.getError
getValuePart=_data.getValuePart
getDimension=_data.getDimension
data=_data.data
decompile=_data.decompile
evaluate=_data.evaluate
rawPart=_data.rawPart
makeData=_data.makeData

_ident=_mimport('ident',1)
Ident=_ident.Ident

_treenode=_mimport('treenode',1)
TreeNode=_treenode.TreeNode
TreePath=_treenode.TreePath
TreeNodeArray=_treenode.TreeNodeArray

_scalar=_mimport('mdsscalar',1)
makeScalar=_scalar.makeScalar
Scalar=_scalar.Scalar
Float32=_scalar.Float32
Float64=_scalar.Float64
Int128=_scalar.Int128
Int16=_scalar.Int16
Int32=_scalar.Int32
Int64=_scalar.Int64
Int8=_scalar.Int8
String=_scalar.String
Uint128=_scalar.Uint128
Uint16=_scalar.Uint16
Uint32=_scalar.Uint32
Uint64=_scalar.Uint64
Uint8=_scalar.Uint8

_tree=_mimport('tree',1)
Tree=_tree.Tree

_mdsdevice=_mimport('mdsdevice',1)
Device=_mdsdevice.Device

_connection=_mimport('connection',1)
Connection=_connection.Connection

_event=_mimport('event',1)
Event=_event.Event

_tdishr=_mimport('_tdishr',1)
TdiException=_tdishr.TdiException

_scope=_mimport('scope',1)
Scope=_scope.Scope

_mdsshr=_mimport('_mdsshr',1)
MdsException=_mdsshr.MdsException
MdsTimeout=_mdsshr.MdsTimeout
MdsNoMoreEvents=_mdsshr.MdsNoMoreEvents
MdsInvalidEvent=_mdsshr.MdsInvalidEvent

_treeshr=_mimport('_treeshr',1)
TreeException=_treeshr.TreeException
TreeNoDataException=_treeshr.TreeNoDataException

_tdipy=_mimport('tdipy',1)
execPy=_tdipy.execPy

_descriptor=_mimport('_descriptor',1)
pointerToObject=_descriptor.pointerToObject

try:
  from tdicompile import tdiCompile,compileFile
except:
  pass
