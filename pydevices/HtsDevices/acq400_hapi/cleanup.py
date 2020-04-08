import signal
import time

def sleep(secs):
    print("sleep(%.2f)" % (secs))
    time.sleep(secs)

class ExitCommand(Exception):
    pass
    
    
def signal_handler(signal, frame):
    raise ExitCommand()

def init():
    signal.signal(signal.SIGINT, signal_handler)