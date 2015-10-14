if '__package__' not in globals() or __package__ is None or len(__package__)==0:
  def _mimport(name):
    return __import__(name,globals())
else:
  def _mimport(name):
    return __import__(name,globals(),{},[],1)  # 1:absolute

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
