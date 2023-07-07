REQUIRES := proj

include ../../makefile-fragments/makefile.inc

all:
	@/bin/echo PROJ_PKG_SEARCH=$(PROJ_PKG_SEARCH)
	@/bin/echo INCLUDES=$(INCLUDES)
	@/bin/echo PROJ_LIBS=$(PROJ_LIBS)
	@/bin/echo REQUIRED_LIBS=$(REQUIRED_LIBS)
