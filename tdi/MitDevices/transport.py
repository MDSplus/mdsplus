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
    import dt100
    # if uut is a single number -> local file transport
    return dt100.DT100(uut)
