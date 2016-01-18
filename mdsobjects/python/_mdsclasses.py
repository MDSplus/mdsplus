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
        names={CLASS_S:'CLASS_S',CLASS_D:'CLASS_D',CLASS_A:'CLASS_A',CLASS_XD:'CLASS_XD',CLASS_XS:'CLASS_XS',CLASS_R:'CLASS_R',CLASS_CA:'CLASS_CA',CLASS_APD:'CLASS_APD'}
        if (self.mdsclass in names):
            return names[self.mdsclass]
        else:
            return 'CLASS_?'
