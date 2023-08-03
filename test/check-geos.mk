REQUIRES := geos

include ../makefile-fragments/makefile.inc

all:
	@/bin/echo __GEOS_INCLUDES_SEARCH="$(__GEOS_INCLUDES_SEARCH)"
	@/bin/echo DOS_DIR="$(GEOS_DIR)"
	@/bin/echo INCLUDES="$(INCLUDES)"
	@/bin/echo GEOS_LIBS="$(GEOS_LIBS)"
	@/bin/echo REQUIRED_LIBS="$(REQUIRED_LIBS)"
	( echo '#include <geos_c.h>'; echo 'int main() { return 0; }'; ) |\
		$(CXX) -x c++ $(CFLAGS) $(INCLUDES) - $(REQUIRED_LIBS) -o /dev/null
