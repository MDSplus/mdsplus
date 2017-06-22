def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import sys as _sys
import ctypes as _C
import numpy as _N
from threading import RLock as _RLock

_descriptor=_mimport('_descriptor')
_Exceptions=_mimport('mdsExceptions')
_mdsshr=_mimport('_mdsshr')
_apd=_mimport('apd')
_scalar=_mimport('mdsscalar')
_array=_mimport('mdsarray')
_data=_mimport('mdsdata')
_dtypes=_mimport('_mdsdtypes')
_ver=_mimport('version')

class MdsIpException(_Exceptions.MDSplusException):
  pass

__MdsIpShr=_ver.load_library('MdsIpShr')
_ConnectToMds=__MdsIpShr.ConnectToMds
_DisconnectFromMds=__MdsIpShr.DisconnectFromMds
_GetAnswerInfoTS=__MdsIpShr.GetAnswerInfoTS
_GetAnswerInfoTS.argtypes=[_C.c_int32,_C.POINTER(_C.c_ubyte),_C.POINTER(_C.c_ushort),_C.POINTER(_C.c_ubyte),
                            _C.c_void_p,_C.POINTER(_C.c_ulong),_C.POINTER(_C.c_void_p),_C.POINTER(_C.c_void_p)]
_MdsIpFree=__MdsIpShr.MdsIpFree
_MdsIpFree.argtypes=[_C.c_void_p]
_SendArg=__MdsIpShr.SendArg
_SendArg.argtypes=[_C.c_int32,_C.c_ubyte,_C.c_ubyte,_C.c_ubyte,_C.c_ushort,_C.c_ubyte,_C.c_void_p, _C.c_void_p]

class Connection(object):
    """Implements an MDSip connection to an MDSplus server"""

    dtype_to_scalar={_dtypes.DTYPE_BU:_scalar.Uint8,_dtypes.DTYPE_WU:_scalar.Uint16,_dtypes.DTYPE_LU:_scalar.Uint32,
                     _dtypes.DTYPE_QU:_scalar.Uint64,_dtypes.DTYPE_B:_scalar.Int8,_dtypes.DTYPE_W:_scalar.Int16,
                     _dtypes.DTYPE_L:_scalar.Int32,_dtypes.DTYPE_Q:_scalar.Int64,_dtypes.DTYPE_FLOAT:_scalar.Float32,
                     _dtypes.DTYPE_DOUBLE:_scalar.Float64,_dtypes.DTYPE_T:_scalar.String}


    def __enter__(self):
        """ Used for with statement. """
        return self

    def __exit__(self, type, value, traceback):
        """ Cleanup for with statement. """
        _DisconnectFromMds(self.socket)

    def __inspect__(self,value):
        """Internal routine used in determining characteristics of the value"""
        d=_descriptor.descriptor(value)
        if d.dtype==_dtypes.DTYPE_DSC:
            if d.pointer.contents.dclass == 4:
                a=_C.cast(d.pointer,_C.POINTER(_descriptor.descriptor_a)).contents
                dims=list()
                if a.dimct == 1:
                    dims.append(a.arsize/a.length)
                else:
                    for i in range(a.dimct):
                        dims.append(a.coeff_and_bounds[i])
                dtype=a.dtype
                length=a.length
                dims=_N.array(dims,dtype=_N.uint32)
                dimct=a.dimct
                pointer=a.pointer
            else:
                raise MdsIpException("Error handling argument of type %s" % (type(value),))
        else:
            length=d.length
            dtype=d.dtype
            dims=_N.array(0,dtype=_N.uint32)
            dimct=0
            pointer=d.pointer
        if dtype == _dtypes.DTYPE_FLOAT:
            dtype = _dtypes.DTYPE_F
        elif dtype == _dtypes.DTYPE_DOUBLE:
            dtype = _dtypes.DTYPE_D
        elif dtype == _dtypes.DTYPE_FLOAT_COMPLEX:
            dtype = _dtypes.DTYPE_FC
        elif dtype == _dtypes.DTYPE_DOUBLE_COMPLEX:
            dtype = _dtypes.DTYPE_DC
        return {'dtype':dtype,'length':length,'dimct':dimct,'dims':dims,'address':pointer}

    def __getAnswer__(self):
        dtype=_C.c_ubyte(0)
        length=_C.c_ushort(0)
        ndims=_C.c_ubyte(0)
        dims=_N.array([0,0,0,0,0,0,0,0],dtype=_N.uint32)
        numbytes=_C.c_ulong(0)
        ans=_C.c_void_p(0)
        mem=_C.c_void_p(0)
        status=_GetAnswerInfoTS(self.socket,dtype,length,ndims,dims.ctypes.data,numbytes,_C.pointer(ans),_C.pointer(mem))
        dtype=dtype.value
        if dtype == _dtypes.DTYPE_F:
            dtype = _dtypes.DTYPE_FLOAT
        elif dtype == _dtypes.DTYPE_D:
            dtype = _dtypes.DTYPE_DOUBLE
        elif dtype == _dtypes.DTYPE_FC:
            dtype = _dtypes.DTYPE_FLOAT_COMPLEX
        elif dtype == _dtypes.DTYPE_DC:
            dtype = _dtypes.DTYPE_DOUBLE_COMPLEX
        if ndims.value == 0:
            if dtype == _dtypes.DTYPE_T:
                ans=_scalar.String(_C.cast(ans,_C.POINTER(_C.c_char*length.value)).contents.value)
            else:
                ans=Connection.dtype_to_scalar[dtype](_C.cast(ans,_C.POINTER(_dtypes.mdsdtypes.ctypes[dtype])).contents.value)
        else:
            val=_descriptor.descriptor_a()
            val.dtype=dtype
            val.dclass=4
            val.length=length.value
            val.pointer=ans
            val.scale=0
            val.digits=0
            val.aflags=0
            val.dimct=ndims.value
            val.arsize=numbytes.value
            val.a0=val.pointer
            if val.dimct > 1:
                val.coeff=1
                for i in range(val.dimct):
                    val.coeff_and_bounds[i]=int(dims[i])
            ans=val.value
        if not ((status & 1) == 1):
            if mem.value is not None:
                _MdsIpFree(mem)
            if isinstance(ans,_scalar.String):
                raise MdsIpException(str(ans))
            else:
                raise _Exceptions.statusToException(status)
        if mem.value is not None:
            _MdsIpFree(mem)
        return ans

    def __init__(self,hostspec):
      self.socket=_ConnectToMds(_ver.tobytes(hostspec))
      if self.socket == -1:
        raise MdsIpException("Error connecting to %s" % (hostspec,))
      self.hostspec=hostspec
      self.lock=_RLock()

    def __del__(self):
        try:
            _DisconnectFromMds(self.socket)
        except:
            pass

    def __sendArg__(self,value,idx,num):
        """Internal routine to send argument to mdsip server"""
        val=_data.makeData(value)
        if not isinstance(val,_scalar.Scalar) and not isinstance(val,_array.Array):
            val=_data.makeData(val.data())
        valInfo=self.__inspect__(val)
        status=_SendArg(self.socket,idx,valInfo['dtype'],num,valInfo['length'],valInfo['dimct'],valInfo['dims'].ctypes.data,valInfo['address'])
        if not ((status & 1)==1):
            raise _Exceptions.statusToException(status)

    def closeAllTrees(self):
        """Close all open MDSplus trees
        @rtype: None
        """
        status=self.get("TreeClose()")
        if not ((status & 1)==1):
            raise _Exceptions.statusToException(status)

    def closeTree(self,tree,shot):
        """Close an MDSplus tree on the remote server
        @param tree: tree name
        @type tree: str
        @param shot: shot number
        @type shot: int
        @rtype: None
        """
        status=self.get("TreeClose($,$)",arglist=(tree,shot))
        if not ((status & 1)==1):
            raise _Exceptions.statusToException(status)

    def getMany(self, value=None):
        """Return instance of a connection.GetMany class. See the connection.GetMany documentation for further information."""
        return GetMany(value, self)

    def openTree(self,tree,shot,mode='NORMAL'):
        """Open an MDSplus tree on a remote server
        @param tree: Name of tree
        @type tree: str
        @param shot: shot number
        @type shot: int
        @param mode: Optional mode, one of 'Normal','Edit','New','Readonly'
        @rtype: None
        """
        mode = mode.upper
        if mode=='EDIT':
            status=self.get("TreeOpenEdit($,$)",tree,shot)
        if mode=='NEW':
            status=self.get("TreeOpenNew($,$)",tree,shot)
        elif mode=='READONLY':
            status=self.get("TreeOpen($,$,1)",tree,shot)
        else:  # stay compatible with old TreeOpen
            status=self.get("TreeOpen($,$)",tree,shot)
        if not ((status & 1)==1):
            raise _Exceptions.statusToException(status)

    def put(self,node,exp,*args):
        """Put data into a node in an MDSplus tree
        @param node: Node name, relative or full path. Include double backslashes in string if node name includes one.
        @type node: str
        @param exp: TDI expression with placeholders for any optional args.
        @type exp: str
        @param args: optional arguments to be inserted for the placeholders in the expression.
        @type args: Data
        @rtype: None
        """
        pargs=[node,exp]
        putexp="TreePut($,$"
        for i in range(len(args)):
            putexp=putexp+",$"
            pargs.append(args[i])
        putexp=putexp+")"
        status=self.get(putexp,arglist=pargs)
        if not ((status & 1)==1):
            raise _Exceptions.statusToException(status)

    def putMany(self, value=None):
        """Return an instance of a connection.PutMany class. See the connection.PutMany documentation for further information."""
        return PutMany(value, connection=self)

    def get(self,exp,*args,**kwargs):
        """Evaluate and expression on the remote server
        @param exp: TDI expression to be evaluated
        @type exp: str
        @param args: optional arguments to be inserted for the placeholders in the expression.
        @type args: Data
        @param kwargs: Used for internal purposes
        @return: result of evaluating the expression on the remote server
        @rtype: Scalar or Array
        """
        try:
            self.lock.acquire()
            if 'arglist' in kwargs:
                args=kwargs['arglist']
            num=len(args)+1
            idx=0
            status=_SendArg(self.socket,idx,14,num,len(exp),0,0,_C.c_char_p(_ver.tobytes(exp)))
            if not ((status & 1)==1):
                raise _Exceptions.statusToException(status)
            #self.__sendArg__(exp,idx,num)
            for arg in args:
                idx=idx+1
                self.__sendArg__(arg,idx,num)
            ans = self.__getAnswer__()
        finally:
            self.lock.release()
        return ans


    def setDefault(self,path):
        """Change the current default tree location on the remote server
        @param path: Tree node path to be the new default location.
        @type path: str
        @rtype: None
        """
        status=self.get("TreeSetDefault($)",path)
        if not ((status & 1)==1):
            raise _Exceptions.statusToException(status)

    # depreciated
    def GetMany(self,*arg):
        raise(MdsIpException('\nThe subclass "Connection.GetMany" is now a class of it own.\nUse "GetMany" instead.'))
    def PutMany(self,*arg):
        raise(MdsIpException('\nThe subclass "Connection.PutMany" is now a class of it own.\nUse "PutMany" instead.'))

class GetMany(_apd.List):
    """Build a list of expressions to evaluate

    To reduce the number of network transactions between you and the remote system you can
    use the GetMany class to specify a list of expressions to be evaluated and then
    send that list to the remote system in one network transation. The remote system will
    then evaluate all of the expressions and return the answer in one response.

    To use the GetMany class you can create an instance using the getMany() method of a
    Connection instance. You then use the GetMany.append(name,expression[,args]) method
    to add expressions to the list. Once the list is complete you then use the GetMany.execute()
    method to execute the expressions on the remote host and retrieve the answers. This will
    return a dictionary instance with the names assigned to the expressions as the key. Each
    name will have a dictionary instance containing the result of the execution of the expression.
    If this dictionary has an 'error' key then its value will be an error string, otherwise
    the dictionary should have a 'value' key containing the result.

    The GetMany instance can be executed multiple times. For instance, if you want to get the
    same information from many different trees you could use the Connection.openTeee(tree,shot)
    method between executions of the same GetMany instance.

    NOTE: MDSplus can currently only address objects less than 4 gigabytes. Therefore the
    maximum size of the expression list with arguments and the result dictionary is approximately 4 gigatypes.
    """

    def __init__(self,value=None,connection=None):
        """GetMany instance initialization."""
        if value is not None:
            _apd.List.__init__(self,value)
        self.connection=connection
        self.result=None

    def append(self,name,exp,*args):
        """Append expression to the list.
        @param name: name to assign to the expression for identifying it in the result dictionary.
        @type name: str
        @param exp: expression to be evaluated with placeholders for optional arguments
        @type exp: str
        @param args: optional arguments to replace placeholders in the expression
        @type args: Data
        @rtype: None
        """
        super(GetMany,self).append(_apd.Dictionary({'name':str(name),'exp':str(exp),'args':args}))

    def execute(self):
        """Execute the list. Send the list to the remote server for evaluation and return the answer as a dict instance."""
        if self.connection is None:
            self.result=_apd.Dictionary()
            for val in self:
                name=val['name']
                try:
                    self.result[name]=_apd.Dictionary({'value':_data.Data.execute('data('+val['exp']+')',tuple(val['args']))})
                except Exception:
                    import sys
                    e=sys.exc_info()[1]
                    self.result[name]=_apd.Dictionary({'error':str(e)})
            return self.result
        else:
            ans=self.connection.get("GetManyExecute($)",self.serialize())
        if isinstance(ans,str):
            raise MdsIpException("Error fetching data: "+ans)
        self.result=ans.deserialize()
        return self.result

    def get(self,name):
        """Get the result of an expression identified by name from the last invokation of the execute() method.
        @param name: name associated with an expression.
        @type name: str
        @return: result of the expression evaluation.
        @rtype: Scalar or Array
        """
        if self.result is None:
            raise MdsIpException("GetMany has not yet been executed. Use the execute() method on this object first.")
        if 'value' in self.result[name]:
            return self.result[name]['value']
        else:
            raise MdsIpException(self.result[name]['error'])

    def insert(self,beforename, name,exp,*args):
        """Insert an expression in the list before the one named in the beforename argument.
        @param beforename: Insert the expression before this one
        @type beforename: str
        @param name: Name to associate with the result of this expression
        @type name: str
        @param exp: TDI expression to be evaluated with optional placeholders for the arguments
        @type expression: str
        @param args: Optional arguments to replace placeholders in the expression
        @type args: Data
        @rtype: None
        """
        d=_apd.Dictionary({'name':name,'exp':str(exp),'args':args})
        n = 0
        for item in self:
            if item['name'] == beforename:
                super(GetMany,self).insert(n,d)
                return
            else:
                n=n+1
        raise MdsIpException("Item %s not found in list" % (beforename,))

    def remove(self,name):
        """Remove first occurrence of expression identified by its name from the list.
        @param name: Name of expression to be removed.
        @type name: str
        @rtype: None
        """
        for item in self:
            if item['name'] == name:
                super(GetMany,self).remove(item)
                return
        raise MdsIpException("Item %s not found in list" % (name,))


class PutMany(_apd.List):
    """Build list of put instructions."""

    def __init__(self,value=None,connection=None):
        """Instance initialization"""
        if value is not None:
            _apd.List.__init__(self,value)
        self.connection=connection
        self.result=None

    def append(self,node,exp,*args):
        """Append node data information
        @param node: Node name where to store the data
        @type node: str
        @param exp: TDI expression to be stored in node with optional placeholders for arguments
        @type exp: str
        @param args: optional arguments to replace placeholders in expression
        @type args: Data
        @rtype: None
        """
        super(PutMany,self).append(_apd.Dictionary({'node':str(node),'exp':str(exp),'args':args}))

    def checkStatus(self,node):
        """Return the status of the put for this node. Anything other than 'Success' will raise an exception.
        @param node: Node name. Must match exactly the node name used in the append() or insert() methods.
        @type node: str
        @result: The string 'Success' otherwise an exception is raised.
        @rtype: str
        """
        if self.result is None:
            raise MdsIpException("PutMany has not yet been executed. Use the execute() method on this object first.")
        if self.result[node] != "Success":
            raise MdsIpException(self.result[node])
        else:
            return self.result[node]

    def execute(self):
        """Execute the PutMany by sending the instructions to the remote server. The remote server will attempt to
        put the data in each of the nodes listed and after completion return a dict instance of the status of each put.
        @return: dict instance with status of each put. The key of the result will be the node name.
        """
        if self.connection is None:
            self.result=_apd.Dictionary()
            for val in self:
                node=val['node']
                try:
                    exp='TreePut($,$'
                    args=[node,val['exp']]
                    for i in range(len(val['args'])):
                        exp=exp+',$'
                        args.append(val['args'][i])
                    exp=exp+')'
                    status=_data.Data.execute(exp,tuple(args))
                    if (status & 1) == 1:
                        self.result[node]='Success'
                    else:
                        self.result[node]=_mdsshr.MdsGetMsg(status)
                except:
                    self.result[node]=str(_sys.exc_info()[1])
            return self.result
        else:
            ans=self.connection.get("PutManyExecute($)",self.serialize())
        if isinstance(ans,str):
            raise MdsIpException("Error putting any data: "+ans)
        self.result=ans.deserialize()
        return self.result

    def insert(self,beforenode, node,exp,*args):
        """Insert put data before node in list specified by beforenode
        @param beforenode: Name of node in list to insert this put data information.
        @type beforenode: str
        @param node: Node name to put data into
        @type node: str
        @param exp: TDI expression to store in node with optional placeholders for arguments
        @type exp: str
        @param args: Optional arguments to replace placeholders in expression
        @type args: Data
        @rtype: None
        """
        d=_apd.Dictionary({'node':str(node),'exp':str(exp),'args':args})
        n = 0
        for item in self:
            if item['node'] == beforenode:
                super(PutMany,self).insert(n,d)
                return
            else:
                n=n+1
        raise MdsIpException("Node %s not found in list" % (str(beforenode),))

    def remove(self,node):
        """Remove the node from the list.
        @param node: node name to remove from list. Must match exactly the node name used in the append() or insert() methods.
        @type node: str
        @rtype: None
        """
        for item in self:
            if item['node'] == node:
                super(PutMany,self).remove(item)
                return
        raise MdsIpException("Node %s not found in list" % (node,))
