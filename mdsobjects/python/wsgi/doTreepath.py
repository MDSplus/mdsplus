import os

def doTreepath(self):
    tree=self.path_parts[1].lower()
    if tree+'_path' in os.environ:
        return ('200 OK', [('Content-type','text/text')],os.environ[tree+'_path'])
    else:
        return ('400 BAD_REQUEST',[('Content-type','text/text')],'No path defined for '+tree)
