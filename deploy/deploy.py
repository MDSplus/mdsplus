import subprocess,os,sys
__doc__="""Deploy installation packages

   Format:
      python deploy.py flavor major minor release

   Where:
      flavor is one of alpha,beta or stable
      major is first number in version (i.e. in release 6.2-42, major is 6)
      minor is second number in version (i.e. in release 6.2.42, minor is 2)
      release is third number in version (i.e. in release 6.2-42, release is 42)

   Deploy does the following:

   * Determine platform and import platform specific packaging class instance.
   * Check if installation packages for this flavor and version already exits in the
     repository and just exit if they do.
   * Build new installation packages.
   * Test new installation packages.
   * Deploy installation packages to repository.

  NOTE: DIST environment variable designating the platform (i.e. el6,fc20,Ubuntu12)
        must be defined!

"""
def flushPrint(text):
  print(text)
  sys.stdout.flush()

  if "DIST" not in os.environ:
    flushPrint("DIST not defined!")
    flushPrint(__doc__)
    sys.exit(1)

  if len(sys.argv) != 5:
    flushPrint("Invalid argument list")
    flushPrint(__doc__)
    sys.exit(1)

  info={'flavor':sys.argv[1],'major':int(sys.argv[2]),'minor':int(sys.argv[3]),'release':int(sys.argv[4]),'dist':os.environ['DIST']}
  if info['flavor']=='stable':
    info['rflavor']=""
  else:
    info['rflavor']="-"+info['flavor']

  if os.environ['DIST'].startswith('el') or os.environ['DIST'].startswith('fc'):
    module=__import__('rpms',globals())
  elif os.environ['DIST'].startswith('Ubuntu'):
    module=__import__('ubuntu',globals())
  elif os.environ['DIST'].startswith('solaris'):
    module=__import__('solaris',globals())
  elif os.environ['DIST'].startswith('win'):
    module=__import__('windows',globals())
  InstallationPackage=module.InstallationPackage(info)
  if not InstallationPackage.exists():
    InstallationPackage.build()
    InstallationPackage.test()
    InstallationPackage.deploy()
