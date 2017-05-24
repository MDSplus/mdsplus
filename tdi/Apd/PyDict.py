"""
This function can be used to generate a dictionary of descriptors
"""
def Dict(*args):
    from MDSplus.apd import Dictionary
    if len(args)==1:
        return Dictionary(args[0].evaluate())
    elif len(args)&1:
         raise Dictionary._key_value_exception
    else:
        return Dictionary((None if a is None else a.evaluate() for a in args))
