import os,sys
def printHelp(stat):
    print "This is going to be help"
    sys.exit(stat)
    return

from pkg_utils import pkgaddCommand
from pkg_utils import pkgremoveCommand
from pkg_utils import listCommand
from pkg_utils import getVersionCommand
from pkg_utils import getReleaseTagCommand
from pkg_utils import getReleaseCommand
from pkg_utils import checkReleaseCommand
from pkg_utils import getPackagesCommand
from pkg_utils import checkReleasesCommand
from pkg_utils import newReleaseCommand
from pkg_utils import makeSrcTarCommand
from pkg_rpms import makeRepoRpmsCommand
from pkg_rpms import makeRpmsCommand
from pkg_msi import makeMsiCommand
from pkg_deb import makeDebsCommand
from pkg_solaris import makeSolarisPkgsCommand
from pkg_macosx import makeMacosxPkgCommand

if __name__ == "__main__":
#    orig_pwd=os.getcwd()
#    if os.path.dirname(sys.argv[0]) != '':
#      os.chdir(os.path.dirname(sys.argv[0]))
#    os.chdir("..")
    if len(sys.argv) < 2:
        printHelp(0)
    else:
        cmd="%sCommand" % (sys.argv[1],)
        if cmd in dir():
            eval(cmd,globals())(sys.argv)
        else:
            print "Invalid command"
            printHelp(1)

