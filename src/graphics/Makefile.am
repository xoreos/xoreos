include $(top_srcdir)/Makefile.common

SUBDIRS = images

noinst_LTLIBRARIES = libgraphics.la

noinst_HEADERS = types.h \
                 glew/glew.h \
                 glew/glxew.h \
                 glew/wglew.h \
                 graphics.h \
                 fpscounter.h \
                 texture.h \
                 renderable.h \
                 cube.h

libgraphics_la_SOURCES = glew/glew.c \
                         graphics.cpp \
                         fpscounter.cpp \
                         texture.cpp \
                         renderable.cpp \
                         cube.cpp

libgraphics_la_LIBADD = images/libimages.la
