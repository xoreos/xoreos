include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = libkotor2.la

noinst_HEADERS = \
                 kotor2.h \
                 probes.h \
                 modelloader.h \
                 console.h \
                 module.h \
                 gui/widgets/kotorwidget.h \
                 gui/gui.h \
                 gui/main/main.h \
                 $(EMPTY)

libkotor2_la_SOURCES = \
                       kotor2.cpp \
                       probes.cpp \
                       modelloader.cpp \
                       console.cpp \
                       gui/main/main.cpp \
                       $(EMPTY)

libkotor2_la_LIBADD = \
                      ../kotor/libkotor.la \
                      $(EMPTY)
