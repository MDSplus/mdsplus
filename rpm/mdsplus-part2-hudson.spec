
%description
Main libraries and programs to get MDSplus operational

%prep
#%setup -q

%build
if [ "%_target" != "i686-linux" ]
then
  cd ${WORKSPACE}/x86_64/mdsplus
  ./configure --prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin64 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib64 --enable-nodebug --enable-mdsip_connections --with-gsi=/usr:gcc64
  env LANG=en_US.UTF-8 make
else
  cd ${WORKSPACE}/i686/mdsplus
  ./configure --prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin32 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib32 --enable-nodebug --target=i686-linux --enable-mdsip_connections --with-gsi=/usr:gcc32
  env LANG=en_US.UTF-8 make
fi


%install

if [ "%_target" != "i686-linux" ]
then
  cd ${WORKSPACE}/x86_64/mdsplus
  env MDSPLUS_VERSION="--PYTHONFLAVOR----VERSION--.--RELEASE--" make install
  rsync -a ${WORKSPACE}/i686/mdsplus/bin32 $RPM_BUILD_ROOT/usr/local/mdsplus/
  rsync -a ${WORKSPACE}/i686/mdsplus/lib32 $RPM_BUILD_ROOT/usr/local/mdsplus/
  rsync -a ${WORKSPACE}/i686/mdsplus/uid32 $RPM_BUILD_ROOT/usr/local/mdsplus/
  rsync -a ${WORKSPACE}/i686/mdsplus/mdsobjects/python/dist $RPM_BUILD_ROOT/usr/local/mdsplus/mdsobjects/python/
else
  cd ${WORKSPACE}/i686/mdsplus
  make install
  pushd mdsobjects/python
  python setup.py bdist_egg
  rsync -a dist $RPM_BUILD_ROOT/usr/local/mdsplus/mdsobjects/python/
  popd
fi

%clean
rm -rf $RPM_BUILD_ROOT




%files



%changelog
* Wed Aug 29 2000 Basil P. DUVAL <basil.duval@epfl.ch>
- version 1.0
