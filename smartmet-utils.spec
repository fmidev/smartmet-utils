%define BINNAME utils
%define SPECNAME smartmet-%{BINNAME}
Summary: utils
Name: %{SPECNAME}
Version: 26.5.2
Release: 2%{?dist}.fmi
License: FMI
Group: Development/Tools
URL: http://www.weatherproof.fi
Source0: %{name}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot-%(%{__id_u} -n)
BuildRequires: make
BuildRequires: bash
BuildRequires: gcc-c++
BuildRequires: ImageMagick-c++-devel
Requires: make bash perl git

%description
FMI SmartSet server related utils

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q -n %{BINNAME}

%build
make %{?_smp_mflags}

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%package devel
Summary: FMI SmartSet server development related utils and files
Provides: smartabicheck
Provides: smartcheckinstalled
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
#TestRequires: geos313-devel
#TestRequires: gdal312-devel
#TestRequires: libcurl-devel
#TestRequires: libtiff-devel >= 4.1

%if 0%{?rhel} && 0%{rhel} >= 8
#TestRequires: proj97-devel
#TestRequires: sqlite-devel
%endif

%description devel
FMI SmartSet server development related utils and files

%defattr(0775,root,root,0775)

%files devel
%defattr(0775,root,root,0775)
%{_bindir}/smartabicheck
%{_bindir}/smartcheckinstalled
%{_bindir}/smartbuild
%{_bindir}/smartbuildcfg
%{_bindir}/smartbuildrev
%{_bindir}/smartbuildtmprpm
%{_bindir}/smartcxxcheck
%{_bindir}/smartmkciconfig
%{_bindir}/smartmkrelease
%{_bindir}/smartmktag
%{_bindir}/smartpngdiff
%{_bindir}/smartrpmsort
%{_bindir}/smartimgdiff_psnr
%{_datadir}/smartmet/devel/makefile.inc
%{_datadir}/smartmet/devel/makefile-abicheck.inc
%{_mandir}/man1/*.1.gz

%changelog
* Sat May  2 2026 Mika Heiskanen <mika.heiskanen@fmi.fi> 26.5.2-2.fmi
- smartbuildcfg: fix swapped if/else branches that caused the CXX
  environment variable to be ignored. The script now uses $CXX when
  set and falls back to g++ otherwise, as originally intended.

* Sat May  2 2026 Mika Heiskanen <mika.heiskanen@fmi.fi> 26.5.2-1.fmi
- --help

* Tue Apr 28 2026 Mika Heiskanen <mika.heiskanen@fmi.fi> 26.4.28-3.fmi
- smartcheckinstalled: classify each unresolved symbol against an
  index of installed libsmartmet-*.so and engine .so files, and
  recommend the right action (add -l<lib>, rebuild against current
  headers, or report missing provider). Engine-supplied symbols
  (late-bound at runtime) are now suppressed correctly. Skip
  /usr/share/smartmet/python/ since those C-extensions resolve
  Python C-API symbols from the interpreter at runtime, not from
  smartmet libraries.

* Tue Apr 28 2026 Mika Heiskanen <mika.heiskanen@fmi.fi> 26.4.28-2.fmi
- New script smartcheckinstalled: scans every installed SmartMet ELF
  for unresolved symbols. Intended as a pre-deployment gate to catch
  inconsistent installed-package sets before they reach production.
- Both scripts: broaden debuginfo-symlink filter to */.build-id/*
  (was only catching /usr/lib/.build-id/*, missing the parallel
  /usr/lib/debug/.build-id/* tree).

* Tue Apr 28 2026 Mika Heiskanen <mika.heiskanen@fmi.fi> 26.4.28-1.fmi
- smartabicheck: ignore unrelated pre-existing breakage by diffing
  ldd -r before/after substituting in the new library, and only warn
  on vtable/data symbol removals (additions are backward-compatible)

* Mon Apr 27 2026 Mika Heiskanen <mika.heiskanen@fmi.fi> 26.4.27-1.fmi
- Add smartabicheck and 'make abicheck' / 'make abicheck-deep' targets
  for per-consumer ABI sanity checking against installed RPMs

* Wed Feb  4 2026 Andris Pavēnis <andris.pavenis@fmi.fi> 26.2.4-1.fmi
- Update to proj-9.7, gdal-3.12, fmt-12

* Thu Nov 27 2025 Andris Pavēnis <andris.pavenis@fmi.fi> 25.11.27-1.fmi
- smartbuild: exclude smartmet-topography-data from dependency check

* Fri Oct 10 2025 Andris Pavēnis <andris.pavenis@fmi.fi> 25.10.10-1.fmi
- Use 'uname -m' for detecting processor type

* Tue Aug 19 2025 Andris Pavēnis <andris.pavenis@fmi.fi> 25.8.19-1.fmi
- Use always -march=x86-64 for x86_64 CPUs (RHEL10 and RockyLinux 10 default is x86-64-v3)

* Wed Aug  6 2025 Andris Pavēnis <andris.pavenis@fmi.fi> 25.8.6-1.fmi
- smartimgdiff_psnr update

* Fri Aug  1 2025 Andris Pavēnis <andris.pavenis@fmi.fi> 25.8.1-1.fmi
- smartbuild: support build for RHEL10

* Wed Jul 23 2025 Andris Pavēnis <andris.pavenis@fmi.fi> 25.7.23-1.fmi
- Reimplement image comparison (do not use 'magick compare')

* Tue Feb 18 2025 Andris Pavēnis <andris.pavenis@fmi.fi> 25.2.18-1.fmi
- Update to gdal-3.10, geos-3.13 and proj-9.5

* Tue Dec 10 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.12.10-1.fmi
- smartbuild default config: add smartmet-press and remove smartmet-plugin-admin

* Tue Sep 10 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.9.10-1.fmi
- Fix smartbuild (broken in previous version)

* Fri Sep  6 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.9.6-1.fmi
- Update smartbuild (use smartmet-open-beta also for RHEL9) and smartrpmsort

* Wed Aug 21 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.8.21-1.fmi
- smartbuild: add smartmet-library-newbase-python to default ignore list

* Wed Aug  7 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.8.7-3.fmi
- Update to gdal-3.8, geos-3.12, proj-94 and fmt-11

* Wed Aug  7 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.8.7-2.fmi
- makefile.inc: update detection of libspatialite

* Wed Aug  7 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.8.7-1.fmi
- makefile.inc: update detection of libspatialite

* Fri Jul 19 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.7.19-1.fmi
- smartbuild and makefile.inc updates

* Mon Jul  1 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.7.1-1.fmi
- Add require string 'filesystem' for libstdc++fs

* Fri May 31 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.5.30-1.fmi
- smartbuild: GIT branch support with optional fallback if not found
- makefile.inc: Link time optimization support (USE_LTO=yes ==> -flto)

* Fri May 10 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.5.10-2.fmi
- smartbuild: Reverted branch support (does not work and causes problems)

* Fri May 10 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.5.10-1.fmi
- smartbuild: update

* Wed Mar 13 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.3.13-2.fmi
- Fix missing installation of new script

* Wed Mar 13 2024 Andris Pavēnis <andris.pavenis@fmi.fi> 24.3.13-1.fmi
- New script smartbuildtmprpm and 1 bugfix

* Thu Feb 29 2024 Mika Heiskanen <mika.heiskanen@fmi.fi> - 24.2.29-1.fmi
- Added smartmet-cropper to smartbuild

* Wed Sep  6 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.9.6-1.fmi
- makefile.inc and tests update

* Wed Aug 23 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.8.23-1.fmi
- makefile.inc: fix libcurl detection

* Fri Aug 11 2023 Andris Pavenis <andris.pavenis@fmi.fi> 23.8.11-1.fmi
- smartbuild: fix RPM version extraction for RHEL7

* Wed Aug  9 2023 Andris Pavēnis <andris.pavenis@fmi.fi> 23.8.9-3.fmi
- Update smartbuild )bugfixes and new features)

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
