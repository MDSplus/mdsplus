
%description
Main libraries and programs to get MDSplus operational

%prep
#%setup -q
cd mdsplus

%build
if [ "%_target" != "i686-linux" ]
then
  cd i686/mdsplus
  ./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin32 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib32 --enable-nodebug --target=i686-linux --disable-java --enable-mdsip_connections
  make clean
  make
  cd ../../mdsplus
  ./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin64 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib64 --enable-nodebug --enable-mdsip_connections
  make clean
  make
else
  cd mdsplus
  ./configure --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin32 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib32 --enable-nodebug --target=i686-linux --enable-mdsip_connections
  make clean
  make
fi


%install

if [ "%_target" != "i686-linux" ]
then
  cd i686/mdsplus
  make install
  cd ../..
fi
cd mdsplus
make install

%clean
rm -rf $RPM_BUILD_ROOT




%files



%changelog
* Wed Aug 29 2000 Basil P. DUVAL <basil.duval@epfl.ch>
- version 1.0
