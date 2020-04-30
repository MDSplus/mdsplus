from MARTE2_PYTHON import MARTE2_PYTHON
import pid
class MARTE2_PYTHON_PID(MARTE2_PYTHON):
    parts = MARTE2_PYTHON.buildPythonGam(pid.pygam, pid)



 
