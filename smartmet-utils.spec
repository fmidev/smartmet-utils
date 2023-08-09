%define BINNAME utils
%define SPECNAME smartmet-%{BINNAME}
Summary: utils
Name: %{SPECNAME}
Version: 23.8.9
Release: 2%{?dist}.fmi
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
Provides: smartpngdiff
Provides: smartrpmsort
Provides: smartmet-makefile-inc
Requires: make bash perl
Requires: ImageMagick
Requires: perl-Carp-Always
Requires: perl-Data-Dumper
Requires: perl-Getopt-Long
Requires: perl-JSON-PP
#TestRequires: bc
#TestRequires: gcc-c++
#TestRequires: geos311-devel
#TestRequires: gdal35-devel
#TestRequires: libcurl-devel

%if 0%{?rhel} && 0%{rhel} == 7
#TestRequires: proj72-devel
#TestdRequires: sqlite33-devel
%endif

%if 0%{?rhel} && 0%{rhel} >= 8
#TestRequires: proj90-devel
#TestRequires: sqlite-devel
%endif

%description devel
FMI SmartSet server development related utils and files

%defattr(0775,root,root,0775)

%files devel
%defattr(0775,root,root,0775)
%{_bindir}/smartbuild
%{_bindir}/smartbuildcfg
%{_bindir}/smartbuildrev
%{_bindir}/smartcxxcheck
%{_bindir}/smartmkciconfig
%{_bindir}/smartmkrelease
%{_bindir}/smartmktag
%{_bindir}/smartpngdiff
%{_bindir}/smartrpmsort
%{_datadir}/smartmet/devel/makefile.inc
%{_datadir}/smartmet/devel/makefile-abicheck.inc

%changelog
* Wed Aug  9 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.8.9-2.fmi
- More smartbuild fixes

* Wed Aug  9 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.8.9-1.fmi
- smartbuild updates

* Mon Jul 17 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.7.17-1.fmi
- New script smartrpmsort

* Tue Jul 11 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.7.11-1.fmi
- Update makefile.inc

* Fri Jul  7 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.7.7-1.fmi
- Prefer gdal-3.5, geos-3.11, proj-9.0, postgresql-15 when found

* Fri Jun 16 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.6.16-2.fmi
- smartbuild: improve output redirection to files when --separate-logs is provided

* Fri Jun 16 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.6.16-1.fmi
- smartbuild: update

* Wed Jun 14 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.6.14-1.fmi
- Update supported compiler feature detection in makefile.inc

* Fri Apr 28 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.4.28-2.fmi
- Fix typo in smartbuild script

* Fri Apr 28 2023 Andris Pavenis <andris.pavenis@fmi.fi> 23.4.28-1.fmi
- Update smartbuild script

* Tue Apr 18 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.4.18-2.fmi
- makefile.inc cleanup and new script smartbuild

* Mon Apr 17 2023 Mika Heiskanen <mika.heiskanen@fmi.fi> - 23.4.17-1.fmi
- Support libpq

* Thu Jan 19 2023 Mika Heiskanen <mika.heiskanen@fmi.fi> - 23.1.19-2.fmi
- Also link to sharpyuv when linking to webp

* Thu Jan 19 2023 Mika Heiskanen <mika.heiskanen@fmi.fi> - 23.1.19-1.fmi
- Support libwebp13

* Wed Dec 14 2022 Andris Pavēnis <andris.pavenis@fmi.fi> 22.12.14-2.fmi
- New script smartmkciconfig (iteration 2)

* Wed Dec 14 2022 Andris Pavēnis <andris.pavenis@fmi.fi> 22.12.14-1.fmi
- Add PRL script smartmkcoconfig

* Fri Oct  7 2022 Andris Pavēnis <andris.pavenis@fmi.fi> 22.10.7-1.fmi
- Add smartpngdiff for use for image comparition in tests

* Tue Feb  8 2022 Andris Pavenis <andris.pavenis@fmi.fi> 22.2.8-3.fmi
- Fixed typo

* Tue Feb  8 2022 Andris Pavēnis <andris.pavenis@fmi.fi> 22.2.8-2.fmi
- Fix sqlite3 and proj detection in makefile.inc

* Tue Feb  8 2022 Andris Pavēnis <andris.pavenis@fmi.fi> 22.2.8-1.fmi
- Reafactor makefile.inc and add support for libspatialite, sqlite3 and proj

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
