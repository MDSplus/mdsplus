import os,sys

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
from pkg_utils import promoteCommand
from pkg_utils import newVersionCommand
from pkg_rpms import makeRpmsCommand
from pkg_msi import makeMsiCommand
from pkg_deb import makeDebsCommand
from pkg_solaris import makeSolarisPkgsCommand
from pkg_macosx import makeMacosxPkgCommand

cmds=[]
icmds=[]

def helpCommand(args):
  if len(args) > 2 and args[2] in cmds:
    print eval(args[2]+"Command",globals()).__doc__
  else:
      print "Valid commands are:\n"
      for cmd in cmds:
        if cmd not in icmds:
          print cmd
      print "\nType python %s help command-name for more information on that command." % (args[0])

if __name__ == "__main__":
    for cmd in dir():
      if "Command" in str(cmd):
        if globals()[cmd].__doc__ is not None and "Internal" in globals()[cmd].__doc__:
          icmds.append(str(cmd)[0:-len("Command")])
        cmds.append(str(cmd)[0:-len("Command")])
      cmds.sort()
    if len(sys.argv) < 2:
        helpCommand(sys.argv)
    else:
        if sys.argv[1] in cmds:
            globals()[sys.argv[1]+"Command"](sys.argv)
        else:
            print "Invalid command: %s\n" % (sys.argv[1])
            helpCommand(sys.argv[0:-1])

