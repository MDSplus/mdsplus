from MDSplus import makeData,Data,Tree,Float64Array
import sys

def doFrame(self):
    def getStringExp(self,name,response_headers):
        if name in self.args:
            try:
                response_headers.append((name,str(Data.execute(self.args[name][-1]).data())))
            except Exception:
                response_headers.append((name,str(sys.exc_info())))

    response_headers = list()
    response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
    response_headers.append(('Pragma','no-cache'))
    response_headers.append(('Content-Type','application/octet-stream'))

    if 'tree' in self.args:
        Tree.usePrivateCtx()
        try:
            t = Tree(self.args['tree'][-1],int(self.args['shot'][-1].split(',')[0]))
        except:
            response_headers.append(('ERROR','Error opening tree'))

    for name in ('title','xlabel','ylabel'):
        getStringExp(self,name,response_headers)

    if 'frame_idx' in self.args:
        frame_idx = self.args['frame_idx'][-1]
    else:
        frame_idx = '0'

    expr = self.args['y'][-1]
    try:
        sig = Data.execute('GetSegment(' + expr + ',' + frame_idx + ')')
        frame_data = makeData(sig.data())
    except Exception:
        response_headers.append(('ERROR','Error evaluating expression: "%s", error: %s' % (expr,sys.exc_info())))

    if 'init' in self.args:
        if 'x' in self.args:
            expr = self.args['x'][-1]
            try:
                times = Data.execute(expr)
                times = makeData(times.data())
            except Exception:
                response_headers.append(('ERROR','Error evaluating expression: "%s", error: %s' % (expr,sys.exc_info())))
        else:
            try:
                #times = Data.execute('dim_of(' + expr + ')')
                times = list()
                numSegments = Data.execute('GetNumSegments(' + expr + ')').data()
                for i in range(0, numSegments):
                    times.append(Data.execute('GetSegmentLimits(' + expr + ',' + str(i) + ')').data()[0])
                times = Float64Array(times)
            except Exception:
                response_headers.append(('ERROR','Error getting x axis of: "%s", error: %s' % (expr,sys.exc_info())))
    
    response_headers.append(('FRAME_WIDTH',str(sig.getShape()[0])))
    response_headers.append(('FRAME_HEIGHT',str(sig.getShape()[1])))
    response_headers.append(('FRAME_BYTES_PER_PIXEL',str(frame_data.data().itemsize)))
    response_headers.append(('FRAME_LENGTH',str(len(frame_data))))

    output = str(frame_data.data().data)

    if 'init' in self.args:
        response_headers.append(('TIMES_DATATYPE',times.__class__.__name__))
        response_headers.append(('TIMES_LENGTH',str(len(times))))
        output = output + str(times.data().data)

    status = '200 OK'
    return (status, response_headers, output)
