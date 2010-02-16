import dt100

class Transport:
        def connectMaster(self):
                pass

        def connectShell(self):
                pass

        def acqcmd(self, command):
                pass

        def acq2sh(self,  command):
                pass



def factory(uut):
        # if uut is a single number -> local file transport
        return dt100.Dt100(uut)
