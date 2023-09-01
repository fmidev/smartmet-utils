REQUIRES := sqlite

include ../makefile-fragments/makefile.inc

all:
	@/bin/echo SQLITE3_PKG_SEARCH=$(SQLITE_PKG_SEARCH)
	@/bin/echo INCLUDES=$(INCLUDES)
	@/bin/echo PROJ_LIBS=$(PROJ_LIBS)
	@/bin/echo REQUIRED_LIBS=$(REQUIRED_LIBS)
	@/bin/echo CFLAGS=$(CFLAGS)
	$(CXX) $(CFLAGS) $(INCLUDES) check-sqlite.cpp $(REQUIRED_LIBS) -o /dev/null
