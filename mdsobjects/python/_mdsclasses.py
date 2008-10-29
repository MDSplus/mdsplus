CLASS_S=1
CLASS_D=2
CLASS_A=4
CLASS_XD=192
CLASS_XS=193
CLASS_R=194
CLASS_CA=195
CLASS_APD=196

class mdsclasses:
    mdsclass=-1

    def __init__(self,mdsclass):
        self.mdsclass=mdsclass

    def __str__(self):
        names={1:'CLASS_S',2:'CLASS_D',4:'CLASS_A',192:'CLASS_XD',193:'CLASS_XS',194:'CLASS_R',195:'CLASS_CA',196:'CLASS_APD'}
        if (self.mdsclass in names):
            return names[self.mdsclass]
        else:
            return 'CLASS_?'
