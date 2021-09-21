ABI_CHECK_LIBNAME := $(patsubst %.so,%,$(LIBFILE))

ifneq ($(rev1),)
REV1_FN := $(subst /,_,$(rev1))
REV1_ABI_DUMP_TARGET := rev1-abi-dump
ABI_REV1_BUILD := rev1-build
ABI_DUMP_REV1 := abi-check/dumps/$(ABI_CHECK_LIBNAME)-$(REV1_FN).dump
ifeq ($(rev2),)
REV2_ABI_DUMP_TARGET := current-abi-dump
REV2_FN := current
ABI_REV2_BUILD := $(LIBFILE)
ABI_DUMP_REV2 := abi-check/dumps/$(ABI_CHECK_LIBNAME)-current.dump
ABI_REV2_LIB := $(LIBFILE)
else
REV2_ABI_DUMP_TARGET := rev2-abi-dump
REV2_FN := $(subst /,_,$(rev2))
ABI_REV2_BUILD := rev2-build
ABI_DUMP_REV2 := abi-check/dumps/$(ABI_CHECK_LIBNAME)-$(REV2_FN)).dump
ABI_REV2_LIB := abi-check/$(ABI_CHECK_LIBNAME)-$(REV2_FN)/$(ABI_CHECK_LIBNAME).so
endif
endif

.PHONY:	abi-check rev1-tree rev2-tree

abi-check:
	$(MAKE) abi-check-clean-builds
	$(MAKE) do-abi-compliance-check

rev1-tree: abi-check-dir
	if [ -z "$(rev1)" ] ; then \
	    echo "make abi-check: mandatory parameter rev1 mising"; \
	    false; \
	else \
	    git archive --prefix=$(ABI_CHECK_LIBNAME)-$(REV1_FN)/ --format tar $(rev1): | tar x -C abi-check; \
	fi

rev2-tree: abi-check-dir
	if [ -z "$(rev2)" ] ; then \
	    echo "make abi-check: mandatory parameter rev2 mising"; \
	    false; \
	else \
	    git archive --prefix=$(ABI_CHECK_LIBNAME)-$(REV2_FN)/ --format tar $(rev2): | tar x -C abi-check; \
	fi

abi-check-dir:
	mkdir -p abi-check/dumps

rev1-build: rev1-tree
	$(MAKE) -C abi-check/$(ABI_CHECK_LIBNAME)-$(REV1_FN) debug

rev2-build: rev2-tree
	$(MAKE) -C abi-check/$(ABI_CHECK_LIBNAME)-$(REV2_FN) debug

rev1-abi-dump: rev1-build
	abi-dumper \
		   abi-check/$(ABI_CHECK_LIBNAME)-$(REV1_FN)/$(ABI_CHECK_LIBNAME).so \
		   -o abi-check/dumps/$(ABI_CHECK_LIBNAME)-$(REV1_FN).dump \
		   --lver $(rev1)

rev2-abi-dump: rev2-build
	abi-dumper abi-check/$(ABI_CHECK_LIBNAME)-$(REV2_FN)/$(ABI_CHECK_LIBNAME).so \
		   -o abi-check/dumps/$(ABI_CHECK_LIBNAME)-$(REV2_FN).dump \
		   --lver $(rev2)

abi-check/dumps/$(ABI_CHECK_LIBNAME)-current.dump: $(LIBFILE)
	abi-dumper $(LIBFILE) -o $@ --lver current

current-abi-dump: abi-check/dumps/$(ABI_CHECK_LIBNAME)-current.dump

do-abi-compliance-check: $(REV1_ABI_DUMP_TARGET) $(REV2_ABI_DUMP_TARGET)
	abi-compliance-checker \
	    -l $(ABI_CHECK_LIBNAME) \
	    -old $(ABI_DUMP_REV1) \
	    -new $(ABI_DUMP_REV2) | tee abi-check/result.txt

abi-check-clean-builds:
	rm -rf abi-check/$(ABI_CHECK_LIBNAME)-*

abi-check-clean: abi-check-clean-builds
	rm -rf abi-check
	rm -rf compat_reports

CLEAN_TARGETS += abi-check-clean
