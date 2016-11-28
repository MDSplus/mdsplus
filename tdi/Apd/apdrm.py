"""
This function can be used to remove elements/entries from a list or dictionary.
apdrm(list,*arguments):       remove values from list
apdrm(dict,*keys)             remove key-value pairs from dictionary
"""
def apdrm(var,*args):
    from MDSplus import List, Dictionary, Ident, PUBLIC, PRIVATE
    apd = var.getData()
    if isinstance(apd, (List,)):
        args = list(args)
        args.sort(reverse=True)
        for idx in args:
            del(apd[int(idx.data())])
    elif isinstance(apd, (Dictionary)):
        for key in args:
            del(apd[Dictionary.toKey(key.data())])
    else:
        raise TypeError('Invalid agument class. Argument must be a List or Dictionary')
    if isinstance(var, (Ident,PUBLIC,PRIVATE)):
        try:   var.assign(apd)
        except Exception as e: print(e)
    return apd
