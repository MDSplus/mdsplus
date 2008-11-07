from threading import Thread,enumerate
from fileinput import FileInput
from MDSobjects.tests import test1


class Test(Thread):
    """Thread to execute the test1.test() function"""
    def __init__(self,idx):
        Thread.__init__(self)
        self.idx=idx

    def run(self):
        """Run test1.test() function"""
        test1(True)
        print "Thread %d done\n" % (self.idx,)

def test(num=20):
    """Creates threads to simultaneously perform Tree and TreeNode methods defined in test1.test()"""
    print "%d threads will be started to perform the test1 function (Tree and TreeNode methods). Please wait for completetion of these threads" % (num,)
    for i in range(num):
        Test(i+1).start()

    for t in enumerate():
        if isinstance(t,Test):
            t.join()

