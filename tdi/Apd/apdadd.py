"""
This function can be used to insert elements/entries to a list or dictionary.
apdadd(list,*arguments):       append arguments
apdadd(idx,list,*arguments)    inserts arguments at position idx
apdadd(dict,key0,value0, ... ) adds key-value pairs
"""
def apdadd(*args):
    from MDSplus.apd import List, Dictionary
    from MDSplus.mdsdata import Data
    from MDSplus.ident import Ident
    def add_apd(apd,args,idx):
        for i in range(len(args)):
            if isinstance(args[i],Data): args[i] = args[i].evaluate()
        if isinstance(apd, (List,)):
            if idx<0:
                idx += len(apd)+1
            for arg in args[::-1]:
                apd.insert(idx,arg)
        elif isinstance(apd, (Dictionary)):
            if  len(args)&1:
                raise Dictionary._key_value_exception
            for i in range(0,len(args),2):
                apd.setdefault(args[i],args[i+1])
        return apd
    arg0 = args[0].getData()
    if isinstance(arg0, (List, Dictionary)):
        apd = add_apd(arg0, list(args[1:]), len(arg0))
        var = args[0]
    else:
        var = args[1]
        arg1= var.getData()
        if isinstance(arg1, (List,)):
            apd = add_apd(arg1, list(args[2:]), int(arg0))
        else:
            raise TypeError('Invalid agument class. Argument must be a List or Dictionary')
    if isinstance(var, (Ident,)):
        apd.assignTo(var.name)
    return apd
