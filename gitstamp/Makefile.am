AM_LDFLAGS =
LIBS =

# Get git version information
.PHONY: gitstamp
gitstamp:
	@-$(eval $@_REVLINE := $(shell git describe --long --match desc/\* | cut -d '/' -f 2- | sed -e 's/\(.*\)-\([^-]*\)-\([^-]*\)/\1+\2.\3/'))
	@-$(eval $@_REVDIRT := $(shell update-index --refresh --unmerged 1>/dev/null 2>&1; git diff-index --quiet HEAD || echo ".dirty"))
	@-$(eval $@_REVDESC := $(shell if test x$($@_REVLINE) != "x"; then echo $($@_REVLINE)$($@_REVDIRT); fi ))
	@[ -f $@ ] || touch $@
	$(AM_V_GEN)echo $($@_REVDESC) | cmp -s $@ - || echo $($@_REVDESC) > $@

clean-local:
	rm -f gitstamp

noinst_LTLIBRARIES = libgitstamp.la

libgitstamp_la_SOURCES =

libgitstamp_la_DEPENDENCIES = gitstamp

libgitstamp_la_LINK = true
