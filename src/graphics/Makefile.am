include $(top_srcdir)/Makefile.common

SUBDIRS = \
          images \
          aurora \
          mesh \
          render \
          shader \
          $(EMPTY)

noinst_LTLIBRARIES = libgraphics.la

noinst_HEADERS = \
                 types.h \
                 util.h \
                 graphics.h \
                 fpscounter.h \
                 icon.h \
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
                 guibackelement.h \
                 yuv_to_rgb.h \
                 ttf.h \
                 indexbuffer.h \
                 vertexbuffer.h \
                 $(EMPTY)

libgraphics_la_SOURCES = \
                         graphics.cpp \
                         fpscounter.cpp \
                         icon.cpp \
                         cursor.cpp \
                         queueman.cpp \
                         queueable.cpp \
                         glcontainer.cpp \
                         texture.cpp \
                         font.cpp \
                         camera.cpp \
                         renderable.cpp \
                         yuv_to_rgb.cpp \
                         ttf.cpp \
                         indexbuffer.cpp \
                         vertexbuffer.cpp \
                         $(EMPTY)

libgraphics_la_LIBADD = \
                        images/libimages.la \
                        aurora/libaurora.la \
                        shader/libshader.la \
                        mesh/libmesh.la \
                        render/librender.la \
                        ../../glew/libglew.la \
                        $(EMPTY)
