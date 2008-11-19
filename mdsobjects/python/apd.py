from MDSobjects.data import Data

class Apd(Data):
    """The Apd class represents the Array of Pointers to Descriptors structure.
    This structure provides a mechanism for storing an array of non-primitive items.
    """

    def __init__(self,descs):
        """Initializes a Apd instance
        """
        if isinstance(descs,tuple):
            self.descs=descs
        else:
            raise TypeError,"must provide tuple of items when creating ApdData"
        return
    
    def __getitem__(self,idx):
        """Return descriptor(s) x.__getitem__(idx) <==> x[idx]
        @rtype: Data|tuple
        """
        try:
            return self.descs[idx]
        except:
            return None
        return

    def __setitem__(self,idx,value):
        """Set descriptor. x.__setitem__(idx,value) <==> x[idx]=value
        @rtype: None
        """
        l=list(self.descs)
        while len(l) <= idx:
            l.append(None)
        l[idx]=value
        self.descs=tuple(l)
        return None
   
    def getDescs(self):
        """Returns the descs of the Apd.
        @rtype: tuple
        """
        return self.descs

    def getDescAt(self,idx=0):
        """Return the descriptor indexed by idx. (indexes start at 0).
        @rtype: Data
        """
        return self[idx]

    def setDescs(self,descs):
        """Set the descriptors of the Apd.
        @type descs: tuple
        @rtype: None
        """
        if isinstance(descs,tuple):
            self.descs=descs
        else:
            raise TypeError,"must provide tuple"
        return

    def setDescAt(self,idx,value):
        """Set a descriptor in the Apd
        """
        self[idx]=value
        return None
