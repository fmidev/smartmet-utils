%define BINNAME utils
%define SPECNAME smartmet-%{BINNAME}
Summary: utils
Name: %{SPECNAME}
Version: 22.1.20
Release: 1%{?dist}.fmi
License: FMI
Group: Development/Tools
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)
BuildRequires: make bash
Requires: make bash perl git

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
%{_bindir}/smartbuildcfg
%{_bindir}/smartbuildrev
%{_bindir}/smartmkrelease
%{_bindir}/smartmktag
%{_datadir}/smartmet/devel/makefile.inc
%{_datadir}/smartmet/devel/makefile-abicheck.inc

%changelog
* Thu Jan 20 2022 Andris Pavēnis <andris.pavenis@fmi.fi> 22.1.20-1.fmi
- Support PGDG gdal-3.4, geos-3.10 ja proj-8.2 packages

* Tue Nov 23 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.11.23-1.fmi
- Support PGDG gdal-3.3 packages

* Thu Nov  4 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.11.4-1.fmi
- Add initial version of script smartbuildcfg

* Mon Oct 25 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.10.25-1.fmi
- makefile.inc: add providing rpmbuild options

* Thu Sep 30 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.9.30-1.fmi
- Update and add some tests for makefile.inc

* Fri Sep 24 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.9.24-1.fmi
- Update Circel-CI support, do not fail if no C++ compiler found

* Tue Sep 21 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.9.21-3.fmi
- Update makefile.inc and fix inc file installation

* Tue Sep 21 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.9.21-2.fmi
- Import smartmkrelease, smartmktag scripts and makefile fragments

* Tue Sep 21 2021 Andris Pavēnis <andris.pavenis@fmi.fi> 21.9.21-1.fmi
- Initial version
