GCC_VER := gcc (GCC) 8.4.1 20200928 (Red Hat 8.4.1-1)
GCC_VER_OLD := gcc (GCC) 4.8.5 20150623 (Red Hat 4.8.5-44)
CLANG_VER := clang version 11.0.0 (Red Hat 11.0.0-1.module+el8.4.0+8598+a071fcd5)

export __SMARTCXXCHECK__ := ../smartcxxcheck

test:	test-pngcomp test-check-proj test-check-gdal test-check-geos test-check-sqlite

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
