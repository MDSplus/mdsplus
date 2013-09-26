try:
    exec("from ..compound import Function as _Function")
except:
    try:
        from compound import Function as _Function
    except:
        from MDSplus.compound import Function as _Function

class Builtin(_Function):
    _dtype=199
    min_args = None
    max_args = None
    precedence=0
    builtins_by_name=dict()
    builtins_by_opcode=dict()
    def __new__(klass, *args, **kwargs):
        if klass == Builtin:
            if len(args)==1:
                newargs=()
            elif isinstance(args[1],tuple):
                newargs=args[1]
            else:
                newargs=args[1:]
            if isinstance(args[0],str) and args[0].upper() in klass.builtins_by_name:
                ans = klass.builtins_by_name[args[0].upper()](args=args[1])
                ans.from_builtin=True
                return ans
            elif args[0] in klass.builtins_by_opcode:
                ans = klass.builtins_by_opcode[args[0]](args=args[1])
                ans.from_builtin=True
                return ans
        return super(Builtin,klass).__new__(klass)

    def __init__(self,*args,**kwargs):
        """Create a compiled MDSplus function reference.
        Number of arguments allowed depends on the opcode supplied.
        """
        if hasattr(self,'opc'):
            return
        if hasattr(self,'from_builtin'):
            args=args[1]
        elif 'args' in kwargs:
            args=kwargs['args']
        opcode=self.name
        if self.min_args is not None and len(args) < self.min_args:
            raise Exception("Builtin: %s requires at least %d arguments, %d provided" % (self.name,self.min_args,len(args)))
        if self.max_args is not None and len(args) > self.max_args:
            raise Exception("Builtin: %s requires no more than %d arguments, %d provided" % (self.name,self.max_args,len(args)))
        self.args=args
#        super(Builtin,self).__init__(opcode=opcode,args=args)

    def loadBuiltins(klass,globals_list):
        for item in globals_list.values():
            try:
                if issubclass(item,Builtin):
                    if item.name in klass.builtins_by_name:
                        raise Exception('duplicate builtin found: %s' % (item.name,))
                    else:
                        klass.builtins_by_name[item.name]=item
                    if item.opcode in klass.builtins_by_opcode:
                        raise Exception('duplicate builtin opcode found: %d' % (item.opcode))
                    else:
                        klass.builtins_by_opcode[item.opcode]=item
            except:
                pass
    loadBuiltins=classmethod(loadBuiltins)

