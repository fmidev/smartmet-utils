MODULE = utils

__SMARTCXXCHECK__ := ./smartcxxcheck

include makefile-fragments/makefile.inc

PROG = \
	smartbuild \
	smartbuildrev \
	smartbuildtmprpm \
	smartbuildcfg \
	smartcxxcheck \
	smartmkciconfig \
	smartmkrelease \
	smartmktag \
	smartpngdiff \
	smartrpmsort

CPP_PROG = \
	smartimgdiff_psnr

.PHONY: rpm

all: $(CPP_PROG)

smartimgdiff_psnr: smartimgdiff_psnr.cpp
	$(CXX) -std=c++17 -O2 -g3 -Wall -Wextra -o $@ $< $(shell pkg-config --cflags --libs Magick++)

clean:
	rm -f *~
	rm -f $(CPP_PROG)
	rm -f smartmet-$(MODULE).tar.gz

install:
	mkdir -p $(bindir)
	@list='$(PROG) $(CPP_PROG)'; \
	for prog in $$list; do \
	 echo $(INSTALL_PROG) $$prog $(bindir)/$$prog; \
	 $(INSTALL_PROG) $$prog $(bindir)/$$prog; \
	done
	@mkdir -p $(datadir)/smartmet/devel
	$(INSTALL_DATA) makefile-fragments/makefile.inc $(datadir)/smartmet/devel/makefile.inc
	$(INSTALL_DATA) makefile-fragments/makefile-abicheck.inc $(datadir)/smartmet/devel/makefile-abicheck.inc

rpm: 	clean
	rm -f smartmet-$(MODULE).tar.gz
	tar -czvf smartmet-$(MODULE).tar.gz --exclude-vcs --transform "s,^,$(MODULE)/," *
	rpmbuild -tb smartmet-$(MODULE).tar.gz
	rm -f smartmet-$(MODULE).tar.gz

.PHONY: test

test:
	$(MAKE) -C test $@
