if [ -r /usr/local/mdsplus/setup.sh ]; then
setup_file=/usr/local/mdsplus/etc/envsyms
if [ -r $setup_file ]; then
  . /usr/local/mdsplus/setup.sh
fi
setup_file=/etc/mdsplus.conf
if [ -r $setup_file ]; then
  . /usr/local/mdsplus/setup.sh
fi
setup_file=$HOME/.mdsplus
if [ -r $setup_file ]; then
. /usr/local/mdsplus/setup.sh
fi
unset setup_file
fi
