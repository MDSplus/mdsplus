from MDSobjects.data import Data

class Apd(Data):

    def __init__(self,descs):
        """Create a compiled MDSplus function reference.
        Number of arguments allowed depends on the opcode supplied.
        """
        if isinstance(descs,tuple):
            self.descs=descs
        else:
            raise TypeError,"must provide tuple of items when creating ApdData"

    def getDescs(self):
        return self.descs

    def getDesc(self,idx):
        return self.descs[idx]

    def setDescs(self,descs):
        self.descs=descs

    def setDesc(self,n,desc):
        l=list(self.descs)
        l[n]=desc
        self.descs=tuple(l)
