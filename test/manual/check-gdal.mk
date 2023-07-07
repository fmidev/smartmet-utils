REQUIRES := gdal

include ../../makefile-fragments/makefile.inc

all:
	@/bin/echo __GDAL_INCLUDES_SEARCH=$(__GDAL_INCLUDES_SEARCH)
	@/bin/echo __GDAL_DN=$(__GDAL_DN)
	@/bin/echo INCLUDES=$(INCLUDES)
	@/bin/echo GDAL_LIBS=$(GDAL_LIBS)
	@/bin/echo REQUIRED_LIBS=$(REQUIRED_LIBS)
