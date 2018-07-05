Function MdsIdlImage
  case !version.os of
    'Win32' : return,'mdslibidl'
    'AIX' : return,'libMdsLibIdl.a'
    'IRIX' : return,'libMdsLibIdl.so'
    'OSF' : return,'libMdsLibIdl.so'
    'sunos' : return,'libMdsLibIdl.so'
    'hp-ux' : begin
      a = getenv('MDS_SHLIB_PATH')
      if (a eq '') then a = '/usr/local/lib'
      return,a+'/libMdsLibIdl.sl'
    end
    'linux' : return,'libMdsLibIdl.so'
    'darwin': return,'libMdsLibIdl.dylib'
    else  : message,'MDS is not supported on this platform',/IOERROR
  endcase
end
