#
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

import ctypes as _C
import numpy as _N

_dsc=_mimport('descriptor')
_exc=_mimport('mdsExceptions')
_apd=_mimport('apd')
_sca=_mimport('mdsscalar')
_arr=_mimport('mdsarray')
_dat=_mimport('mdsdata')
_ver=_mimport('version')

class MdsIpException(_exc.MDSplusException):
  pass

__MdsIpShr=_ver.load_library('MdsIpShr')
_ConnectToMds=__MdsIpShr.ConnectToMds
_DisconnectFromMds=__MdsIpShr.DisconnectFromMds
_GetAnswerInfoTO=__MdsIpShr.GetAnswerInfoTO
_GetAnswerInfoTO.argtypes=[_C.c_int32,_C.POINTER(_C.c_ubyte),_C.POINTER(_C.c_ushort),_C.POINTER(_C.c_ubyte),
                            _C.c_void_p,_C.POINTER(_C.c_ulong),_C.POINTER(_C.c_void_p),_C.POINTER(_C.c_void_p), _C.c_int32]
_MdsIpFree=__MdsIpShr.MdsIpFree
_MdsIpFree.argtypes=[_C.c_void_p]
_SendArg=__MdsIpShr.SendArg
_SendArg.argtypes=[_C.c_int32,_C.c_ubyte,_C.c_ubyte,_C.c_ubyte,_C.c_ushort,_C.c_ubyte,_C.c_void_p, _C.c_void_p]

class Connection(object):
    """Implements an MDSip connection to an MDSplus server"""

    def __enter__(self):
        """ Used for with statement. """
        return self

    def __exit__(self, type, value, traceback):
        """ Cleanup for with statement. """
        _DisconnectFromMds(self.socket)

    def __inspect__(self,value):
        """Internal routine used in determining characteristics of the value"""
        d=value.descriptor
        if d.dclass == 4:
            dims=list()
            if d.dimct == 1:
                dims.append(d.arsize/d.length)
            else:
                for i in range(d.dimct):
                    dims.append(d.coeff_and_bounds[i])
            dtype=d.dtype
            length=d.length
            dims=_N.array(dims,dtype=_N.uint32)
            dimct=d.dimct
            pointer=d.pointer
        else:
            length=d.length
            dtype=d.dtype
            dims=_N.array(0,dtype=_N.uint32)
            dimct=0
            pointer=d.pointer
        if   dtype == 52: dtype = 10
        elif dtype == 53: dtype = 11
        elif dtype == 54: dtype = 12
        elif dtype == 55: dtype = 13
        return {'dtype':dtype,'length':length,'dimct':dimct,'dims':dims,'address':pointer}

    def __getAnswer__(self,to_msec=-1):
        dtype=_C.c_ubyte(0)
        length=_C.c_ushort(0)
        ndims=_C.c_ubyte(0)
        dims=_N.array([0,0,0,0,0,0,0,0],dtype=_N.uint32)
        numbytes=_C.c_ulong(0)
        ans=_C.c_void_p(0)
        mem=_C.c_void_p(0)
        try:
            _exc.checkStatus(_GetAnswerInfoTO(self.socket,dtype,length,ndims,dims.ctypes.data,numbytes,_C.byref(ans),_C.byref(mem),int(to_msec)))
            dtype=dtype.value
            if   dtype == 10: dtype = 52
            elif dtype == 11: dtype = 53
            elif dtype == 12: dtype = 54
            elif dtype == 13: dtype = 55
            if ndims.value == 0:
                d=_dsc.Descriptor_s()
                d.dtype=dtype
                d.length=length.value
                d.pointer=ans
                return d.value
            val=_dsc.Descriptor_a()
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
            return val.value
        except _exc.MDSplusException:
            if ndims.value == 0 and dtype == _sca.String.dtype_id:
                d=_dsc.Descriptor_s()
                d.dtype=dtype
                d.length=length.value
                d.pointer=ans
                raise MdsIpException(str(d.value))
            raise
        finally:
            if mem.value is not None:
                _MdsIpFree(mem)

    def __init__(self,hostspec):
        self.socket=_ConnectToMds(_ver.tobytes(hostspec))
        if self.socket == -1:
            raise MdsIpException("Error connecting to %s" % (hostspec,))
        self.hostspec=hostspec

    def __del__(self):
        try:    _DisconnectFromMds(self.socket)
        except: pass

    def __sendArg__(self,value,idx,num):
        """Internal routine to send argument to mdsip server"""
        val=_dat.Data(value)
        if not isinstance(val,_sca.Scalar) and not isinstance(val,_arr.Array):
            val=_dat.Data(val.data())
        valInfo=self.__inspect__(val)
        _exc.checkStatus(
            _SendArg(self.socket,
                     idx,
                     valInfo['dtype'],
                     num,
                     valInfo['length'],
                     valInfo['dimct'],
                     valInfo['dims'].ctypes.data,
                     valInfo['address']))

    def closeAllTrees(self):
        """Close all open MDSplus trees
        @rtype: number of closed trees
        """
        self.get("_i=0;WHILE(IAND(TreeClose(),1)) _i++;_i")

    def closeTree(self,tree,shot):
        """Close an MDSplus tree on the remote server
        @param tree: tree name
        @type tree: str
        @param shot: shot number
        @type shot: int
        @rtype: None
        """
        _exc.checkStatus(self.get("TreeClose($,$)",arglist=(tree,shot)))

    def getMany(self, value=None):
        """Return instance of a connection.GetMany class. See the connection.GetMany documentation for further information."""
        return GetMany(value, self)

    def openTree(self,tree,shot):
        """Open an MDSplus tree on a remote server
        @param tree: Name of tree
        @type tree: str
        @param shot: shot number
        @type shot: int
        @rtype: None
        """
        _exc.checkStatus(self.get("TreeOpen($,$)",tree,shot))

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
        pexp  = 'TreePut($,$%s)'%(',$'*len(args),)
        pargs = [node,exp]+args
        _exc.checkStatus(self.get(pexp,arglist=pargs))

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
        if 'arglist' in kwargs:
            args=kwargs['arglist']
        timeout = kwargs.get('timeout',-1)
        num=len(args)+1
        exp = _ver.tobytes(exp)
        _exc.checkStatus(_SendArg(self.socket,0,14,num,len(exp),0,0,_C.c_char_p(exp)))
        for i,arg in enumerate(args):
            self.__sendArg__(arg,i+1,num)
        return self.__getAnswer__(timeout)

    def getObject(self,exp,*args,**kwargs):
        return self.get('serializeout(`(%s;))'%exp,*args,**kwargs).deserialize()

    def setDefault(self,path):
        """Change the current default tree location on the remote server
        @param path: Tree node path to be the new default location.
        @type path: str
        @rtype: None
        """
        _exc.checkStatus(self.get("TreeSetDefault($)",path))

    def GetMany(self): return GetMany(self)
    def PutMany(self): return PutMany(self)


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

    def __init__(self,connection):
        """Instance initialization"""
        super(GetMany,self).__init__()
        if isinstance(connection,Connection):
            self.connection=connection
        else:
            self.connection=Connection(connection)
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
                    self.result[name]=_apd.Dictionary({'value':_dat.Data.execute('data('+val['exp']+')',tuple(val['args']))})
                except Exception as exc:
                    self.result[name]=_apd.Dictionary({'error':str(exc)})
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
    def __init__(self,connection):
        """Instance initialization"""
        super(PutMany,self).__init__()
        if isinstance(connection,Connection):
            self.connection=connection
        else:
            self.connection=Connection(connection)
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
            self.result = _apd.Dictionary()
            for val in self:
                node = val['node']
                try:
                    exp  = 'TreePut($,$%s)'%(',$'*len(val['args']),)
                    args = [node,val['exp']]+val['args']
                    _exc.checkStatus(_dat.Data.execute(exp,tuple(args)))
                    self.result[node] = 'Success'
                except _exc.MDSplusException as exc:
                    self.result[node] = exc.message
                except Exception as exc:
                    self.result[node] = str(exc)
            return self.result
        else:
            ans = self.connection.get("PutManyExecute($)",self.serialize())
        if isinstance(ans,str):
            raise MdsIpException("Error putting any data: "+ans)
        self.result = ans.deserialize()
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
                n += 1
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
