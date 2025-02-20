REQUIRES := proj

include ../makefile-fragments/makefile.inc

all:
	@/bin/echo PROJ_PKG_SEARCH=$(PROJ_PKG_SEARCH)
	@/bin/echo INCLUDES=$(INCLUDES)
	@/bin/echo PROJ_LIBS=$(PROJ_LIBS)
	@/bin/echo REQUIRED_LIBS=$(REQUIRED_LIBS)
	@/bin/echo CFLAGS=$(CFLAGS)
# Verify that pkg-config actually founds proj (could fail also due to missing dependency,
# makefile.inc redirects pkg-config stderr to /dev/null, so we must check once more here)
	PKG_CONFIG_PATH=$(PROJ_PKG_SEARCH) pkg-config --cflags --libs $(REQUIRES)
	( echo "#include <proj.h>"; echo "int main() { return 0; }" ) |\
		$(CXX) -x c++ $(CFLAGS) $(INCLUDES) - $(REQUIRED_LIBS) -o /dev/null
