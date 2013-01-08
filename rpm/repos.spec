Name: mdsplus-%flavor-repo
Version: 1.0
Release: 0%{dist}
License: BSD Open Source
Summary: The MDSplus Data System
Group: Applications/Acquisition
Prefix: /
Summary: MDSplus Data Acquisition System
AutoReqProv: no

%description
MDSplus Yum repository setup

%prep
%build

%install
mkdir -p $RPM_BUILD_ROOT/etc/yum.repos.d
mkdir -p $RPM_BUILD_ROOT/etc/pki/rpm-gpg/
cp ${WORKSPACE}/x86_64/mdsplus/rpm/RPM-GPG-KEY-MDSplus $RPM_BUILD_ROOT/etc/pki/rpm-gpg/
outfile=$RPM_BUILD_ROOT/etc/yum.repos.d/mdsplus-%flavor.repo
if [ "%flavor" == "stable" ]
then
  echo [MDSplus] > $outfile
else
  echo [MDSplus-%flavor] > $outfile
fi
cat - >> $outfile <<EOF
name=MDSplus-%flavor 
baseurl=http://www.mdsplus.org/dist/%{s_dist}/%{flavor}/RPMS
enabled=1
EOF
if [ "%_target" != "i686-linux" ]
then
echo "exclude=*i686*" >> $outfile
fi
cat - >>$outfile <<EOF 
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus
metadata_expire=300
EOF


%clean
rm -Rf $RPM_BUILD_ROOT

%files
/etc/yum.repos.d/mdsplus-%flavor.repo
/etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus

%post
dummy=$(rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus 2>/dev/null)

%postun
nohup rpm -e gpg-pubkey-b09cb563 >dev/null 2>&1 &

