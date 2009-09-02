import ctypes as _C
import numpy as _N
from _descriptor import descriptor,descriptor_a
from _mdsshr import _load_library,MdsException,MdsGetMsg
from mdsdata import makeData,Data
from mdsscalar import Scalar
from mdsarray import Array
from _mdsdtypes import *
from apd import List,Dictionary

class Connection(object):
    """Implements an MDSip connection to an MDSplus server"""
    __MdsIpShr=_load_library('MdsIpShr')
    __ConnectToMds=__MdsIpShr.ConnectToMds
    __ConnectToMds.argtypes=[_C.c_char_p]
    __MdsOpen=__MdsIpShr.MdsOpen
    __MdsOpen.argtypes=[_C.c_int,_C.c_char_p,_C.c_int]
    __GetAnswerInfoTS=__MdsIpShr.GetAnswerInfoTS
    __GetAnswerInfoTS.argtypes=[_C.c_int,_C.POINTER(_C.c_ubyte),_C.POINTER(_C.c_ushort),_C.POINTER(_C.c_ubyte),
                                _C.c_void_p,_C.POINTER(_C.c_ulong),_C.POINTER(_C.c_void_p),_C.POINTER(_C.c_void_p)]
    __mdsipFree=__MdsIpShr.MdsIpFree
    __mdsipFree.argtypes=[_C.c_void_p]
    __SendArg=__MdsIpShr.SendArg
    __SendArg.argtypes=[_C.c_int,_C.c_ubyte,_C.c_ubyte,_C.c_ubyte,_C.c_ushort,_C.c_ubyte,_C.c_void_p, _C.c_void_p]

    def __inspect__(self,value):
        """Internal routine used in determining characteristics of the value"""
        d=descriptor(value)
        if d.dtype==DTYPE_DSC:
            if d.pointer.contents.dclass == 4:
                a=_C.cast(d.pointer,_C.POINTER(descriptor_a)).contents
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
                raise MdsException,"Error handling argument of type %s" % (type(value),)
        else:
            length=d.length
            dtype=d.dtype
            dims=_N.array(0,dtype=_N.uint32)
            dimct=0
            pointer=d.pointer
        if dtype == DTYPE_FLOAT:
            dtype = DTYPE_F
        elif dtype == DTYPE_DOUBLE:
            dtype = DTYPE_D
        elif dtype == DTYPE_FLOAT_COMPLEX:
            dtype = DTYPE_FC
        elif dtype == DTYPE_DOUBLE_COMPLEX:
            dtype = DTYPE_DC
        return {'dtype':dtype,'length':length,'dimct':dimct,'dims':dims,'address':pointer}

    def __getAnswer__(self):
        dtype=_C.c_ubyte(0)
        length=_C.c_ushort(0)
        ndims=_C.c_ubyte(0)
        dims=_N.array([0,0,0,0,0,0,0,0],dtype=_N.uint32)
        numbytes=_C.c_ulong(0)
        ans=_C.c_void_p(0)
        mem=_C.c_void_p(0)
        status=self.__GetAnswerInfoTS(self.socket,dtype,length,ndims,dims.ctypes.data,numbytes,_C.pointer(ans),_C.pointer(mem))
        dtype=dtype.value
        if dtype == DTYPE_F:
            dtype = DTYPE_FLOAT
        elif dtype == DTYPE_D:
            dtype = DTYPE_DOUBLE
        elif dtype == DTYPE_FC:
            dtype = DTYPE_FLOAT_COMPLEX
        elif dtype == DTYPE_DC:
            dtype = DTYPE_DOUBLE_COMPLEX
        if ndims.value == 0:
            val=descriptor()
            val.dtype=dtype
            val.dclass=1
            val.length=length.value
            val.pointer=_C.cast(ans,_C.POINTER(descriptor))
            ans=val.value
        else:
            val=descriptor_a()
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
                self.__mdsipFree(mem)
            raise MdsException,MdsGetMsg(status)
        if mem.value is not None:
            self.__mdsipFree(mem)
        return ans
        
    def __init__(self,hostspec):
        self.socket=self.__ConnectToMds(hostspec)
        if self.socket == -1:
            raise Exception,"Error connecting to %s" % (hostspec,)
        self.hostspec=hostspec

    def __processGetMany__(cls):
        try:
            cls.GetMany(value=Data.getTdiVar("__getManyIn__").deserialize()).execute().serialize().setTdiVar("__getManyOut__")
        except Exception,e:
            print e
            raise
    __processGetMany__=classmethod(__processGetMany__)

    def __processPutMany__(cls):
        try:
            cls.PutMany(value=Data.getTdiVar("__putManyIn__").deserialize()).execute().serialize().setTdiVar("__putManyOut__")
        except Exception,e:
            print e
            raise
    __processPutMany__=classmethod(__processPutMany__)

                
    def __sendArg__(self,value,idx,num):
        """Internal routine to send argument to mdsip server"""
        val=makeData(value)
        if not isinstance(val,Scalar) and not isinstance(val,Array):
            val=makeData(val.data())
        valInfo=self.__inspect__(val)
        status=self.__SendArg(self.socket,idx,valInfo['dtype'],num,valInfo['length'],valInfo['dimct'],valInfo['dims'].ctypes.data,valInfo['address'])
        if not ((status & 1)==1):
            raise MdsException,MdsGetMsg(status)

    def closeAllTrees(self):
        """Close all open MDSplus trees
        @rtype: None
        """
        status=self.get("TreeClose()")
        if not ((status & 1)==1):
            raise MdsException,MdsGetMsg(status)

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
            raise MdsException,MdsGetMsg(status)

    def getMany(self):
        """Return instance of a Connection.GetMany class. See the Connection.GetMany documentation for further information."""
        return Connection.GetMany(connection=self)

    def openTree(self,tree,shot):
        """Open an MDSplus tree on a remote server
        @param tree: Name of tree
        @type tree: str
        @param shot: shot number
        @type shot: int
        @rtype: None
        """
        status=self.get("TreeOpen($,$)",tree,shot)
        if not ((status & 1)==1):
            raise MdsException,MdsGetMsg(status)

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
        num=len(args)+3
        idx=0
        pargs=[node,exp]
        putexp="TreePut($,$"
        for i in range(len(args)):
            putexp=putexp+",$"
            pargs.append(args[i])
        putexp=putexp+")"
        status=self.get(putexp,arglists=pargs)
        if not ((status & 1)==1):
            raise MdsException,MdsGetMsg(status)

    def putMany(self):
        """Return an instance of a Connection.PutMany class. See the Connection.PutMany documentation for further information."""
        return Connection.PutMany(connection=self)

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
        num=len(args)+1
        idx=0
        self.__sendArg__(exp,idx,num)
        for arg in args:
            idx=idx+1
            self.__sendArg__(arg,idx,num)
        return self.__getAnswer__()
    

    def setDefault(self,path):
        """Change the current default tree location on the remote server
        @param path: Tree node path to be the new default location.
        @type path: str
        @rtype: None
        """
        status=self.get("TreeSetDefault($)",path)
        if not ((status & 1)==1):
            raise MdsException,MdsGetMsg(status)


    class GetMany(List):
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
                List.__init__(self,value)
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
            super(Connection.GetMany,self).append(Dictionary({'name':str(name),'exp':str(exp),'args':args}))

        def execute(self):
            """Execute the list. Send the list to the remote server for evaluation and return the answer as a dict instance."""
            if self.connection is None:
                self.result=Dictionary()
                for val in self:
                    name=val['name']
                    try:
                        self.result[name]=Dictionary({'value':Data.execute('data('+val['exp']+')',tuple(val['args']))})
                    except Exception,e:
                        self.result[name]=Dictionary({'error':str(e)})
                return self.result
            else:
                ans=self.connection.get("public __getManyIn__=$,Py('Connection.__processGetMany__()'),public __getManyOut__",self.serialize())
	    if isinstance(ans,str):
		raise Exception("Error fetching data: "+ans)
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
                raise Exception,"GetMany has not yet been executed. Use the execute() method on this object first."
            if 'value' in self.result[name]:
                return self.result[name]['value']
            else:
                raise Exception,self.result[name]['error']

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
            d=Dictionary({'name':name,'exp':str(exp),'args':args})
            n = 0
            for item in self:
                if item['name'] == beforename:
                    super(Connection.GetMany,self).insert(n,d)
                    return
                else:
                    n=n+1
            raise Exception,"Item %s not found in list" % (beforename,)
                
        def remove(self,name):
            """Remove first occurrence of expression identified by its name from the list.
            @param name: Name of expression to be removed.
            @type name: str
            @rtype: None
            """
            for item in self:
                if item['name'] == name:
                    super(Connection.GetMany,self).remove(item)
                    return
            raise Exception,"Item %s not found in list" % (name,)

    class PutMany(List):
        """Build list of put instructions."""

        def __init__(self,value=None,connection=None):
            """Instance initialization"""
            if value is not None:
                List.__init__(self,value)
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
            super(Connection.PutMany,self).append(Dictionary({'node':str(node),'exp':str(exp),'args':args}))

        def getStatus(self,node):
            """Return the status of the put for this node. Anything other than 'Success' will raise an exception.
            @param node: Node name. Must match exactly the node name used in the append() or insert() methods.
            @type node: str
            @result: The string 'Success' otherwise an exception is raised.
            @rtype: str
            """
            if self.result is None:
                raise Exception,"PutMany has not yet been executed. Use the execute() method on this object first."
            if self.result[node] != "Success":
                raise MdsException,self.result[node]
            else:
                return self.result[node]
            
        def execute(self):
            """Execute the PutMany by sending the instructions to the remote server. The remote server will attempt to
            put the data in each of the nodes listed and after completion return a dict instance of the status of each put.
            @return: dict instance with status of each put. The key of the result will be the node name.
            """
            if self.connection is None:
                self.result=Dictionary()
                for val in self:
                    node=val['node']
                    try:
                        exp='TreePut($,$'
                        args=[node,val['exp']]
                        for i in range(len(val['args'])):
                            exp=exp+',$'
                            args.append(val['args'][i])
                        exp=exp+')'
                        status=Data.execute(exp,tuple(args))
                        if (status & 1) == 1:
                            self.result[node]='Success'
                        else:
                            self.result[node]=MdsGetMsg(status)
                    except Exception,e:
                        self.result[node]=str(e)
                return self.result
            else:
                ans=self.connection.get("public __putManyIn__=$,Py('Connection.__processPutMany__()'),public __putManyOut__",self.serialize())
	    if isinstance(ans,str):
		raise Exception("Error putting any data: "+ans)
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
            d=Dictionary({'node':str(node),'exp':str(exp),'args':args})
            n = 0
            for item in self:
                if item['node'] == beforenode:
                    super(Connection.PutMany,self).insert(n,d)
                    return
                else:
                    n=n+1
            raise Exception,"Node %s not found in list" % (str(beforenode),)
                
        def remove(self,node):
            """Remove the node from the list.
            @param node: node name to remove from list. Must match exactly the node name used in the append() or insert() methods.
            @type node: str
            @rtype: None
            """
            for item in self:
                if item['node'] == node:
                    super(Connection.PutMany,self).remove(item)
                    return
            raise Exception,"Node %s not found in list" % (node,)
