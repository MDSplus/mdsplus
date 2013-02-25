from MDSplus import makeData,Data,Tree
"""Reponds to scope queries of the format:

scopePanel?title=title&xlabel=xlabel&ylabel=ylabel&xmin=xmin&xmax=xmax&ymin=ymin&ymax=ymax&default_node=default_node&tree=tree&shot=shotlist&y1=expr&x1=expr&y2=expr&x2=expr...

where all the fields are optional except y1

Response contains the following headers, one for each expression per shot numbered from 1 to number of shots x number of y expressions:
Xn_DATATYPE
Xn_LENGTH
Yn_DATATYPE
Yn_LENGTH
or
ERRORn
"""

def doScopepanel(self):
    def getStringExp(self,name,response_headers):
        if name in self.args:
            try:
                response_headers.append((name,str(Data.execute(self.args[name][-1]).data())))
            except Exception,e:
                response_headers.append((name,str(e)))

    response_headers=list()
    response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
    response_headers.append(('Pragma','no-cache'))
    if 'tree' in self.args:
	Tree.usePrivateCtx()
        try:
            t=Tree(self.args['tree'][-1],int(self.args['shot'][-1].split(',')[0]))
        except:
            pass

    for name in ('title','xlabel','ylabel','xmin','xmax','ymin','ymax'):
        getStringExp(self,name,response_headers)

    sig_idx=0
    output=''
    if 'tree' in self.args:
        shots=self.args['shot'][-1].split(',')
        for shot in shots:
            y_idx=1
            y_idx_s='%d' % (y_idx,)
            while 'y'+y_idx_s in self.args:
                x_idx_s=y_idx_s
                sig_idx=sig_idx+1
                sig_idx_s='%d' % (sig_idx,)
                expr=self.args['y'+y_idx_s][-1]
                y_idx=y_idx+1
                y_idx_s='%d' % (y_idx,)
                try:
                    t=Tree(self.args['tree'][-1],int(shot))
                    response_headers.append(('SHOT'+sig_idx_s,str(t.shot)))
                except Exception,e:
                    response_headers.append(('ERROR'+sig_idx_s,'Error opening tree %s, shot %s, error: %s' % (self.args['tree'][-1],shot,e)))
                    continue
                if 'default_node' in self.args:
                    try:
                        t.setDefault(t.getNode(self.args['default_node'][-1]))
                    except Exception,e:
                        response_headers.append(('ERROR'+sig_idx_s,'Error setting default to %s in tree %s, shot %s, error: %s' % (self.args['default_node'][-1],
                                                                                                                                    self.args['tree'][-1],shot,e)))
                        continue
                try:
                    sig=Data.execute(expr)
                    y=makeData(sig.data())
                except Exception,e:
                    response_headers.append(('ERROR' + sig_idx_s,'Error evaluating expression: "%s", error: %s' % (expr,e)))
                    continue
                if 'x'+x_idx_s in self.args:
                    expr=self.args['x'+x_idx_s][-1]
                    try:
                        x=Data.execute(expr)
                        x=makeData(x.data())
                    except Exception,e:
                        response_headers.append(('ERROR'+sig_idx_s,'Error evaluating expression: "%s", error: %s' % (expr,e)))
                        continue
                else:
                    try:
                        x=makeData(sig.dim_of().data())
                    except Exception,e:
                        response_headers.append(('ERROR'+sig_idx_s,'Error getting x axis of %s: "%s", error: %s' % (expr,e)))
                        continue
                response_headers.append(('X'+sig_idx_s+'_DATATYPE',x.__class__.__name__))
                response_headers.append(('Y'+sig_idx_s+'_DATATYPE',y.__class__.__name__))
                response_headers.append(('X'+sig_idx_s+'_LENGTH',str(len(x))))
                response_headers.append(('Y'+sig_idx_s+'_LENGTH',str(len(y))))
                output=output+str(x.data().data)+str(y.data().data)
    else:            
        y_idx=1
        y_idx_s='%d' % (y_idx,)
        sig_idx=sig_idx+1
        sig_idx_s='%d' % (sig_idx,)
        while 'y'+y_idx_s in self.args:
            x_idx_s = y_idx_s
            expr=self.args['y'+y_idx_s][-1]
            y_idx=y_idx+1
            y_idx_s='%d' % (y_idx,)
            sig_idx=sig_idx+1
            sig_idx_s='%d' % (sig_idx,)
            try:
                sig=Data.execute(expr)
                y=makeData(sig.data())
            except Exception,e:
                response_headers.append(('ERROR' + sig_idx_s,'Error evaluating expression: "%s", error: %s' % (expr,e)))
                continue
            if 'x'+x_idx_s in self.args:
                expr=self.args['x'+x_idx_s][-1]
                try:
                    x=Data.execute(expr)
                    x=makeData(x.data())
                except Exception,e:
                    response_headers.append(('ERROR'+sig_idx_s,'Error evaluating expression: "%s", error: %s' % (expr,e)))
                    continue
            else:
                try:
                    x=makeData(sig.dim_of().data())
                except Exception,e:
                    response_headers.append(('ERROR'+sig_idx_s,'Error getting x axis of %s: "%s", error: %s' % (expr,e)))
                    continue
            response_headers.append(('X'+sig_idx_s+'_DATATYPE',x.__class__.__name__))
            response_headers.append(('Y'+sig_idx_s+'_DATATYPE',y.__class__.__name__))
            response_headers.append(('X'+sig_idx_s+'_LENGTH',str(len(x))))
            response_headers.append(('Y'+sig_idx_s+'_LENGTH',str(len(y))))
            output=output+str(x.data().data)+str(y.data().data)
    return ('200 OK', response_headers, output)
