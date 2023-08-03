REQUIRES := proj

include ../makefile-fragments/makefile.inc

all:
	@/bin/echo PROJ_PKG_SEARCH=$(PROJ_PKG_SEARCH)
	@/bin/echo INCLUDES=$(INCLUDES)
	@/bin/echo PROJ_LIBS=$(PROJ_LIBS)
	@/bin/echo REQUIRED_LIBS=$(REQUIRED_LIBS)
	@/bin/echo CFLAGS=$(CFLAGS)
	( echo "#include <proj.h>"; echo "int main() { return 0; }" ) |\
		$(CXX) -x c++ $(CFLAGS) $(INCLUDES) - $(REQUIRED_LIBS) -o /dev/null
