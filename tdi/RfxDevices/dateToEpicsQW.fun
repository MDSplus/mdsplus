public fun dateToEpicsQW(in _dateTime)
{
    public __date = _dateTime;
    public __out=*;
    Py(["from MDSplus import Data, makeData",
        "import subprocess",
        "pp = Data.getTdiVar('__date')",
        "p=subprocess.Popen(['date', '-d', str(pp) , '+%s'], stdout=subprocess.PIPE)",
        "tm = (long(p.communicate()[0]) * 1000000000L) - 631148400000000000L ",
        "print tm",
        "makeData(tm).setTdiVar('__out')"]);
    return ( __out );
}
