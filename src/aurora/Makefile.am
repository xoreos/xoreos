include $(top_srcdir)/Makefile.common

SUBDIRS = \
          nwscript \
          $(EMPTY)

noinst_LTLIBRARIES = libaurora.la

noinst_HEADERS = \
                 types.h \
                 error.h \
                 util.h \
                 archive.h \
                 aurorafile.h \
                 keyfile.h \
                 biffile.h \
                 bzffile.h \
                 erffile.h \
                 rimfile.h \
                 ndsrom.h \
                 zipfile.h \
                 resman.h \
                 talktable.h \
                 talkman.h \
                 ssffile.h \
                 2dafile.h \
                 2dareg.h \
                 locstring.h \
                 gfffile.h \
                 gffstructs.h \
                 dlgfile.h \
                 lytfile.h \
                 visfile.h \
                 ifofile.h \
                 pefile.h \
                 herffile.h \
                 $(EMPTY)

libaurora_la_SOURCES = \
                       error.cpp \
                       util.cpp \
                       archive.cpp \
                       aurorafile.cpp \
                       keyfile.cpp \
                       biffile.cpp \
                       bzffile.cpp \
                       erffile.cpp \
                       rimfile.cpp \
                       ndsrom.cpp \
                       zipfile.cpp \
                       resman.cpp \
                       talktable.cpp \
                       talkman.cpp \
                       ssffile.cpp \
                       2dafile.cpp \
                       2dareg.cpp \
                       locstring.cpp \
                       gfffile.cpp \
                       gffstructs.cpp \
                       dlgfile.cpp \
                       lytfile.cpp \
                       visfile.cpp \
                       ifofile.cpp \
                       pefile.cpp \
                       herffile.cpp \
                       $(EMPTY)

libaurora_la_LIBADD = \
                      nwscript/libnwscript.la \
                      $(EMPTY)
