from threading import Thread,enumerate
from fileinput import FileInput
from MDSobjects.tests import test1


class Test(Thread):
    def __init__(self,idx):
        Thread.__init__(self)
        self.idx=idx

    def run(self):
        test1(True)
        print "Thread %d done\n" % (self.idx,)

def test(num=20):
    print "%d threads will be started to perform the test1 function (Tree and TreeNode methods). Please wait for completetion of these threads" % (num,)
    for i in range(num):
        Test(i+1).start()

    for t in enumerate():
        if isinstance(t,Test):
            t.join()

