include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = libgraphics.la

noinst_HEADERS = types.h \
                 glew/glew.h \
                 glew/glxew.h \
                 glew/wglew.h \
                 images/s3tc.h \
                 images/decoder.h \
                 images/tga.h \
                 images/dds.h \
                 images/tpc.h \
                 images/dumpppm.h \
                 graphics.h \
                 fpscounter.h \
                 texture.h \
                 renderable.h \
                 cube.h

libgraphics_la_SOURCES = glew/glew.c \
                         images/s3tc.cpp \
                         images/decoder.cpp \
                         images/tga.cpp \
                         images/dds.cpp \
                         images/tpc.cpp \
                         images/dumpppm.cpp \
                         graphics.cpp \
                         fpscounter.cpp \
                         texture.cpp \
                         renderable.cpp \
                         cube.cpp
