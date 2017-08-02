def pyfun(fun,module=None,*args):
    import MDSplus
    MDSplus.DEALLOCATE('public _py_exception')
    fun = str(fun)
    if module is not None:
        module = __import__(str(module), globals(), locals(), [fun])
        fun = module.__dict__[fun]
    elif fun in __builtins__:
        fun = __builtins__[fun]
    else:
        try:
            module = __import__('MDSplus', globals(), locals(), [fun])
            fun = module.__dict__[fun]
        except:
            raise MDSplus.TdiUNKNOWN_VAR(fun)
    args = tuple(map(MDSplus.Data.evaluate,args))

    try:
        return fun(*args)
    except Exception as exc:
        import traceback
        traceback.print_exc()
        MDSplus.String(str(exc)).setTdiVar("_py_exception")
