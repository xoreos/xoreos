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
                 fpscounter.h \
                 cursor.h \
                 queueman.h \
                 queueable.h \
                 glcontainer.h \
                 texture.h \
                 font.h \
                 camera.h \
                 renderable.h \
                 object.h \
                 guifrontelement.h \
                 yuv_to_rgb.h \
                 ttf.h \
                 indexbuffer.h \
                 vertexbuffer.h \
                 $(EMPTY)

libgraphics_la_SOURCES = \
                         graphics.cpp \
                         fpscounter.cpp \
                         cursor.cpp \
                         queueman.cpp \
                         queueable.cpp \
                         glcontainer.cpp \
                         texture.cpp \
                         font.cpp \
                         camera.cpp \
                         renderable.cpp \
                         object.cpp \
                         guifrontelement.cpp \
                         yuv_to_rgb.cpp \
                         ttf.cpp \
                         indexbuffer.cpp \
                         vertexbuffer.cpp \
                         $(EMPTY)

if DARWIN
libgraphics_la_SOURCES += macosx/macosx.mm
endif

libgraphics_la_LIBADD = \
                        images/libimages.la \
                        aurora/libaurora.la \
                        ../../glew/libglew.la \
                        $(EMPTY)
