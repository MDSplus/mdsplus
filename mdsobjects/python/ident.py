from mdsdata import Data
class Ident(Data):
    """Reference to MDSplus Ken Variable"""
    def __init__(self,name):
        self.name=str(name)
    def __str__(self):
        return self.name

