pro mdssetcompression,level
  dummy=call_external(MdsIPImage(),'IdlSetCompressionLevel',mds$socket(),level,value=[1,1])
  return
end

