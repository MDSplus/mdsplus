Function MdsIdlImage
  case !version.os of
    'vms' : return,'mdslibidl'
    'windows' : return,'mdslibidl'
    'AIX' : return,'libMdsLibIdl.lib'
    'IRIX' : return,'libMdsLibIdl.so'
    'OSF' : return,'libMdsLibIdl.so'
    'sunos' : return,'libMdsLibIdl.so'
    'hp-ux' : return,'libMdsLibIdl.sl'
    else  : message,'MDS is not supported on this platform',/IOERROR 
  endcase
end
