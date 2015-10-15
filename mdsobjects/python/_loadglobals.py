from sys import version_info as pyver
if pyver<(2,5):
    def _mimport(name, level=1):
        return __import__(name, globals())
else:
    def _mimport(name, level=1):
        return __import__(name, globals(), level=level)

def load(gbls):
  def loadmod(name,gbls):
    mod=_mimport(name)
    for key in mod.__dict__:
      if not key.startswith('_'):
        gbls[key]=mod.__dict__[key]

  for mod in ('apd','mdsarray','compound','mdsdata','ident','treenode','mdsscalar',
              'tree','mdsdevice','event','_tdishr','scope','_mdsshr','_tdishr',
              '_treeshr','tdipy','_descriptor','connection','mdsExceptions','mdsdcl'):
    loadmod(mod,gbls)
