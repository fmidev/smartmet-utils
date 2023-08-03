REQUIRES := gdal

include ../makefile-fragments/makefile.inc

all:
	@/bin/echo __GDAL_INCLUDES_SEARCH=\"$(__GDAL_INCLUDES_SEARCH)\"
	@/bin/echo GDAL_DIR=$(GDAL_DIR)
	@/bin/echo INCLUDES=$(INCLUDES)
	@/bin/echo GDAL_LIBS=$(GDAL_LIBS)
	@/bin/echo REQUIRED_LIBS=$(REQUIRED_LIBS)
	@/bin/echo CFLAGS=$(CFLAGS)
	( echo '#include <gdal.h>'; echo 'int main() { return 0; }'; ) |\
		$(CXX) -x c++ $(CFLAGS) $(INCLUDES) - $(REQUIRED_LIBS) -o /dev/null
