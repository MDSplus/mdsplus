# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import os
import sys
from doEvent import noCache
from MDSplus import Data

def getValue(lines, key):
    i = 0;
    for currLine in lines:
        if(currLine.strip() != ''):
            splitted = currLine.split(':', 1)
        if (splitted[0] == key):
            return splitted[1].strip()
    return None

def encodeUrl(inStr):
    urlStr = inStr.replace("%", "%25"); 
    urlStr = urlStr.replace("/", "%2F"); 
    urlStr = urlStr.replace("#", "%23"); 
    urlStr = urlStr.replace("?", "%3F"); 
    urlStr = urlStr.replace("&", "%26"); 
    urlStr = urlStr.replace(",", "%2C"); 
    urlStr = urlStr.replace("=", "%3D"); 
    urlStr = urlStr.replace("@", "%40"); 
    urlStr = urlStr.replace(";", "%3B"); 
    urlStr = urlStr.replace("$", "%24"); 
    urlStr = urlStr.replace("+", "%2B"); 
    urlStr = urlStr.replace(" ", "%20");
    urlStr = urlStr.replace(">", "%3E");
    urlStr = urlStr.replace("<", "%3C");
    urlStr = urlStr.replace(":", "%3A");
    urlStr = urlStr.replace("[", "%5B");
    urlStr = urlStr.replace("]", "%5D");
    return urlStr.replace("|||", "")

def getConfigFiles(user,jScope,subdir):
    file_list=[]
    homedir=os.path.expanduser('~'+user)
    if jScope:
        dirspec=homedir+"/jScope/configurations"
    else:
        dirspec=homedir+"/scopes"
    if len(subdir) > 0:
        dirspec+=subdir
    try:
        files=os.listdir(dirspec)
        for f in files:
            filespec=dirspec+'/'+f
            isdir=os.path.isdir(filespec)
            if f.endswith('.jscp') or f.endswith('.dat') or f.endswith('.scope') or isdir:
                file_list.append({'file':f,'filespec':filespec,'jScope':jScope,'notdir':not isdir,'subdir':subdir})
    except:
        pass
    return file_list

def file_list_cmp(x,y):
    ans=0
    for part in ('notdir','jScope','filespec'):
        ans=cmp(x[part],y[part])
        if ans != 0:
            break
    return ans

def doScope(self):
  response_headers=list()
  noCache(response_headers)
  overallShot = None
  if 'user' in self.args:
      user=self.args['user'][-1]
      response_headers.append(('Content-type','text/html'))
      outStr='<html><head><title>Scope Selection</title><body>'
      subdir=''
      if 'dir' in self.args and '..' not in self.args['dir'][-1]:
          file_list=getConfigFiles(user,'jScope' in self.args,self.args['dir'][-1])
      else:
          file_list=getConfigFiles(user,True,'')
          file_list.extend(getConfigFiles(user,False,''))
      if(len(file_list) == 0):
          outStr = outStr+'No Scope configuration file found</body></html>'            
          status = '200 OK'
          return (status, response_headers, outStr)
      file_list.sort(file_list_cmp)
      last_is_dir=False
      for f in file_list:
          if not f['notdir']:
              last_is_dir=True
              if f['jScope']:
                  jscope='&jScope=yes'
              else:
                  jscope=''
              outStr = outStr + '<a href="?user='+user+'&dir='+f['subdir']+'/'+f['file']+jscope+'">'+f['filespec']+'</a><br>'
          else:
              if last_is_dir:
                  outStr = outStr + '<br>'
              last_is_dir=False
              outStr = outStr + '<a href="scope?config='+f['filespec'] +'" target="_blank">'+f['filespec']+'</a><br>'
      outStr = outStr + '</body></html>'
      status = '200 OK'
      return (status, response_headers, outStr)
  elif 'configxml' in self.args:

#Handle direct configuration
    response_headers.append(('IS_CONFIG','YES'))
    response_headers.append(('Content-type','text/xml'))
    if 'shot' in self.args:
        overallShot = self.args['shot'][-1]
    f = open(self.args['configxml'][-1],'r')
    lines = f.readlines()
    f.close()
    outStr = '<scope>'
    title=getValue(lines, 'Scope.title')
    if title:
      outStr = outStr + '<title><expression>'+encodeUrl(title)+'</expression>'
      event=getValue(lines,'Scope.update_event')
      if event:
        outStr = outStr + '<event>'+event+'</event>'
      outStr = outStr + '</title>'
    outStr = outStr + '<palette>'
    idx = 0
    GLOBAL_SHOT_IDX = 8
    GLOBAL_TREE_IDX = 7
    GLOBAL_XMIN_IDX = 12
    GLOBAL_XMAX_IDX = 13
    GLOBAL_YMIN_IDX = 14
    GLOBAL_YMAX_IDX = 15
    while True:
        color = getValue(lines, 'Scope.color_'+str(idx))
        if(color == None):
            break
        color = color.split(',')[0]
        if color == 'Blak':
            color = 'Black'
        outStr = outStr+'<color>'+color+'</color>'
        idx = idx + 1
#Handle missing color palette
    if (idx == 0):
        outStr = outStr+'<color>Black</color>'
    outStr = outStr+'</palette>'
    globalTree = getValue(lines, 'Scope.global_1_1.experiment')
    globalShot = getValue(lines, 'Scope.global_1_1.shot')
         
    globalXMin = getValue(lines, 'Scope.global_1_1.xmin')
    globalXMax = getValue(lines, 'Scope.global_1_1.xmax')
    globalYMin = getValue(lines, 'Scope.global_1_1.ymin')
    globalYMax = getValue(lines, 'Scope.global_1_1.ymax')
    globalXLabel = getValue(lines, 'Scope.global_1_1.x_label')
    globalYLabel = getValue(lines, 'Scope.global_1_1.y_label')  
    globalEvent = getValue(lines, 'Scope.global_1_1.event')
    numCols = int(getValue(lines, 'Scope.columns'))
    outStr = outStr+'<columns>'
    for colIdx in range(1, numCols+1):
      outStr = outStr+'<column>'
      numRows = int(getValue(lines, 'Scope.rows_in_column_'+str(colIdx)))
      for rowIdx in range(1, numRows+1):
        outStr = outStr + '<panel '
        globalDefs = int(getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.global_defaults'))
        if(globalDefs & (1 << GLOBAL_TREE_IDX)):
          tree = globalTree
        else:
          tree = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.experiment')

        if(tree != None):
          outStr = outStr+' tree = "'+tree+'" ';
          if overallShot != None:
            outStr = outStr+' shot = "'+overallShot+'" '
          else:
            if(globalDefs & (1 << GLOBAL_SHOT_IDX)):
              shotNum = globalShot
            else:
              shotNum = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.shot')
            if(shotNum != None):
              outStr = outStr+' shot = "'+shotNum+'" '

        if(globalDefs & (1 << GLOBAL_XMIN_IDX)):
          xmin = globalXMin
        else:
          xmin = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.xmin')
        if(xmin != None):
          outStr = outStr+' xmin = "'+encodeUrl(xmin)+'" '

        if(globalDefs & (1 << GLOBAL_XMAX_IDX)):
          xmax = globalXMax
        else:
          xmax = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.xmax')
        if(xmax != None):
          outStr = outStr+' xmax = "'+encodeUrl(xmax)+'" '

        if(globalDefs & (1 << GLOBAL_YMIN_IDX)):
          ymin = globalYMin
        else:
          ymin = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.ymin')
        if(ymin != None):
          outStr = outStr+' ymin = "'+encodeUrl(ymin)+'" '

        if(globalDefs & (1 << GLOBAL_YMAX_IDX)):
          ymax = globalYMax
        else:
          ymax = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.ymax')
        if(ymax != None):
          outStr = outStr+' ymax = "'+encodeUrl(ymax)+'" '

        title = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.title') 
        if(title != None):
          title = encodeUrl(title)
          title = title.replace('"', "'")
          outStr = outStr+' title = "'+title+'"'
        xlabel = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.x_label')
        if (xlabel == None):
          xlabel = globalXLabel
        if (xlabel != None):
          xlabel = encodeUrl(xlabel)
          xlabel = xlabel.replace('"',"'")
          outStr = outStr+' xlabel = "'+xlabel+'"'
        ylabel = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.y_label')
        if (ylabel == None):
          ylabel = globalYLabel
        if (ylabel != None):
          ylabel = encodeUrl(ylabel)
          ylabel = ylabel.replace('"',"'")
          outStr = outStr+' ylabel = "'+ylabel+'"'
        event = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.event')
        if (event == None):
          event = globalEvent
        if (event != None):
          outStr = outStr+' event = "'+event+'"'
        continuous_update = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.continuous_update')
        if (continuous_update != None):
          outStr = outStr+' continuous_update = "'+continuous_update+'"'
        is_image = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.is_image')
        if (is_image != None and is_image == 'true'):
          outStr = outStr+' is_image = "true"'
        keep_ratio = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.keep_ratio')
        if (keep_ratio != None):
          outStr = outStr+' keep_ratio = "'+keep_ratio+'"'
        horizontal_flip = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.horizontal_flip')
        if (horizontal_flip != None):
          outStr = outStr+' horizontal_flip = "'+horizontal_flip+'"'
        vertical_flip = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.vertical_flip')
        if (vertical_flip != None):
          outStr = outStr+' vertical_flip = "'+vertical_flip+'"'
        palette = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.palette')
        if (palette != None):
          outStr = outStr+' palette = "'+palette+'"'
        bitShift = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.bitShift')
        if (bitShift != None):
          outStr = outStr+' bitShift = "'+bitShift+'"'
        bitClip = getValue(lines,'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.bitClip')
        if (bitClip != None):
          outStr = outStr+' bitClip = "'+bitClip+'"'

        outStr = outStr + '>'
        numExprStr = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.num_expr')
        if(numExprStr == None):
            numExpr = 1
        else: 
            numExpr = int(numExprStr) 
        for exprIdx in range(1, numExpr+1):
          outStr = outStr+'<signal'
          color = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.color_'+str(exprIdx)+'_1')
          if(color != None):
            if color == 'Blak':
              color = 'Black'  #fix old config file typo
            outStr = outStr+' color="'+color+'"'
          else:
            outStr = outStr + ' color="Black"'
          mode = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.mode_1D_'+str(exprIdx)+'_1')
          marker = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.marker_'+str(exprIdx)+'_1')
          if(mode == 'Line' and marker == '0'):
            outStr = outStr+ ' mode="1"'
          elif(mode == 'Line' and marker != '0'):
            outStr = outStr+ ' mode="3"'
          elif(mode == 'Noline' and marker != '0'):
            outStr = outStr+ ' mode="2"'
          outStr = outStr+'>'
          yExpr = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.y_expr_'+str(exprIdx))
          if(yExpr == None):
              yExpr = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.y')
          xExpr = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.x_expr_'+str(exprIdx))
          if(xExpr == None):
              xExpr = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.x')
          if(xExpr == None):
            outStr = outStr+encodeUrl(yExpr)+'</signal>'
          else:
            outStr = outStr+'BUILD_SIGNAL('+encodeUrl(yExpr)+',,'+encodeUrl(xExpr)+')</signal>'
        outStr = outStr+'</panel>\n'
      outStr = outStr+'</column>\n\n'
    outStr = outStr+'</columns></scope>'
    output=str(outStr)
    status = '200 OK'
    return (status, response_headers, output)
  elif 'panel' in self.args:
    return doScopepanel(self)
  elif 'title' in self.args:
    response_headers.append(('Content-type','text/text'))
    try:
      output = str(Data.execute(self.args['title'][0]))
    except Exception:
      output = str(sys.exc_info())+' expression was '+self.args['title'][0]
    return ('200 OK',response_headers, output)
  elif 'colortables' in self.args:
    response_headers.append(('Content-type','application/octet-stream'))
    try:
      f=open(os.path.dirname(__file__)+'/colors1.tbl',"r")
      output=f.read()
      f.close()
    except:
      response_headers.append(('ERROR', str(sys.exc_info())))
      output=''
    return ('200 OK',response_headers, output)
  else:
    ans=('400 NOT FOUND',[('Content-type','text/text'),],'')
    try:
      f=open(os.path.dirname(__file__)+'/../html/scope.html',"r")
      contents=f.read()
      f.close()
      ans = ('200 OK',[('Content-type','text/html'),],contents)
    except:
      pass
    return ans
from MDSplus import makeData,Data,Tree,Float64Array
from numpy import finfo,integer
from math import floor
"""Reponds to scope queries of the format:

scopePanel?title=title&xlabel=xlabel&ylabel=ylabel&xmin=xmin&xmax=xmax&ymin=ymin&ymax=ymax&default_node=default_node&tree=tree&shot=shotlist&continuous_update=continuous_update&displayed_width=displayed_width&y1=expr&x1=expr&curr_max_x1=curr_max_x1&y2=expr&x2=expr&curr_max_x2=curr_max_x2...

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
            except Exception:
                response_headers.append((name + '_error:',str(sys.exc_info())))

    def manageContinuousUpdate(self,x):
          if x.getShape().size == 1:
              new_max_x=x.data()[-1]
          else:
              new_max_x=x.data()[0][-1]
          curr_max_x=float(self.args['curr_max_x'+x_idx_s][-1])
          displayed_width=float(self.args['displayed_width'][-1])
          default_num_samples=float(self.args['default_num_samples'][-1])
          num_samples=int((new_max_x-curr_max_x)*default_num_samples/displayed_width)
          if new_max_x > curr_max_x and num_samples > 1:
              if issubclass(x.dtype.type, integer):
                  min_req_x=floor(curr_max_x+1)
              else:
                  min_req_x=curr_max_x+finfo(new_max_x).eps
              update_function='MdsMisc->GetXYWave:DSC'
              infinity=sys.float_info.max
              if x.__class__.__name__ == 'Uint64Array' or x.__class__.__name__ == 'Int64Array':
                  update_function='MdsMisc->GetXYWaveLongTimes:DSC'
                  min_req_x=str(int(min_req_x))+'QU'
                  infinity=str((1<<63)-1)+'QU'
              sig=Data.execute(update_function+'(\''+expr.replace('\\','\\\\').replace('\'', '\\\'')+'\','+str(min_req_x)+','+str(infinity)+','+str(num_samples)+')')
              return makeData(sig.dim_of().data()),makeData(sig.data())
          else:
              return Float64Array([]),Float64Array([])

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

    continuous_update = 'continuous_update' in self.args

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
                except Exception:
                    response_headers.append(('ERROR'+sig_idx_s,'Error opening tree %s, shot %s, error: %s' % (self.args['tree'][-1],shot,sys.exc_info())))
                    continue
                if 'default_node' in self.args:
                    try:
                        t.setDefault(t.getNode(self.args['default_node'][-1]))
                    except Exception:
                        response_headers.append(('ERROR'+sig_idx_s,'Error setting default to %s in tree %s, shot %s, error: %s' % (self.args['default_node'][-1],
                                                                                                                                    self.args['tree'][-1],shot,sys.exc_info())))
                        continue
                try:
                    sig=Data.execute(expr)
                    if not continuous_update:
                        y=makeData(sig.data())
                except Exception:
                    response_headers.append(('ERROR' + sig_idx_s,'Error evaluating expression: "%s", error: %s' % (expr,sys.exc_info())))
                    continue
                if 'x'+x_idx_s in self.args:
                    expr=self.args['x'+x_idx_s][-1]
                    try:
                        x=Data.execute(expr)
                        x=makeData(x.data())
                        if continuous_update:
                            x,y=manageContinuousUpdate(self,x)
                    except Exception:
                        response_headers.append(('ERROR'+sig_idx_s,'Error evaluating expression: "%s", error: %s' % (expr,sys.exc_info())))
                        continue
                else:
                    try:
                        x=makeData(sig.dim_of().data())
                        if continuous_update:
                            x,y=manageContinuousUpdate(self,x)
                    except Exception:
                        response_headers.append(('ERROR'+sig_idx_s,'Error getting x axis of %s:, error: %s' % (expr,sys.exc_info())))
                        continue
                if x.__class__.__name__ == 'Uint64Array' or x.__class__.__name__ == 'Int64Array':
                    x=Float64Array(x)     # Javascript doesn't support 64 bit integers
                    response_headers.append(('X_IS_DATETIME','true'))
                response_headers.append(('X'+sig_idx_s+'_DATATYPE',x.__class__.__name__))
                response_headers.append(('Y'+sig_idx_s+'_DATATYPE',y.__class__.__name__))
                response_headers.append(('X'+sig_idx_s+'_LENGTH',str(len(x))))
                response_headers.append(('Y'+sig_idx_s+'_LENGTH',str(len(y))))
                output=output+str(x.data().data)+str(y.data().data)
    else:            
        y_idx=1
        y_idx_s='%d' % (y_idx,)
        while 'y'+y_idx_s in self.args:
            x_idx_s = y_idx_s
            expr=self.args['y'+y_idx_s][-1]
            y_idx=y_idx+1
            y_idx_s='%d' % (y_idx,)
            sig_idx=sig_idx+1
            sig_idx_s='%d' % (sig_idx,)
            try:
                sig=Data.execute(expr)
                if not continuous_update:
                    y=makeData(sig.data())
            except Exception:
                response_headers.append(('ERROR' + sig_idx_s,'Error evaluating expression: "%s", error: %s' % (expr,sys.exc_info())))
                continue
            if 'x'+x_idx_s in self.args:
                expr=self.args['x'+x_idx_s][-1]
                try:
                    x=Data.execute(expr)
                    x=makeData(x.data())
                    if continuous_update:
                        x,y=manageContinuousUpdate(self,x)
                except Exception:
                    response_headers.append(('ERROR'+sig_idx_s,'Error evaluating expression: "%s", error: %s' % (expr,sys.exc_info())))
                    continue
            else:
                try:
                    x=makeData(sig.dim_of().data())
                    if continuous_update:
                        x,y=manageContinuousUpdate(self,x)
                except Exception:
                    response_headers.append(('ERROR'+sig_idx_s,'Error getting x axis of %s: "%s", error: %s' % (expr,sys.exc_info())))
                    continue
            if x.__class__.__name__ == 'Uint64Array' or x.__class__.__name__ == 'Int64Array':
                x=Float64Array(x)     # Javascript doesn't support 64 bit integers
                response_headers.append(('X_IS_DATETIME','true'))
            response_headers.append(('X'+sig_idx_s+'_DATATYPE',x.__class__.__name__))
            response_headers.append(('Y'+sig_idx_s+'_DATATYPE',y.__class__.__name__))
            response_headers.append(('X'+sig_idx_s+'_LENGTH',str(len(x))))
            response_headers.append(('Y'+sig_idx_s+'_LENGTH',str(len(y))))
            output=output+str(x.data().data)+str(y.data().data)
    return ('200 OK', response_headers, output)
