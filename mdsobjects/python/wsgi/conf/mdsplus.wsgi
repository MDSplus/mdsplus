#!/usr/bin/env python
#################################################
# MDSplus.wsgi startup script (apache2)         #
# adapt environment and place in wsgi folder    #
# e.g. '/var/www/mdsplusWsgi/'                  #
#################################################

## load environment from mdsplus.conf scripts e.g. /etc/mdsplus.conf

import os
from subprocess import Popen,PIPE
getenv = 'env -i sh -c ". /usr/local/mdsplus/setup.sh;env"'
p = Popen(getenv,stdout=PIPE,shell=True)
s = p.wait()
envs = p.stdout.read().split('\n')[:-1]
p.stdout.close()
for env in envs:
    name, value=env.split('=',2)
    os.environ[name] = value
    if name == "PYTHONPATH":       
        for path in value.split(":"):
            os.sys.path.append(path)

## add more envs here


## get application class for wsgi
import MDSplus
from MDSplus.wsgi import application

## run as cgi if executed
if __name__ == '__main__':
    if "WSGI":
        from flup.server.fcgi import WSGIServer
        WSGIServer(application).run()
    else:
        from wsgiref.simple_server import make_server
        port = int(os.sys.argv[1]) if len(os.sys.argv)>1 else 8080
        httpd = make_server('', port, application)
        print("Serving on port %d..."%port)
        # Serve until process is killed
        httpd.serve_forever()
