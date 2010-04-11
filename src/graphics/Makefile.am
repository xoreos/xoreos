include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = libgraphics.la

noinst_HEADERS = types.h \
                 glew/glew.h \
                 glew/glxew.h \
                 glew/wglew.h \
                 images/decoder.h \
                 images/tga.h \
                 images/dumpppm.h \
                 graphics.h \
                 fpscounter.h \
                 renderable.h \
                 cube.h

libgraphics_la_SOURCES = glew/glew.c \
                         images/tga.cpp \
                         images/dumpppm.cpp \
                         graphics.cpp \
                         fpscounter.cpp \
                         renderable.cpp \
                         cube.cpp
