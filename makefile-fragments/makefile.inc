##############################################################################
#
#  This file is part of smartmet-library-macgyver
#
#  It is installed as $PREFIX/share/smartmet/devel/makefile.inc
#
#  For use in other projects put into the Makefile
#
#  include $$(PREFIX-/usr)/share/smartmet/devel/makefile.inc
#
###############################################################################

-include $(HOME)/.smartmet.mk

# Installation

INSTALL_PROG = install -p -m 775
INSTALL_DATA = install -p -m 664

ifeq ($(origin PREFIX), undefined)
  PREFIX = /usr
else
  PREFIX = $(PREFIX)
endif

processor := $(shell uname -p)
ifeq ($(processor), x86_64)
  libdir ?= $(PREFIX)/lib64
else
  libdir ?= $(PREFIX)/lib
endif

bindir ?= $(PREFIX)/bin
includedir ?= $(PREFIX)/include
datadir ?= $(PREFIX)/share
sbindir ?= $(PREFIX)/sbin
objdir = obj

enginedir ?= $(datadir)/smartmet/engines
plugindir ?= $(datadir)/smartmet/plugins

# pkg-config extra directories for use with PGDG RPM packages

SQLITE3_PKG_SEARCH := $(subst $(eval) ,:,$(foreach ver, 330, /usr/sqlite$(ver)/lib/pkgconfig))
SPATIALITE_PKG_SEARCH := $(subst $(eval) ,:,$(foreach ver, 43 50, /usr/libspatialite$(ver)/lib/pkgconfig))
PROJ_PKG_SEARCH := $(subst $(eval) ,:,$(foreach ver, 82 81, /usr/proj$(ver)/lib/pkgconfig))

# Compiler flags

OPTIMIZE ?= -O2
GCC_DIAG_COLOR ?= always

CXX_VERSION_STRING := $(shell $(CXX) --version 2>/dev/null | head -n 1)

ifneq ($(CXX_VERSION_STRING),)

CXX_ID_ := $(shell echo "$(CXX_VERSION_STRING)" | sed -e 's:[\ -].*::')

ifneq ($(filter $(CXX_ID_), clang),)
  USE_CLANG=yes
  CXX_STD ?= c++17
else
  GCC_VERSION := $(shell $(CXX) -dumpversion 2>/dev/null)
  GCC_MAJOR := $(shell echo $(GCC_VERSION) | sed -e 's:\..*::')
  USE_CLANG=no
  ifeq ($(GCC_MAJOR),)
    $(error Failed to query compiler version: $(CXX) -dumpversion)
  endif
  ifeq ($(filter $(GCC_MAJOR), 6 7 8 9 10 11 12),)
    CXX_STD ?= c++11
  else
    CXX_STD ?= c++17
  endif
endif

FLAGS += -std=$(CXX_STD) -fdiagnostics-color=$(GCC_DIAG_COLOR) \
	-ggdb3 -fPIC -fno-omit-frame-pointer \
	-Wall -Wextra \
	-Wno-unused-parameter \
        -Wno-unknown-warning-option

FLAGS_DEBUG = -Og -Wpedantic -Wundef
FLAGS_RELEASE = $(OPTIMIZE) -Wuninitialized -DNDEBUG
FLAGS_PROFILE = $(OPTIMIZE) -g -pg -DNDEBUG
PACKAGES_MISSING =

ifeq ($(USE_CLANG), yes)
  FLAGS_DEBUG += -Wshadow -Wweak-vtables -Wzero-as-null-pointer-constant
  # clang does not by default provide parameter --build-id to linker. Add it directly
  LDFLAGS += -Wl,--build-id=sha1
endif

# Sanitizer support

ifeq ($(TSAN), yes)
  FLAGS += -fsanitize=thread
  LDFLAGS += -fsanitize=thread
endif
ifeq ($(ASAN), yes)
  FLAGS += -fsanitize=address -fsanitize=pointer-compare -fsanitize=pointer-subtract \
           -fsanitize=undefined -fsanitize-address-use-after-scope
  LDFLAGS += -fsanitize=address -fsanitize=undefined
endif

# Static code analyzer support (warning: currently only gcc-10+)

ifeq ($(ANALYZER), yes)
  ifeq ($(USE_CLANG), yes)
     $(error "TODO: Implement static code analyzer support for CLANG")
  else
     FLAGS += -fanalyzer
  endif
endif


FLAGS += $(EXTRA_FLAGS)

# Compile modes (debug / release)

ifneq (,$(findstring debug,$(MAKECMDGOALS)))
  CFLAGS = $(DEFINES) $(FLAGS) $(FLAGS_DEBUG)
else
  ifneq (,$(findstring profile,$(MAKECMDGOALS)))
    CFLAGS = $(DEFINED) $(FILES) $(FLAGS_PROFILE)
  else
    CFLAGS = $(DEFINES) $(FLAGS) $(FLAGS_RELEASE)
  endif
endif

# Include paths and libs

ifneq ($(PREFIX),/usr)
  INCLUDES += -isystem $(includedir)
endif

INCLUDES += -I$(includedir)/smartmet

REQUIRES ?=

REQUIRES_MISSING :=
REQUIRES_LEFT := $(REQUIRES)
REQUIRED_LIBS :=

ifneq "$(wildcard /usr/include/boost169)" ""
  INCLUDES += -isystem /usr/include/boost169
  LIBS += -L/usr/lib64/boost169
endif

ifneq ($(filter spatialite,$(REQUIRES)),)
  ifeq ($(filter sqlite3,$(REQUIRES)),)
    REQUIRES := $(REQUIRES) sqlite3
  endif
  ifeq ($(filter proj,$(REQUIRES)),)
    REQUIRES := $(REQUIRES) proj
  endif
endif

ifneq ($(filter cairo,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out cairo, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags cairo 2>/dev/null)
  CAIRO_LIBS := $(shell pkg-config --libs cairo)
  ifeq ($(CAIRO_LIBS),)
    REQUIRES_MISSING += libcairo
  endif
  REQUIRED_LIBS += $(CAIRO_LIBS)
endif

ifneq ($(filter configpp,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out configpp, $(REQUIRES_LEFT))
  ifeq ($(wildcard /usr/libconfig17/lib/libconfig.so),)
    INCLUDES += $(shell pkg-config --cflags libconfig++ 2>/dev/null)
    CONFIGPP_LIBS = $(shell pkg-config --libs libconfig++ 2>/dev/null)
  else
    INCLUDES += -I /usr/libconfig17/include
    CONFIGPP_LIBS = -L /usr/libconfig17/lib -lconfig++ -lconfig
  endif
  ifeq ($(CONFIGPP_LIBS),)
    REQUIRES_MISSING += libconfig++
  endif
  REQUIRED_LIBS += $(CONFIGPP_LIBS)
endif

ifneq ($(filter ctpp2,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out ctpp2, $(REQUIRES_LEFT))
  CTPP2_LIBS := -lctpp2
  REQUIRED_LIBS += $(CTPP2_LIBS)
endif

# This entry is for debugging makefile.inc only
ifneq ($(filter debug_dummy,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out debug_dummy, $(REQUIRES_LEFT))
  REQUIRES_MISSING += debug_dummy
endif

ifneq ($(filter fmt,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out fmt, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags fmt 2>/dev/null)
  FMT_LIBS := $(shell pkg-config --libs fmt 2>/dev/null)
  ifeq ($(FMT_LIBS),)
    REQUIRES_MISSING += fmt
  endif
  REQUIRED_LIBS += $(FMT_LIBS)
endif

ifneq ($(filter gdal,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out gdal, $(REQUIRES_LEFT))
  ifneq "$(wildcard /usr/gdal34/include)" ""
    INCLUDES += -isystem /usr/gdal34/include
    GDAL_LIBS = -L$(PREFIX)/gdal34/lib -lgdal
  else
    ifneq "$(wildcard /usr/gdal33/include)" ""
      INCLUDES += -isystem /usr/gdal33/include
      GDAL_LIBS = -L$(PREFIX)/gdal33/lib -lgdal
    else
      INCLUDES += -isystem /usr/include/gdal
      GDAL_LIBS = -lgdal
    endif
  endif
  REQUIRED_LIBS += $(GDAL_LIBS)
endif

ifneq ($(filter geos,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out geos, $(REQUIRES_LEFT))
  ifneq "$(wildcard /usr/geos310/bin/geos-config)" ""
    # Avoid unresolved references with geos-3.10 (src/inline.cpp dropped from sources)
    FLAGS += -DGEOS_INLINE
    INCLUDES += $(shell /usr/geos310/bin/geos-config --cflags 2>/dev/null)
#   /usr/geos310/bin/geos-config --libs is broken
    GEOS_LIBS = -L /usr/geos310/lib64 -lgeos
  else
    ifneq "$(wildcard /usr/geos39/bin/geos-config)" ""
      INCLUDES += $(shell /usr/geos39/bin/geos-config --cflags 2>/dev/null)
      GEOS_LIBS = $(shell /usr/geos39/bin/geos-config --libs 2>/dev/null)
    else
      ifneq "$(wildcard /usr/bin/geos-config)" ""
        GEOS_LIBS = $(shell /usr/bin/geos-config --libs 2>/dev/null)
      endif
    endif
  endif
  REQUIRED_LIBS += $(GEOS_LIBS)
endif

ifneq ($(filter icu-i18n,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out icu-i18n, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags icu-i18n 2>/dev/null)
  ICU_I18N_LIBS = $(shell pkg-config --libs icu-i18n 2>/dev/null)
  ifeq ($(ICU_I18N_LIBS),)
     REQUIRES_MISSING += icu-i18n
  endif
  REQUIRED_LIBS += $(ICU-I18N_LIBS)
endif

ifneq ($(filter jsoncpp,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out jsoncpp, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags jsoncpp 2>/dev/null)
  JSONCPP_LIBS = $(shell pkg-config --libs jsoncpp 2>/dev/null)
  ifeq ($(JSONCPP_LIBS),)
    REQUIRES_MISSING += jsoncpp
  endif
  REQUIRED_LIBS += $(JSONCPP_LIBS)
endif

ifneq ($(filter libpqxx,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out libpqxx, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags libpqxx 2>/dev/null)
  LIBPQXX_LIBS = $(shell pkg-config --libs libpqxx 2>/dev/null)
  ifeq ($(LIBPQXX_LIBS),)
    REQUIRES_MISSING += libpqxx
  endif
  REQUIRED_LIBS += $(LIBPQXX_LIBS)
endif

ifneq ($(filter librsvg,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out librsvg, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags librsvg-2.0 2>/dev/null)
  LIBRSVG_LIBS = $(shell pkg-config --libs librsvg-2.0)
  ifeq ($(LIBRSVG_LIBS),)
    REQUIRES_MISSING += librsvg-2.0
  endif
  REQUIRED_LIBS += $(LIBRSVG_LIBS)
endif

ifneq ($(filter mysql,$(REQUIRES)),)
  $(warning Use mariadb instead of mysql in REQUIRES list)
  REQUIRES_LEFT := $(filter-out mysql, $(REQUIRES_LEFT))
  ifeq ($(filter mariadb,$(REQUIRES)),)
     REQUIRES += mariadb
  endif
endif

ifneq ($(filter mariadb,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out mariadb, $(REQUIRES_LEFT))
  MARIADB_LIBS = $(shell mysql_config --libs_r 2>/dev/null)
  INCLUDES += $(shell mysql_config --cflags 2>/dev/null)
  ifeq ($(MARIADB_LIBS),)
    REQUIRES_MISSING += mariadb
  endif
  REQUIRED_LIBS += $(MARIADB_LIBS)
  MYSQL_LIBS := $(MARIADB_LIBS)
endif

ifneq ($(filter spatialite,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out spatialite, $(REQUIRES_LEFT))
  # Search in /usr/spatiallite(?:50|43) at first
  SPATIALITE_LIBS := $(shell PKG_CONFIG_PATH="$(SPATIALITE_PKG_SEARCH)" pkg-config --libs spatialite)
  INCLUDES += $(shell PKG_CONFIG_PATH="$(SPATIALITE_PKG_SEARCH)" pkg-config --cflags spatialite)
  ifeq ($(SPATIALITE_LIBS),)
    REQUIRES_MISSING += spatialite
  endif
  REQUIRED_LIBS += $(SPATIALITE_LIBS)
endif

ifneq ($(filter sqlite3,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out sqlite3, $(REQUIRES_LEFT))
  # Search in /usr/sqlite330 at first
  SQLITE3_LIBS := $(shell PKG_CONFIG_PATH="$(SQLITE3_PKG_SEARCH)" pkg-config --libs sqlite3)
  INCLUDES += $(shell PKG_CONFIG_PATH="$(SQLITE3_PKG_SEARCH)" pkg-config --cflags sqlite3)
  ifeq ($(SQLITE3_LIBS),)
    REQUIRES_MISSING += sqlite3
  endif
  REQUIRED_LIBS += $(SQLITE3_LIBS)
endif

ifneq ($(filter xerces-c,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out xerces-c, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags xerces-c --cflags 2>/dev/null)
  XERCES_C_LIBS = $(shell pkg-config --libs xerces-c --libs 2>/dev/null)
  ifeq ($(XERCES_C_LIBS),)
    REQUIRES_MISSING += xerces-c
  endif
  REQUIRED_LIBS += $(XERCES_C_LIBS)
endif

ifneq ($(filter proj,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out proj, $(REQUIRES_LEFT))
  INCLUDES += $(shell PKG_CONFIG_PATH="$(PROJ_PKG_SEARCH)" pkg-config --cflags proj 2>/dev/null)
  PROJ_LIBS := $(shell PKG_CONFIG_PATH="$(PROJ_PKG_SEARCH)" pkg-config --libs proj 2>/dev/null)
  ifeq ($(PROJ_LIBS),)
    ifneq ($(wildcard /usr/proj72/lib/libproj*),)
      INCLUDES += -I/usr/proj72/include
      PROJ_LIBS += -L/usr/proj72/lib
    endif
  endif
  ifeq ($(PROJ_LIBS),)
    REQUIRES_MISSING += proj
  endif
  REQUIRED_LIBS += $(PROJ_LIBS)
endif

# For testing only
ifneq ($(filter missing-require-test,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out missing-require-test, $(REQUIRES_LEFT))
  REQUIRES_MISSING += missing-require-test
endif

ifneq ($(REQUIRES_MISSING),)
  ifeq ($(wildcard smartmet-*.spec),)
    ifneq ($(filter all release debug profile rpm,$(MAKECMDGOALS)),)
      $(error Development files missing for: $(REQUIRES_MISSING))
    endif
  else
    ifneq ($(filter all release debug profile rpm,$(MAKECMDGOALS)),)
       $(error Development files missing for: $(REQUIRES_MISSING))
    endif
    ifeq ($(MAKECMDGOALS),)
       $(error Development files missing for: $(REQUIRES_MISSING))
    endif
  endif
endif

ifneq ($(REQUIRES_LEFT),)
$(warning Unrecognized entries in REQUIRES variable: $(REQUIRES_LEFT))
endif

INCLUDES := $(shell echo $(INCLUDES) | sed -r 's/-I[[:space:]]*/-isystem /g')

endif
# end of 'ifneq ($(CXX_VERSION_STRING),)' block

CLEAN_TARGETS :=

#############################
#     rpmbuild options      #
#############################

RPMBUILD_OPT ?=
WITH_TESTS ?=

ifneq ($(WITH_TESTS),)
  RPMBUILD_OPT += --define "with-tests 1"
endif
