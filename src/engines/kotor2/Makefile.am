include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = libkotor2.la

noinst_HEADERS = \
                 kotor2.h \
                 probes.h \
                 types.h \
                 console.h \
                 modelloader.h \
                 game.h \
                 object.h \
                 situated.h \
                 placeable.h \
                 door.h \
                 creature.h \
                 area.h \
                 room.h \
                 module.h \
                 gui/widgets/kotorwidget.h \
                 gui/gui.h \
                 gui/main/main.h \
                 $(EMPTY)

libkotor2_la_SOURCES = \
                       kotor2.cpp \
                       probes.cpp \
                       console.cpp \
                       modelloader.cpp \
                       game.cpp \
                       object.cpp \
                       situated.cpp \
                       placeable.cpp \
                       door.cpp \
                       creature.cpp \
                       area.cpp \
                       room.cpp \
                       module.cpp \
                       gui/main/main.cpp \
                       $(EMPTY)

libkotor2_la_LIBADD = \
                      ../kotor/libkotor.la \
                      $(EMPTY)
