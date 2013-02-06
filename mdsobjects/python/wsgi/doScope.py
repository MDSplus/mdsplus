import os

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
    return urlStr.replace("|||", "")

def doScope(self):
  response_headers=list()
  response_headers.append(('Cache-Control','no-store, no-cache, must-revalidate'))
  response_headers.append(('Pragma','no-cache'))
  if 'user' in self.args:
      subdir=''
      dirspec=os.path.expanduser('~'+self.args['user'][-1])+"/jScope/configurations"
      if 'dir' in self.args and '..' not in self.args['dir']:
          subdir=self.args['dir'][-1]
          dirspec+=subdir
      response_headers.append(('Content-type','text/html'))
      outStr='<html><head><title>Scope Selection</title><body>'
      try:
          files=os.listdir(dirspec)
      except:
          files=''
      if(len(files) == 0):
          outStr = outStr+'No Scope configuration file found in '+dirspec+'</body></html>'            
          status = '200 OK'
          return (status, response_headers, outStr)
#list first directories
      for currFileName in files:
          if(os.path.isdir(dirspec+'/'+currFileName)):
              outStr = outStr + '<a href="?user='+self.args['user'][-1]+'&dir='+subdir+'/'+currFileName+'">'+currFileName+'</a><br>'
      outStr = outStr + '<br>'
#list actual configurations            
      for currFileName in files:
          if(not(os.path.isdir(dirspec+'/'+currFileName))):
              outStr = outStr + '<a href="scope?config='+ dirspec+'/'+currFileName +'" target="_blank">'+currFileName+'</a><br>'
      outStr = outStr + '</body></html>'
      status = '200 OK'
      return (status, response_headers, outStr)
  elif 'configxml' in self.args:

#Handle direct configuration
    response_headers.append(('IS_CONFIG','YES'))
    f = open(self.args['configxml'][-1],'r')
    lines = f.readlines()
    f.close()
    outStr = '<scope><palette>'
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
    outStr = outStr+'</palette>'
    globalTree = getValue(lines, 'Scope.global_1_1.experiment')
    globalShot = getValue(lines, 'Scope.global_1_1.shot')
         
    globalXMin = getValue(lines, 'Scope.global_1_1.xmin')
    globalXMax = getValue(lines, 'Scope.global_1_1.xmax')
    globalYMin = getValue(lines, 'Scope.global_1_1.ymin')
    globalYMax = getValue(lines, 'Scope.global_1_1.ymax')
      
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
          if(globalDefs & (1 << GLOBAL_SHOT_IDX)):
            shotNum = '-2'
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

        outStr = outStr + '>'
        numExpr = int(getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.num_expr')) 
        for exprIdx in range(1, numExpr+1):
          outStr = outStr+'<signal'
          color = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.color_'+str(exprIdx)+'_1')
          if(color != None):
            if color == 'Blak':
              color = 'Black'  #fix old config file typo
            outStr = outStr+' color="'+color+'"'
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
          xExpr = getValue(lines, 'Scope.plot_'+str(rowIdx)+'_'+str(colIdx)+'.x_expr_'+str(exprIdx))
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
