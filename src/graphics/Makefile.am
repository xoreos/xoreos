include $(top_srcdir)/Makefile.common

SUBDIRS = images video

noinst_LTLIBRARIES = libgraphics.la

noinst_HEADERS = types.h \
                 glew/glew.h \
                 glew/glxew.h \
                 glew/wglew.h \
                 util.h \
                 graphics.h \
                 fpscounter.h \
                 queueable.h \
                 listcontainer.h \
                 texture.h \
                 font.h \
                 renderable.h \
                 object.h \
                 guifrontelement.h \
                 text.h \
                 cube.h

libgraphics_la_SOURCES = glew/glew.c \
                         graphics.cpp \
                         fpscounter.cpp \
                         listcontainer.cpp \
                         texture.cpp \
                         font.cpp \
                         renderable.cpp \
                         object.cpp \
                         guifrontelement.cpp \
                         text.cpp \
                         cube.cpp

libgraphics_la_LIBADD = images/libimages.la video/libvideo.la
