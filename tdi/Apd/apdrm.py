"""
This function can be used to remove elements/entries from a list or dictionary.
apdrm(list,*arguments):       remove values from list
apdrm(dict,*keys)             remove key-value pairs from dictionary
"""
def apdrm(var,*args):
    from MDSplus.apd import List, Dictionary
    from MDSplus.ident import Ident
    apd = var.getData()
    if isinstance(apd, (List,)):
        args = list(args)
        args.sort(reverse=True)
        for arg in args:
            del(apd[int(arg)])
    elif isinstance(apd, (Dictionary)):
        for k in args:
            del(apd[k.data()])
    else:
        raise Exception('Invalid agument class. Argument must be a List or Dictionary')
    if isinstance(var, (Ident,)):
        apd.assignTo(var.name)
    return apd
