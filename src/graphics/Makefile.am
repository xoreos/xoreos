include $(top_srcdir)/Makefile.common

SUBDIRS = \
          images \
          aurora \
          $(EMPTY)

noinst_LTLIBRARIES = libgraphics.la

noinst_HEADERS = \
                 macosx/macosx.h \
                 types.h \
                 util.h \
                 graphics.h \
                 renderer.h \
                 yuv_to_rgb.h \
                 textureman.h \
                 materialman.h \
                 cursor.h \
                 cursorman.h \
                 cameraman.h \
                 $(EMPTY)

libgraphics_la_SOURCES = \
                         graphics.cpp \
                         renderer.cpp \
                         yuv_to_rgb.cpp \
                         textureman.cpp \
                         materialman.cpp \
                         cursor.cpp \
                         cursorman.cpp \
                         cameraman.cpp \
                         $(EMPTY)

if DARWIN
libgraphics_la_SOURCES += macosx/macosx.mm
endif

libgraphics_la_LIBADD = \
                        images/libimages.la \
                        aurora/libaurora.la \
                        $(EMPTY)
