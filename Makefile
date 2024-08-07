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

.PHONY: rpm

all:

clean:
	rm -f *~
	rm -f smartmet-$(MODULE).tar.gz

install:
	mkdir -p $(bindir)
	@list='$(PROG)'; \
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
	rpmbuild --target noarch -tb smartmet-$(MODULE).tar.gz
	rm -f smartmet-$(MODULE).tar.gz

.PHONY: test

test:
	$(MAKE) -C test $@
