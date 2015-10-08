from unittest import TestCase,TestSuite,TextTestRunner,TestResult
import os

class cleanup(TestCase):
    if "TMPDIR" in os.environ:
        dir=os.environ["TMPDIR"]
    else:
        dir=None

    def cleanup(self):
        if cleanup.dir is not None:
            dir=cleanup.dir
            for i in os.walk(dir,False):
                for f in i[2]:
                    try:
                      os.remove(i[0]+os.sep+f)
                    except Exception:
                      import sys
                      e=sys.exc_info()[1]
                      print( e)
                for d in i[1]:
                    try:
                      os.rmdir(i[0]+os.sep+d)
                    except Exception:
                      import sys
                      e=sys.exc_info()[1]
                      print( e)
            try:
              os.rmdir(dir)
            except Exception:
              import sys
              e=sys.exc_info()[1]
              print( e)
        return
    def runTest(self):
        self.cleanup()
