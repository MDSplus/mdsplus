if ( -r /usr/local/mdsplus/setup.csh) then
  if ( -r /usr/local/mdsplus/etc/envsyms ) source /usr/local/mdsplus/setup.csh /usr/local/mdsplus/etc/envsyms
  if ( -r /etc/mdsplus.conf ) source /usr/local/mdsplus/setup.csh /etc/mdsplus.conf
  if ( -r $HOME/.mdsplus ) source /usr/local/mdsplus/setup.csh $HOME/.mdsplus
endif
