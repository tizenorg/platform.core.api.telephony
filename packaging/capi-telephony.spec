Name:       capi-telephony
Summary:    Telephony Core API
Version:    0.1.47
Release:    1
Group:      System/Libraries
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(tapi)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(openssl)
BuildRequires: pkgconfig(capi-system-info)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description

%package devel
Summary:  Telephony Core API
Group:    TO_BE/FILLED_IN
Requires: %{name} = %{version}-%{release}

%description devel

%prep
%setup -q

%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} \
	-DLIB_INSTALL_DIR=%{_libdir} \
	-DFULLVER=%{version} \
	-DMAJORVER=${MAJORVER}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/usr/share/license

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest capi-telephony.manifest
%{_libdir}/libcapi-telephony.so.*
/usr/share/license/capi-telephony

%files devel
%{_includedir}/telephony/telephony.h
%{_includedir}/telephony/telephony_common.h
%{_includedir}/telephony/telephony_call.h
%{_includedir}/telephony/telephony_sim.h
%{_includedir}/telephony/telephony_network.h
%{_includedir}/telephony/telephony_modem.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-telephony.so
