%define BINNAME utils
%define SPECNAME smartmet-%{BINNAME}
Summary: utils
Name: %{SPECNAME}
Version: 21.9.21
Release: 3%{?dist}.fmi
License: FMI
Group: Development/Tools
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)
BuildRequires: make bash
Requires: make bash perl

%description
FMI SmartSet server related utils

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{BINNAME}
 
%build

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%package devel
Summary: FMI SmartSet server development related utils and files
Provides: smartbuildrev
Provides: smartmkrelease
Provides: smartmktag
Provides: smartmet-makefile-inc
Requires: make bash perl

%description devel
FMI SmartSet server development related utils and files

%defattr(0775,root,root,0775)

%files devel
%defattr(0775,root,root,0775)
%{_bindir}/smartbuildrev
%{_bindir}/smartmkrelease
%{_bindir}/smartmktag
%{_datadir}/smartmet/devel/makefile.inc
%{_datadir}/smartmet/devel/makefile-abicheck.inc

%changelog
* Tue Sep 21 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.9.21-3.fmi
- Update makefile.inc and fix inc file installation

* Tue Sep 21 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.9.21-2.fmi
- Import smartmkrelease, smartmktag scripts and makefile fragments

* Tue Sep 21 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.9.21-1.fmi
- Initial version
