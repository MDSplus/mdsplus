if ( -r /usr/local/mdsplus/setup.csh) then
  if ( -r /usr/local/mdsplus/etc/envsyms ) then
    set setup_file=/usr/local/mdsplus/etc/envsyms 
    source /usr/local/mdsplus/setup.csh /usr/local/mdsplus/etc/envsyms
    unset setup_file
  endif
  if ( -r /etc/mdsplus.conf ) then
    set setup_file=/etc/mdsplus.conf
    source /usr/local/mdsplus/setup.csh /etc/mdsplus.conf
    unset setup_file
  endif
  if ( -r $HOME/.mdsplus ) then
    set setup_file=$HOME/.mdsplus
    source /usr/local/mdsplus/setup.csh $HOME/.mdsplus
  endif
endif
