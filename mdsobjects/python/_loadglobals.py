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

    for mod in ('mdsdata','mdsscalar','mdsarray','compound','descriptor',
                'ident','apd','event','tree','scope','_mdsshr',
                'connection','mdsdcl','mdsExceptions'):
        loadmod_full(mod,gbls)

