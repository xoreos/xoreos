include $(top_srcdir)/Makefile.common

SUBDIRS = images video aurora

noinst_LTLIBRARIES = libgraphics.la

noinst_HEADERS = types.h \
                 glew/glew.h \
                 glew/glxew.h \
                 glew/wglew.h \
                 util.h \
                 graphics.h \
                 fpscounter.h \
                 cursor.h \
                 queueable.h \
                 glcontainer.h \
                 listcontainer.h \
                 texture.h \
                 font.h \
                 camera.h \
                 renderable.h \
                 object.h \
                 guifrontelement.h

libgraphics_la_SOURCES = glew/glew.c \
                         graphics.cpp \
                         fpscounter.cpp \
                         cursor.cpp \
                         glcontainer.cpp \
                         listcontainer.cpp \
                         texture.cpp \
                         font.cpp \
                         camera.cpp \
                         renderable.cpp \
                         object.cpp \
                         guifrontelement.cpp

libgraphics_la_LIBADD = images/libimages.la video/libvideo.la aurora/libaurora.la
