include $(top_srcdir)/Makefile.common

# Get git version information
.PHONY: gitstamp
gitstamp:
	@if ! $(AM_V_P); then echo "  UPD      gitstamp"; fi
	@-$(eval $@_REVLINE := $(shell git describe --long --match desc/\* | cut -d '/' -f 2-))
	@-$(eval $@_REVDIRT := $(shell update-index --refresh --unmerged 1>/dev/null 2>&1; git diff-index --quiet HEAD || echo "-dirty"))
	@-$(eval $@_REVDESC := $(shell if test x$($@_REVLINE) != "x"; then echo $($@_REVLINE)$($@_REVDIRT); fi ))
	@[ -f $@ ] || touch $@
	@echo $($@_REVDESC) | cmp -s $@ - || echo $($@_REVDESC) > $@

clean-local:
	rm -f gitstamp

noinst_LTLIBRARIES = libgitstamp.la

libgitstamp_la_SOURCES =

libgitstamp_la_DEPENDENCIES = gitstamp

libgitstamp_la_LINK = /bin/true
