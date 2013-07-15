"""
MDSplus
==========

Provides a object oriented interface to the MDSplus data system.

Information about the B{I{MDSplus Data System}} can be found at U{the MDSplus Homepage<http://www.mdsplus.org>}
@authors: Tom Fredian(MIT/USA), Gabriele Manduchi(CNR/IT), Josh Stillerman(MIT/USA)
@copyright: 2008
@license: GNU GPL



"""

from apd import Apd,Dictionary,List
from mdsarray import Array,makeArray,Float32Array,Float64Array,Int128Array,Int16Array,Int32Array,Int64Array,Int8Array,StringArray,Uint128Array,Uint16Array,Uint32Array,Uint64Array,Uint8Array
from compound import Action,Call,Conglom,Dependency,Dimension,Dispatch,Function,Method,Procedure,Program,Range,Routine,Signal,Window,Opaque
from mdsdata import Data,EmptyData,getUnits,getError,getValuePart,getDimension,data,decompile,evaluate,rawPart,makeData
from ident import Ident
from treenode import TreeNode,TreePath,TreeNodeArray
from mdsscalar import Scalar,Float32,Float64,Int128,Int16,Int32,Int64,Int8,String,Uint128,Uint16,Uint32,Uint64,Uint8
from tree import Tree
from mdsdevice import Device
from connection import Connection
from event import Event
from _tdishr import TdiException
from scope import Scope
from _mdsshr import MdsException,MdsTimeout,MdsNoMoreEvents,MdsInvalidEvent
from _treeshr import TreeException,TreeNoDataException
from tdipy import execPy
try:
  from tdicompile import tdiCompile
except:
  pass
