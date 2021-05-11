import os
import xml.etree.ElementTree as ET

srcdir = os.path.realpath(os.path.dirname(__file__)+'/../..')

def get_info():
    info = dict()
    info['buildroot'] = os.environ['BUILDROOT']
    info['branch'] = os.environ['BRANCH']
    info['dist'] = os.environ['DISTNAME']
    info['platform'] = os.environ['PLATFORM']
    info['arch'] = os.environ['ARCH']
    version = os.environ['RELEASE_VERSION'].split('.')
    info['major'] = int(version[0])
    info['minor'] = int(version[1])
    info['release'] = int(version[2])
    if info['branch'] == 'stable':
        flavor = "stable"
        rflavor = ""
    elif info['branch'] == 'alpha':
        flavor = "alpha"
        rflavor = "-alpha"
    else:
        flavor = "other"
        rflavor = "-other"
    info['flavor'] = flavor
    info['rflavor'] = rflavor
    return info


def get_root():
    return ET.parse(srcdir+'/deploy/packaging/linux.xml').getroot()


def external_package(info, root, package):
    for extpackages in root.getiterator('external_packages'):
        dist = extpackages.attrib.get('dist', None)
        if dist:
            if info['dist'] != dist:
                continue
        else:
            platform = extpackages.attrib.get('platform', None)
            if platform:
                if info['platform'] != platform:
                    continue
            else:
                continue
        pkg = extpackages.find(package)
        if pkg is not None:  # found and include dependency
            pkg = pkg.attrib.get('package', package)
            print('REQUIRES: %s' % (pkg,))
            return pkg
        print("found and dont include '%s' dependency" % package)
        return None
    # not found so dont include dependency
    return None
