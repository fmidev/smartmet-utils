GCC_VER := gcc (GCC) 8.4.1 20200928 (Red Hat 8.4.1-1)
GCC_VER_OLD := gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)
CLANG_VER := clang version 11.0.0 (Red Hat 11.0.0-1.module+el8.4.0+8598+a071fcd5)

test:
	if $(MAKE) -f Makefile.test all CXX_VERSION_STRING="$(GCC_VER)" REQUIRES=missing-require-test >/dev/null 2>&1; then false; else true; fi
	if $(MAKE) -f Makefile.test debug CXX_VERSION_STRING="$(GCC_VER)" REQUIRES=missing-require-test >/dev/null 2>&1; then false; else true; fi
	if $(MAKE) -f Makefile.test rpm CXX_VERSION_STRING="$(GCC_VER)" REQUIRES=missing-require-test >/dev/null 2>&1; then false; else true; fi
	if $(MAKE) -f Makefile.test test CXX_VERSION_STRING="$(GCC_VER)" REQUIRES=missing-require-test >/dev/null 2>&1; then true; else false; fi
	if $(MAKE) -f Makefile.test test-installed CXX_VERSION_STRING="$(GCC_VER)" REQUIRES=missing-require-test >/dev/null 2>&1; then true; else false; fi
	if $(MAKE) -f Makefile.test all EMPTY_CXX_VERSION=Y REQUIRES=missing-require-test >/dev/null 2>&1; then true; else false; fi
	if $(MAKE) -f Makefile.test debug EMPTY_CXX_VERSION=Y REQUIRES=missing-require-test >/dev/null 2>&1; then true; else false; fi
	if $(MAKE) -f Makefile.test rpm EMPTY_CXX_VERSION=Y REQUIRES=missing-require-test >/dev/null 2>&1; then true; else false; fi
	if $(MAKE) -f Makefile.test test EMPTY_CXX_VERSION=Y REQUIRES=missing-require-test >/dev/null 2>&1; then true; else false; fi
	if $(MAKE) -f Makefile.test test-installed EMPTY_CXX_VERSION=Y REQUIRES=missing-require-test >/dev/null 2>&1; then true; else false; fi
#
	$(MAKE) -f Makefile.test check-std CXX_VERSION_STRING="$(GCC_VER_OLD)" GCC_VERSION="4.8.5" EXP_CXX_STD=c++11 REQUIRES=
	$(MAKE) -f Makefile.test check-std CXX_VERSION_STRING="$(GCC_VER)"  GCC_VERSION="8" EXP_CXX_STD=c++17 REQUIRES=
	$(MAKE) -f Makefile.test check-std CXX_VERSION_STRING="$(CLANG_VER)" EXP_CXX_STD=c++17 REQUIRES=
