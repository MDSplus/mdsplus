import os, MDSplus, numpy
def getShotDB(expt,path=None,lower=None,upper=None):
    """
    getShotDB("mytree")
    getShotDB("mytree",0)
    getShotDB("mytree","/my/local/tree/path")
    getShotDB("mytree","myserver::")
    getShotDB("mytree","myserver::/my/remote/tree/path")
    getShotDB("mytree",*,_from)
    getShotDB("mytree",*,*,_upto)
    getShotDB("mytree",*,_from,_upto)
    """
    isTdi = isinstance(expt, MDSplus.mdsdata.Data)
    expt = str(expt).lower()

    def getTreePath():
        # split path variable into single paths and replaces the ~t treename
        expt_paths = os.getenv(expt+'_path')
        if expt_paths is None:
            raise MDSplus.mdsExceptions.TreeNOPATH
        return expt_paths.replace('~t',expt).split(';')

    def getshots(expt_path,lower,upper):
        if expt_path.find('::')>=0:
            # path is referring to remote tree
            server,path = expt_path.split('::',2)
            # check if thick or distributed
            path = "*"  if len(path)==0 else '"'+path+'"'
            # handle None for upper and lower
            if lower is None: lower = "*"
            if upper is None: upper = "*"
            # fetch data from server
            return MDSplus.Connection(server).get('getShotDb("%s",%s,%s,%s)'%(expt,path,str(lower),str(upper)),).data().tolist()
        start = expt+'_'
        files = [f[len(expt):-5].split('_') for f in os.listdir(expt_path) if f.endswith('.tree') and f.startswith(start)]
        return [int(f[1]) for f in files if len(f)==2 and f[1]!='model']
    """The path argument is interpreted"""
    # try to convert to native datatype
    try: path = path.data().tolist()
    except: pass
    if isinstance(path, (int,)):
        # path is int and refering to the index of the path list
        path = getTreePath()[path]
    if isinstance(path, MDSplus.version.basestring):
        # path is str and used as path
        shots = getshots(path,lower,upper)
    else:
        # path is undefined and the total list will be collected
        shots = []
        for expt_path in getTreePath():
            try:
                shots += getshots(expt_path,lower,upper)
            except:
                import traceback
                traceback.print_exc()

    """filter result by upper and lower limits"""
    if lower is not None or upper is not None:
        if lower is None:
            shots = numpy.array(filter(lambda x: x <= int(upper), shots))
        elif upper is None:
            shots = numpy.array(filter(lambda x: x >= int(lower), shots))
        else:
            shots = numpy.array(filter(lambda x: x >= int(lower) and x <= int(upper), shots))
    shots.sort()

    # return as MDSplus type if input was MDSplus type
    return MDSplus.Int32Array(shots) if isTdi else shots
