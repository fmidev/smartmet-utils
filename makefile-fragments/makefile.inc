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
PROJ_PKG_SEARCH := $(subst $(eval) ,:,$(foreach ver, 90 82 81, /usr/proj$(ver)/lib/pkgconfig /usr/proj$(ver)/lib64/pkgconfig))
__GDAL_INCLUDES_SEARCH := $(foreach ver, 35 34, $(wildcard /usr/gdal$(ver)/include/gdal.h))
__GEOS_INCLUDES_SEARCH := $(foreach ver, 311 310, $(wildcard /usr/geos$(ver)/include/geos/version.h))

# /usr/lib64 ja samanlaiset ei saa antaa -L parametrin arvoina. Ne tulevat kuitenkin usein pkg-config vastauksissa
__REMOVE_LINK_DIRS := -L/lib -L/lib64 -L/usr/lib -L/usr/lib64 -L/usr/local/lib -L/usr/local/lib64

# Empty if $(libdir) is system library directory or -L$(libdir) otherwise
libdir_opt := $(filter-out $(__REMOVE_LINK_DIRS), -L$(libdir))

# Compiler flags

__SMARTCXXCHECK__ ?= smartcxxcheck

OPTIMIZE ?= -O2
GCC_DIAG_COLOR ?= always

__CXX_INFO__ := $(shell $(__SMARTCXXCHECK__) $(CXX))

ifneq ($(word 1, $(__CXX_INFO__)),unknown)

ifeq ($(word 1, $(__CXX_INFO__)),clang++)
  USE_CLANG := yes;
  CXX_STD ?= $(word 3, $(__CXX_INFO__))
  FLAGS += --gcc-toolchain=/usr
else
  ifeq ($(word 1, $(__CXX_INFO__)),g++)
    USE_CLANG := no;
    CXX_STD ?= $(word 3, $(__CXX_INFO__))
  else
    $(error Failed to recognize compiler version (CXX=$(CXX)))
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
  LDFLAGS += --gcc-toolchain=/usr -Wl,--build-id=sha1
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

# LTO support
ifeq ($(USE_LTO), yes)
  FLAGS += -flto
  LDFLAGS += -flto
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
  ifeq ($(filter geos,$(REQUIRES)),)
    REQUIRES += geos
  endif
  ifeq ($(filter proj,$(REQUIRES)),)
    REQUIRES := $(REQUIRES) proj
  endif
endif

ifneq ($(filter proj,$(REQUIRES)),)
  ifeq ($(filter libcurl,$(REQUIRES)),)
    REQUIRES := $(REQUIRES) libcurl
  endif
endif

ifneq ($(filter filesystem,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out filesystem,$(REQUIRES))
  ifneq ($($(CXX) -print-file-name=libstdc++fs.a),libstdc++fs.a)
    REQUIRED_LIBS += -lstdc++fs
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
    INCLUDES += $(filter-out $(__REMOVE_LINK_DIRS), $(shell pkg-config --cflags libconfig++ 2>/dev/null))
    CONFIGPP_LIBS = $(filter-out $(__REMOVE_LINK_DIRS), $(shell pkg-config --libs libconfig++ 2>/dev/null))
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
  FMT_LIBS := $(filter-out $(__REMOVE_LINK_DIRS), $(shell pkg-config --libs fmt 2>/dev/null))
  ifeq ($(FMT_LIBS),)
    REQUIRES_MISSING += fmt
  endif
  REQUIRED_LIBS += $(FMT_LIBS)
endif

ifneq ($(filter gdal,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out gdal, $(REQUIRES_LEFT))
  ifneq "$(words $(__GDAL_INCLUDES_SEARCH))" "0"
    __GDAL_DN := $(word 2, $(subst /, ,$(firstword $(__GDAL_INCLUDES_SEARCH))))
    INCLUDES += -isystem /usr/$(__GDAL_DN)/include
    GDAL_LIBS := -L$(PREFIX)/$(__GDAL_DN)/lib -lgdal
    GDAL_DIR := /usr/$(__GDAL_DN)
  else
    ifneq "$(wildcard /usr/bin/gdal-config)" ""
      # gdal-config is intentionally not used here
      INCLUDES += -isystem /usr/include/gdal
      GDAL_LIBS := -lgdal
      GDAL_DIR := /usr
    else
      REQUIRES_MISSING += gdal
    endif
  endif
  ifneq "$(GDAL_DIR)" ""
    ifneq "$(shell $(GDAL_DIR)/bin/gdal-config --ogr-enabled)" "yes"
      $(error GDAL detected at $(GDAL_DIR), but is built without OGR support)
    endif
  endif
  REQUIRED_LIBS += $(GDAL_LIBS)
endif

ifneq ($(filter geos,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out geos, $(REQUIRES_LEFT))
  ifneq "$(words $(__GEOS_INCLUDES_SEARCH))" "0"
    __GEOS_DN :=  $(word 2, $(subst /, ,$(firstword $(__GEOS_INCLUDES_SEARCH))))
    GEOS_DIR := /usr/$(__GEOS_DN)
    # Avoid unresolved references with geos-3.10 (src/inline.cpp dropped from sources)
    FLAGS += -DGEOS_INLINE
    INCLUDES += $(shell $(GEOS_DIR)/bin/geos-config --cflags 2>/dev/null)
    # Workaround possible conflict between 2 different versions of libgeos_c by
    # providing correct directory in RPATH. That may happens if spatialite is also selected
    # and more than one GEOS version is installed
    GEOS_LIBS = -Wl,--rpath,$(GEOS_DIR)/lib64 -L$(GEOS_DIR)/lib64 -lgeos
  else
    ifneq "$(wildcard /usr/bin/geos-config)" ""
      # geos-config is intentionally not used here
      GEOS_LIBS = -lgeos
    else
      REQUIRES_MISSING += geos
    endif
  endif
  REQUIRED_LIBS += $(GEOS_LIBS)
endif

ifneq ($(filter icu-i18n,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out icu-i18n, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags icu-i18n 2>/dev/null)
  ICU_I18N_LIBS = $(filter-out $(__REMOVE_LINK_DIRS), $(shell pkg-config --libs icu-i18n 2>/dev/null))
  ifeq ($(ICU_I18N_LIBS),)
     REQUIRES_MISSING += icu-i18n
  endif
  REQUIRED_LIBS += $(ICU-I18N_LIBS)
endif

ifneq ($(filter jsoncpp,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out jsoncpp, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags jsoncpp 2>/dev/null)
  JSONCPP_LIBS = $(filter-out $(__REMOVE_LINK_DIRS), $(shell pkg-config --libs jsoncpp 2>/dev/null))
  ifeq ($(JSONCPP_LIBS),)
    REQUIRES_MISSING += jsoncpp
  endif
  REQUIRED_LIBS += $(JSONCPP_LIBS)
endif

ifneq ($(filter libpqxx,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out libpqxx, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags libpqxx 2>/dev/null)
  LIBPQXX_LIBS = $(filter-out $(__REMOVE_LINK_DIRS), $(shell pkg-config --libs libpqxx 2>/dev/null))
  ifeq ($(LIBPQXX_LIBS),)
    REQUIRES_MISSING += libpqxx
  endif
  REQUIRED_LIBS += $(LIBPQXX_LIBS)
endif

ifneq ($(filter librsvg,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out librsvg, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags librsvg-2.0 2>/dev/null)
  LIBRSVG_LIBS = $(filter-out $(__REMOVE_LINK_DIRS), $(shell pkg-config --libs librsvg-2.0))
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
  INCLUDES += $(filter-out $(__REMOVE_LINK_DIRS), $(shell mysql_config --cflags 2>/dev/null))
  ifeq ($(MARIADB_LIBS),)
    REQUIRES_MISSING += mariadb
  endif
  REQUIRED_LIBS += $(MARIADB_LIBS)
  MYSQL_LIBS := $(MARIADB_LIBS)
endif

ifneq ($(filter spatialite,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out spatialite, $(REQUIRES_LEFT))
  # Search in /usr/spatiallite(?:50|43) at first
  SPATIALITE_LIBS := $(filter-out $(__REMOVE_LINK_DIRS), $(shell PKG_CONFIG_PATH="$(SPATIALITE_PKG_SEARCH)" pkg-config --libs spatialite))
  INCLUDES += $(shell PKG_CONFIG_PATH="$(SPATIALITE_PKG_SEARCH)" pkg-config --cflags spatialite)
  ifeq ($(SPATIALITE_LIBS),)
    REQUIRES_MISSING += spatialite
  endif
  REQUIRED_LIBS += $(SPATIALITE_LIBS)
endif

ifneq ($(filter sqlite,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out sqlite, $(REQUIRES_LEFT))
  REQUIRES := $(REQUIRES) sqlite3
endif

ifneq ($(filter sqlite3,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out sqlite3, $(REQUIRES_LEFT))
  # Search in /usr/sqlite330 at first
  SQLITE3_LIBS := $(filter-out $(__REMOVE_LINK_DIRS), $(shell PKG_CONFIG_PATH="$(SQLITE3_PKG_SEARCH)" pkg-config --libs sqlite3))
  INCLUDES += $(shell PKG_CONFIG_PATH="$(SQLITE3_PKG_SEARCH)" pkg-config --cflags sqlite3)
  ifeq ($(SQLITE3_LIBS),)
    REQUIRES_MISSING += sqlite3
  endif
  REQUIRED_LIBS += $(SQLITE3_LIBS)
endif

ifneq ($(filter xerces-c,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out xerces-c, $(REQUIRES_LEFT))
  INCLUDES += $(shell pkg-config --cflags xerces-c --cflags 2>/dev/null)
  XERCES_C_LIBS = $(filter-out $(__REMOVE_LINK_DIRS), $(shell pkg-config --libs xerces-c --libs 2>/dev/null))
  ifeq ($(XERCES_C_LIBS),)
    REQUIRES_MISSING += xerces-c
  endif
  REQUIRED_LIBS += $(XERCES_C_LIBS)
endif

ifneq ($(filter proj,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out proj, $(REQUIRES_LEFT))
  INCLUDES += $(shell PKG_CONFIG_PATH="$(PROJ_PKG_SEARCH)" pkg-config --cflags proj 2>/dev/null)
  PROJ_LIBS := $(filter-out $(__REMOVE_LINK_DIRS), $(shell PKG_CONFIG_PATH="$(PROJ_PKG_SEARCH)" pkg-config --libs proj 2>/dev/null))
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

ifneq ($(filter webp,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out webp, $(REQUIRES_LEFT))
  ifneq "$(wildcard /usr/include/webp13)" ""
    INCLUDES += -isystem /usr/include/webp13
    WEBP_LIBS = -Wl,-rpath=$(PREFIX)/lib64/webp13 -L$(PREFIX)/lib64/webp13 -lwebp -lwebpmux -lsharpyuv
  else
    WEBP_LIBS = -lwebp -lwebpmux -lsharpyuv
  endif
  REQUIRED_LIBS += $(WEBP_LIBS)
endif

ifneq ($(filter libpq,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out libpq, $(REQUIRES_LEFT))
  ifneq "$(wildcard /usr/pgsql-15/include)" ""
    INCLUDES += -isystem /usr/pgsql-15/include
    PQ_LIBS = -Wl,-rpath=$(PREFIX)/pgsql-15/lib -L$(PREFIX)/pgsql-15/lib -lpq
  else
  ifneq "$(wildcard /usr/pgsql-13/include)" ""
    INCLUDES += -isystem /usr/pgsql-13/include
    PQ_LIBS = -Wl,-rpath=$(PREFIX)/pgsql-13/lib -L$(PREFIX)/pgsql-13/lib -lpq
  else
    PQ_LIBS = -lpq
  endif
  endif
  REQUIRED_LIBS += $(PQ_LIBS)
endif

ifneq ($(filter libcurl,$(REQUIRES)),)
  REQUIRES_LEFT := $(filter-out libcurl, $(REQUIRES_LEFT))
  ifneq "$(shell pkg-config --atleast-version=7.61.0 libcurl && echo OK)" "OK"
     REQUIRES_MISSING := $(REQUIRES_MISSING) libcurl>=7.61.0
  else
     INCLUDES += $(shell pkg-config --cflags libcurl)
     LIBCURL_LIBS += $(filter-out $(__REMOVE_LINK_DIRS), $(shell pkg-config --libs libcurl))
  endif
  REQUIRED_LIBS += $(LIBCURL_LIBS)
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

REQUIRED_LIBS := $(filter-out $(__REMOVE_LINK_DIRS), $(REQUIRED_LIBS))
PREFIX_LDFLAGS := $(filter-out $(__REMOVE_LINK_DIRS), -L$(libdir))

#############################
#     rpmbuild options      #
#############################

RPMBUILD_OPT ?=
WITH_TESTS ?=

ifneq ($(WITH_TESTS),)
  RPMBUILD_OPT += --define "with-tests 1"
endif
