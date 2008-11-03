class A12(object):
    def __init__(self,node):
        self.node=node
        return

    def init(self,arg):
        print "Doing the init method on conglomerate whose top node is: "+str(self.node)
        print "arg="+str(arg)
        return 1
