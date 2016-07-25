def load(gbls):
    def loadmod(name, gbls=globals()):
        try:
            return __import__(name, gbls, level=1)
        except:
            return __import__(name, gbls)

    def loadmod_full(name,gbls):
        mod=loadmod(name)
        for key in mod.__dict__:
            if not key.startswith('_'):
                gbls[key]=mod.__dict__[key]

    for mod in ('apd','mdsarray','compound','mdsdata','ident','mdsscalar',
                'tree','mdsdevice','event','scope','_mdsshr',
                'connection','mdsdcl'):
        loadmod_full(mod,gbls)
    for mod in ('mdsExceptions', 'tdibuiltins'):
        loadmod_full(mod, gbls)

