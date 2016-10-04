"""
This function can be used to insert elements/entries to a list or dictionary.
apdadd(list,*arguments):       append arguments
apdadd(idx,list,*arguments)    inserts arguments at position idx
apdadd(dict,key0,value0, ... ) adds key-value pairs
"""
def apdadd(*args):
    from MDSplus.apd import List, Dictionary
    def add_apd(apd,args,idx):
        if isinstance(apd, (List,)):
            if idx<0:
                idx += len(apd)+1
            for arg in args[::-1]:
                apd.insert(idx,arg.evaluate())
        elif isinstance(apd, (Dictionary)):
            if  len(args)&1:
                raise Dictionary._key_value_exception
            for idx in range(0,len(args),2):
                apd.setdefault(args[idx].evaluate(),args[idx+1].evaluate())
        else:
            raise Exception('Invalid agument class. Argument must be a List or Dictionary')
        return apd

    arg0 = args[0].getData()
    if isinstance(arg0, (List, Dictionary)):
        apd = add_apd(arg0, args[1:], len(arg0))
        var = args[0]
    else:
        var = args[1]
        apd = add_apd(var.getData(), args[2:], int(arg0))
    from MDSplus.ident import Ident
    if isinstance(var, (Ident,)):
        apd.assignTo(var.name)
    return apd
