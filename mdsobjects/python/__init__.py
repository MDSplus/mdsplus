"""
MDSobjects
==========

Provides a object oriented interface to the MDSplus data system.

Information about the B{I{MDSplus Data System}} can be found at U{the MDSplus Homepage<http://www.mdsplus.org>}
@authors: Tom Fredian(MIT/USA), Gabriele Manduchi(CNR/IT), Josh Stillerman(MIT/USA)
@copyright: 2008
@license: GNU GPL



"""

from pydoc import TextDoc
from MDSobjects.apd import Apd,Dictionary
from MDSobjects.array import makeArray,Float32Array,Float64Array,Int128Array,Int16Array,Int32Array,Int64Array,Int8Array,StringArray,Uint128Array,Uint16Array,Uint32Array,Uint64Array,Uint8Array
from MDSobjects.compound import Action,Conglom,Dependency,Dimension,Dispatch,Function,Method,Procedure,Program,Range,Routine,Signal,Window
from MDSobjects.data import Data,EmptyData,getUnits,getError,getValuePart,getDimension,data,decompile,evaluate,rawPart,makeData
from MDSobjects.ident import Ident
from MDSobjects.treenode import TreeNode,TreePath,TreeNodeArray
from MDSobjects.scalar import Scalar,Float32,Float64,Int128,Int16,Int32,Int64,Int8,String,Uint128,Uint16,Uint32,Uint64,Uint8
from MDSobjects.tree import Tree
#tdoc=TextDoc()
#__doc__=__doc__+tdoc.docmodule(Apd)+tdoc.docclass(Apd)
#__doc__=__doc__+tdoc.docmodule(Array)
#__doc__=__doc__+tdoc.docmodule(Compound)+tdoc.docclass(Action)+tdoc.docclass(Conglom)
#__doc__=__doc__+tdoc.docmodule(Ident)+tdoc.docclass(Ident)
#__doc__=__doc__+tdoc.docmodule(Scalar)
#__doc__=__doc__+tdoc.docmodule(Tree)+tdoc.docclass(Tree)
#__doc__=__doc__+tdoc.docmodule(TreeNode)+tdoc.docclass(TreeNode)
#del tdoc
