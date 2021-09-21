MODULE = utils

PROG = \
	smartbuildrev \
	smartmkrelease \
	smartmktag

INSTALL_PROG = install -m 775
INSTALL_DATA = install -m 644

ifeq ($(origin BINDIR), undefined)
  bindir = $(PREFIX)/bin
else
  bindir = $(BINDIR)
endif

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

rpm: 	clean
	rm -f smartmet-$(MODULE).tar.gz
	tar -czvf smartmet-$(MODULE).tar.gz --exclude-vcs --transform "s,^,$(MODULE)/," *
	rpmbuild --target noarch -tb smartmet-$(MODULE).tar.gz
	rm -f smartmet-$(MODULE).tar.gz
