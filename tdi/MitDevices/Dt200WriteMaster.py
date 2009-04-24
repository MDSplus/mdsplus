def Dt200WriteMaster(board, cmd, rsh):
  print "Dt200WriteMaster(%d, %s, %d)" % (board, cmd, rsh)
  if rsh :
    file = '/dev/acq200/acq200.%d.rsh' % board
  else :
    file = '/dev/acq32/acq32.%d.m%d' % (board, board)
  try:
    f = open(file, 'rw+')
  except Exception,e:
    print "Error opening D-Tacq device %s\n\t%s" % (file ,str(e),)
    raise e
  try:  
    f.write(cmd+'\n')
  except Exception,e:
    print "Error writing %s to D-Tacq device %s\n\t%s" % (cmd, file ,str(e),)
    raise e
  try:
    ans = f.readlines()
  except Exception,e:
    print "Error reading answer to %s from D-Tacq device %s\n\t%s" % (cmd, file ,str(e),)
    raise e
  f.close()
  print "\t%s" % ans
  return(ans)
