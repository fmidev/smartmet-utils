GCC_VER := gcc (GCC) 8.4.1 20200928 (Red Hat 8.4.1-1)
GCC_VER_OLD := gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)
CLANG_VER := clang version 11.0.0 (Red Hat 11.0.0-1.module+el8.4.0+8598+a071fcd5)

export __SMARTCXXCHECK__ := ../smartcxxcheck

test:	test-detection test-pngcomp test-check-proj test-check-gdal test-check-geos test-check-sqlite

test-detection:
	@echo =====================================================================
	@echo = Tests below contains also error condition tests                   =
	@echo = One should get error messages from these tests,                   =
	@echo = but only final result of test run matters                         =
	@echo =====================================================================
	$(MAKE) test-pngcomp
	if $(MAKE) -f Makefile.test all CXX=./dummy841.sh REQUIRES=missing-require-test ; then false; else true; fi
	if $(MAKE) -f Makefile.test debug CXX=./dummy841.sh REQUIRES=missing-require-test ; then false; else true; fi
	if $(MAKE) -f Makefile.test rpm CXX=./dummy841.sh REQUIRES=missing-require-test ; then false; else true; fi
	if $(MAKE) -f Makefile.test test CXX=./dummy841.sh REQUIRES=missing-require-test ; then true; else false; fi
	if $(MAKE) -f Makefile.test test-installed CXX=./dummy841.sh GCC_VERSION=8 REQUIRES=missing-require-test ; then true; else false; fi
	if $(MAKE) -f Makefile.test all CXX=/usr/bin/non-existing REQUIRES=missing-require-test ; then true; else false; fi
	if $(MAKE) -f Makefile.test debug CXX=/usr/bin/non-existing REQUIRES=missing-require-test ; then true; else false; fi
	if $(MAKE) -f Makefile.test rpm CXX=/usr/bin/non-existing REQUIRES=missing-require-test ; then true; else false; fi
	if $(MAKE) -f Makefile.test test CXX=/usr/bin/non-existing REQUIRES=missing-require-test ; then true; else false; fi
	if $(MAKE) -f Makefile.test test-installed CXX=/usr/bin/non-existing REQUIRES=missing-require-test ; then true; else false; fi
#  Does not work if CXX_STD is specified in environmen variable or ~/.smartmet.mk
#	$(MAKE) -f Makefile.test check-std CXX=./dummy485.sh EXP_CXX_STD=c++11 REQUIRES=
#	$(MAKE) -f Makefile.test check-std CXX=./dummy841.sh GCC_VERSION="8" EXP_CXX_STD=c++17 REQUIRES=

test-pngcomp:
	$(MAKE) -C pngdiff test

test-check-proj:
	$(MAKE) -f check-proj.mk

test-check-gdal:
	$(MAKE) -f check-gdal.mk

test-check-geos:
	$(MAKE) -f check-geos.mk

test-check-sqlite:
	$(MAKE) -f check-sqlite.mk
