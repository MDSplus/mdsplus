#!/usr/bin/env python
#################################################
# MDSplus.wsgi startup script (apache2)         #
# adapt environment and place in wsgi folder    #
# e.g. '/var/www/mdsplusWsgi/'                  #
#################################################

from MDSplus import setenv

## load environment from mdsplus.conf scripts e.g. /etc/mdsplus.conf

from subprocess import Popen,PIPE
getenv = 'env -i sh -c ". /usr/local/mdsplus/setup.sh;env"'
p = Popen(getenv,stdout=PIPE,shell=True)
s = p.wait()
envs = p.stdout.read().split('\n')[:-1]
p.stdout.close()
for env in envs:
    name,value=env.split('=')
    setenv(name,value)

## add more envs here


## get application class for wsgi
from MDSplus.wsgi import application

## run as cgi if executed
if __name__ == '__main__':
    from wsgiref.simple_server import make_server
    httpd = make_server('', 8080, application)
    print("Serving on port 8080...")
    # Serve until process is killed
    httpd.serve_forever()
