function mdsisclient
  defsysv,'!MDS_SOCKET',exists=mdsClient
  return,mdsClient
end
