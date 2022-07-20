import os
import sys
import xml.etree.ElementTree as ET

srcdir = os.path.realpath(os.path.dirname(__file__)+'/../..')

if sys.version_info < (3,):
    def b(s): return s
    def s(b): return b
    def writeb(f, string): os.write(f, string)
    def writes(f, string): os.write(f, string)
else:
    def b(s): return s.encode() if isinstance(s, str) else s
    def s(b): return b if isinstance(b, str) else b.decode()
    def writeb(f, string): os.write(f, b(string))
    def writes(f, string): os.write(f, s(string))

def get_info():
    info = dict()
    info['buildroot'] = os.environ['BUILDROOT']
    info['branch'] = os.environ['BRANCH']
    info['flavor'] = os.environ['FLAVOR']
    info['bname'] = os.environ['BNAME']
    info['dist'] = os.environ['DISTNAME']
    info['platform'] = os.environ['PLATFORM']
    info['arch'] = os.environ['ARCH']
    version = os.environ['RELEASE_VERSION'].split('.')
    info['major'] = int(version[0])
    info['minor'] = int(version[1])
    info['release'] = int(version[2])
    return info


def get_root():
    return ET.parse(srcdir+'/deploy/packaging/linux.xml').getroot()


def external_package(info, root, package):
    for extpackages in root.iter('external_packages'):
        dist = extpackages.attrib.get('dist', None)
        if dist:
            if info['dist'] != dist:
                continue
            selected = dist
        else:
            platform = extpackages.attrib.get('platform', None)
            if platform:
                if info['platform'] != platform:
                    continue
                selected = platform
            else:
                continue
        pkg = extpackages.find(package)
        if pkg is not None:  # found and include dependency
            pkg = pkg.attrib.get('package', package)
            print('REQUIRES: %s' % (pkg,))
            return pkg
        print('REQUIRES: %s but skipped in %s.' % (package, selected))
        return None
    # not found so dont include dependency
    print('REQUIRES: %s but no fitting external_packages found.' % (package,))
    return None
