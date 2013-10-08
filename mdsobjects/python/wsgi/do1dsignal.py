from MDSplus import makeData,Data
import sys

def do1dsignal(self):
    if len(self.path_parts) > 2:
        self.openTree(self.path_parts[1],self.path_parts[2])
    expr=self.args['expr'][-1]
    try:
        sig=Data.execute(expr)
        y=makeData(sig.data())
        x=makeData(sig.dim_of().data())
    except Exception:
        raise Exception("Error evaluating expression: '%s', error: %s" % (expr,sys.exc_info()))
    response_headers=list()
    response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
    response_headers.append(('Pragma','no-cache'))
    response_headers.append(('XDTYPE',x.__class__.__name__))
    response_headers.append(('YDTYPE',y.__class__.__name__))
    response_headers.append(('XLENGTH',str(len(x))))
    response_headers.append(('YLENGTH',str(len(y))))
    if self.tree is not None:
        response_headers.append(('TREE',self.tree))
        response_headers.append(('SHOT',self.shot))
    output=str(x.data().data)+str(y.data().data)
    status = '200 OK'
    return (status, response_headers, output)
