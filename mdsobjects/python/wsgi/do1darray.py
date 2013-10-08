from MDSplus import makeData,Data
import sys
    
def do1darray(self):
    if len(self.path_parts) > 2:
        self.openTree(self.path_parts[1],self.path_parts[2])
    expr=self.args['expr'][-1]
    try:
        a=makeData(Data.execute(expr).data())
    except Exception:
        raise Exception("Error evaluating expression: '%s', error: %s" % (expr,sys.exc_info()))
    response_headers=list()
    response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
    response_headers.append(('Pragma','no-cache'))
    response_headers.append(('DTYPE',a.__class__.__name__))
    response_headers.append(('LENGTH',str(len(a))))
    if self.tree is not None:
        response_headers.append(('TREE',self.tree))
        response_headers.append(('SHOT',self.shot))
    output=str(a.data().data)
    status = '200 OK'
    return (status, response_headers, output)
