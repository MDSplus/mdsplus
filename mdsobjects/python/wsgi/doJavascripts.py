import os
def doJavascripts(self):
  ans=('400 NOT FOUND',[('Content-type','text/javascript'),],'')
  jsfile=self.path_parts[1]
  try:
    f=open(os.path.dirname(__file__)+'/../js/'+jsfile,"r")
    contents=f.read()
    f.close()
    ans = ('200 OK',[('Content-type','text/javascript'),],contents)
  except:
    pass
  return ans
