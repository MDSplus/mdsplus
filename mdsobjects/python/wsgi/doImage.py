from MDSplus import Data
import sys

def doImage(self):
    if len(self.path_parts) > 2:
        self.openTree(self.path_parts[1],self.path_parts[2])
    expr=self.args['expr'][-1]
    try:
        d=Data.execute(expr)
        try:
            im=d.getImage()
        except:
            raise Exception("Expression does not evaluate to an image type")
        if im.format == "MPEG":
            response_headers=[('Content-type','video/mpeg'),('Content-Disposition','inline; filename="%s.mpeg"' % (expr,))]
        elif im.format == "GIF":
            response_headers=[('Content-type','image/gif'),('Content-Disposition','inline; filename="%s.gif"' % (expr,))]
        elif im.format == "JPEG":
            response_headers=[('Content-type','image/jpeg'),('Content-Disposition','inline; filename="%s.jpeg"' % (expr,))]
        else:
            raise Exception("not an known image type")
        output=str(d.getData().data().data)
    except Exception:
        raise Exception("Error evaluating expression: '%s', error: %s" % (expr,sys.exc_info()))
    status = '200 OK'
    return (status, response_headers, output)
