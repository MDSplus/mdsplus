Function MdsIdlImage
  case !version.os of
    'vms' : return,'mdslibidl'
    'Win32' : return,'mdslibidl'
    'AIX' : return,'libMdsLibIdl.lib'
    'IRIX' : return,'libMdsLibIdl.so'
    'OSF' : return,'libMdsLibIdl.so'
    'sunos' : return,'libMdsLibIdl.so'
    'hp-ux' : return,getenv('MDS_SHLIB_PATH')+'/libMdsLibIdl.sl'
    'linux' : return,'libMdsLibIdl.so'
    else  : message,'MDS is not supported on this platform',/IOERROR
  endcase
end
